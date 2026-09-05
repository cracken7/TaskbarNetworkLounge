// --- Network monitoring ----------------------------------------------------
// One GetIfTable2 call per interval on a worker thread. Speed comes from the
// delta of the interface octet counters divided by the *measured* elapsed time
// (QueryPerformanceCounter), never from an assumed timer period.

struct TrackedInterface {
    NET_LUID luid{};
    NET_IFINDEX index = 0;
    unsigned long long lastIn = 0;
    unsigned long long lastOut = 0;
    bool seen = false;
};

struct InterfaceCandidate {
    NET_LUID luid{};
    NET_IFINDEX index = 0;
    std::wstring alias;
    std::wstring description;
    IFTYPE type = 0;
    unsigned long long inOctets = 0;
    unsigned long long outOctets = 0;
    bool up = false;
    bool physical = false;
};

class NetworkMonitor {
   public:
    void Initialize() {
        QueryPerformanceFrequency(&m_frequency);
        m_lastCounter.QuadPart = 0;
        m_haveBaseline = false;

        ModSettings s = GetSettings();
        if (s.counterMode == CounterMode::Persistent) {
            PersistentTotals stored;
            if (LoadPersistentTotals(&stored)) {
                m_totalDown = stored.down;
                m_totalUp = stored.up;
                Wh_Log(L"Traffic counter loaded: down=%llu up=%llu", stored.down,
                       stored.up);
            } else {
                Wh_Log(L"No stored traffic counter, starting from zero");
            }
        }
        m_lastSave = GetTickCount64();
    }

    void Shutdown() {
        FlushTotals(true);
    }

    // Returns true when the visible values changed enough to justify a repaint.
    bool Sample(NetSnapshot* out) {
        ModSettings s = GetSettings();

        unsigned long long resetFlags = g_pendingReset.exchange(0);
        if (resetFlags & 1) {
            m_totalDown = 0;
        }
        if (resetFlags & 2) {
            m_totalUp = 0;
        }
        if (resetFlags) {
            Wh_Log(L"Traffic counters reset (flags=%llu)", resetFlags);
            FlushTotals(true);
        }

        if (s.counterMode != m_lastCounterMode) {
            // Switching Session <-> Persistent: reload or restart the totals.
            if (s.counterMode == CounterMode::Persistent) {
                PersistentTotals stored;
                if (LoadPersistentTotals(&stored)) {
                    m_totalDown = stored.down;
                    m_totalUp = stored.up;
                } 
            } else {
                m_totalDown = 0;
                m_totalUp = 0;
            }
            m_lastCounterMode = s.counterMode;
        }

        MIB_IF_TABLE2* table = nullptr;
        NETIO_STATUS status = GetIfTable2(&table);
        if (status != NO_ERROR || !table) {
            if (table) {
                FreeMibTable(table);
            }
            if (!m_loggedTableFailure) {
                Wh_Log(L"GetIfTable2 failed (status=%lu)", (unsigned long)status);
                m_loggedTableFailure = true;
            }
            NetSnapshot snapshot = MakeDisconnectedSnapshot();
            bool changed = SnapshotDiffers(snapshot);
            *out = snapshot;
            m_lastPublished = snapshot;
            return changed;
        }
        m_loggedTableFailure = false;

        std::vector<InterfaceCandidate> candidates;
        candidates.reserve(table->NumEntries);

        for (ULONG i = 0; i < table->NumEntries; i++) {
            const MIB_IF_ROW2& row = table->Table[i];

            InterfaceCandidate candidate;
            candidate.luid = row.InterfaceLuid;
            candidate.index = row.InterfaceIndex;
            candidate.alias = row.Alias;
            candidate.description = row.Description;
            candidate.type = row.Type;
            candidate.inOctets = row.InOctets;
            candidate.outOctets = row.OutOctets;
            candidate.up = row.OperStatus == IfOperStatusUp &&
                           row.MediaConnectState == MediaConnectStateConnected;
            candidate.physical =
                row.InterfaceAndOperStatusFlags.HardwareInterface &&
                !row.InterfaceAndOperStatusFlags.FilterInterface;

            if (!IsUsableInterface(row, s)) {
                continue;
            }
            candidates.push_back(std::move(candidate));
        }

        FreeMibTable(table);

        std::vector<InterfaceCandidate> selected = SelectInterfaces(candidates, s);

        // --- elapsed time ---------------------------------------------------
        LARGE_INTEGER now;
        QueryPerformanceCounter(&now);
        double elapsedSeconds = 0.0;
        if (m_lastCounter.QuadPart != 0 && m_frequency.QuadPart != 0) {
            elapsedSeconds = (double)(now.QuadPart - m_lastCounter.QuadPart) /
                             (double)m_frequency.QuadPart;
        }
        m_lastCounter = now;

        // --- deltas ---------------------------------------------------------
        unsigned long long deltaIn = 0;
        unsigned long long deltaOut = 0;

        for (auto& tracked : m_tracked) {
            tracked.seen = false;
        }

        for (const auto& candidate : selected) {
            TrackedInterface* tracked = FindTracked(candidate.luid);
            if (!tracked) {
                TrackedInterface fresh;
                fresh.luid = candidate.luid;
                fresh.index = candidate.index;
                fresh.lastIn = candidate.inOctets;
                fresh.lastOut = candidate.outOctets;
                fresh.seen = true;
                m_tracked.push_back(fresh);
                continue;  // no delta on the first sighting
            }

            tracked->seen = true;
            tracked->index = candidate.index;

            // Counters can go backwards after a driver reset / adapter reset.
            if (candidate.inOctets >= tracked->lastIn) {
                deltaIn += candidate.inOctets - tracked->lastIn;
            }
            if (candidate.outOctets >= tracked->lastOut) {
                deltaOut += candidate.outOctets - tracked->lastOut;
            }
            tracked->lastIn = candidate.inOctets;
            tracked->lastOut = candidate.outOctets;
        }

        m_tracked.erase(std::remove_if(m_tracked.begin(), m_tracked.end(),
                                       [](const TrackedInterface& t) {
                                           return !t.seen;
                                       }),
                        m_tracked.end());

        // Ignore absurd deltas (sleep/wake, counter wrap) instead of showing a
        // fake multi-gigabyte spike.
        const bool plausible = m_haveBaseline && elapsedSeconds > 0.0005 &&
                               elapsedSeconds < 30.0;

        double downRate = 0.0;
        double upRate = 0.0;
        if (plausible) {
            downRate = (double)deltaIn / elapsedSeconds;
            upRate = (double)deltaOut / elapsedSeconds;
            m_totalDown += deltaIn;
            m_totalUp += deltaOut;
        }
        m_haveBaseline = true;

        // Light smoothing so the text does not jitter between two samples.
        m_downRate = m_downRate * 0.35 + downRate * 0.65;
        m_upRate = m_upRate * 0.35 + upRate * 0.65;
        if (m_downRate < 1.0) {
            m_downRate = downRate;
        }
        if (m_upRate < 1.0) {
            m_upRate = upRate;
        }

        NetSnapshot snapshot;
        snapshot.downBytesPerSec = m_downRate;
        snapshot.upBytesPerSec = m_upRate;
        snapshot.totalDown = m_totalDown;
        snapshot.totalUp = m_totalUp;
        snapshot.ifCount = (int)selected.size();
        snapshot.haveInterface = !selected.empty();
        snapshot.connected = !selected.empty();

        if (selected.empty()) {
            snapshot.ifName = candidates.empty() ? L"No Network" : L"Disconnected";
            snapshot.ifKind = L"";
            snapshot.ipv4 = L"";
        } else if (selected.size() == 1) {
            snapshot.ifName = selected[0].alias;
            snapshot.ifKind = DescribeType(selected[0].type);
            snapshot.ipv4 = GetIPv4(selected[0].index);
        } else {
            wchar_t buffer[64];
            swprintf_s(buffer, L"%d interfaces", (int)selected.size());
            snapshot.ifName = buffer;
            snapshot.ifKind = L"Multiple";
            snapshot.ipv4 = GetIPv4(selected[0].index);
        }

        LogInterfaceChange(snapshot, selected);

        if (s.counterMode == CounterMode::Persistent) {
            FlushTotals(false);
        }

        bool changed = SnapshotDiffers(snapshot);
        m_lastPublished = snapshot;
        *out = snapshot;
        return changed;
    }

    void ForceSaveNow() {
        FlushTotals(true);
    }

   private:
    static bool IsUsableInterface(const MIB_IF_ROW2& row, const ModSettings& s) {
        if (row.Type == IF_TYPE_SOFTWARE_LOOPBACK) {
            return false;
        }
        if (row.InterfaceAndOperStatusFlags.FilterInterface) {
            return false;  // NDIS filter / capture pseudo-interfaces
        }

        if (!s.excludeVirtual) {
            return true;
        }

        if (row.Type == IF_TYPE_TUNNEL || row.TunnelType != TUNNEL_TYPE_NONE) {
            return false;
        }
        if (!row.InterfaceAndOperStatusFlags.HardwareInterface) {
            return false;
        }

        std::wstring haystack = ToLower(std::wstring(row.Description) + L" " +
                                       std::wstring(row.Alias));
        static const wchar_t* kVirtualMarkers[] = {
            L"vmware",      L"virtualbox",  L"vbox",       L"hyper-v",
            L"vethernet",   L"docker",      L"loopback",   L"tap-windows",
            L"tap adapter", L"openvpn",     L"wintun",     L"wireguard",
            L"tunnel",      L"vpn",         L"zerotier",   L"tailscale",
            L"nordlynx",    L"proton",      L"expressvpn", L"hamachi",
            L"radmin",      L"pseudo",      L"teredo",     L"isatap",
            L"6to4",        L"npcap",       L"wan minipor", L"bluetooth",
            L"virtual",
        };
        for (const wchar_t* marker : kVirtualMarkers) {
            if (Contains(haystack, marker)) {
                return false;
            }
        }
        return true;
    }

    static std::wstring DescribeType(IFTYPE type) {
        switch (type) {
            case IF_TYPE_ETHERNET_CSMACD:
                return L"Ethernet";
            case IF_TYPE_IEEE80211:
                return L"Wi-Fi";
            case IF_TYPE_PPP:
                return L"PPP";
            case IF_TYPE_TUNNEL:
                return L"Tunnel";
            case IF_TYPE_IEEE1394:
                return L"FireWire";
            case IF_TYPE_SOFTWARE_LOOPBACK:
                return L"Loopback";
            default:
                return L"Network";
        }
    }

    // Auto mode: prefer the adapter carrying the default route (the one Windows
    // would use to reach the internet). Falls back to the busiest active
    // adapter, and logs the decision.
    std::vector<InterfaceCandidate> SelectInterfaces(
        const std::vector<InterfaceCandidate>& candidates,
        const ModSettings& s) {
        std::vector<InterfaceCandidate> result;

        auto pushActive = [&result](const InterfaceCandidate& c) {
            if (c.up) {
                result.push_back(c);
            }
        };

        switch (s.ifMode) {
            case InterfaceMode::All:
                for (const auto& c : candidates) {
                    pushActive(c);
                }
                return result;

            case InterfaceMode::Ethernet:
                for (const auto& c : candidates) {
                    if (c.type == IF_TYPE_ETHERNET_CSMACD) {
                        pushActive(c);
                    }
                }
                return result;

            case InterfaceMode::Wifi:
                for (const auto& c : candidates) {
                    if (c.type == IF_TYPE_IEEE80211) {
                        pushActive(c);
                    }
                }
                return result;

            case InterfaceMode::Specific: {
                if (s.selectedInterface.empty()) {
                    break;  // behave like Auto
                }
                std::wstring needle = ToLower(s.selectedInterface);
                for (const auto& c : candidates) {
                    std::wstring haystack =
                        ToLower(c.alias + L" " + c.description);
                    if (haystack.find(needle) != std::wstring::npos) {
                        pushActive(c);
                    }
                }
                if (!result.empty()) {
                    return result;
                }
                if (!m_loggedSpecificMiss) {
                    Wh_Log(L"Specific interface '%s' not found or not active, using Auto",
                           s.selectedInterface.c_str());
                    m_loggedSpecificMiss = true;
                }
                break;
            }

            case InterfaceMode::Auto:
            default:
                break;
        }

        // --- Auto ------------------------------------------------------------
        NET_IFINDEX bestIndex = GetInternetInterfaceIndex();
        if (bestIndex != 0) {
            for (const auto& c : candidates) {
                if (c.index == bestIndex && c.up) {
                    result.push_back(c);
                    return result;
                }
            }
        }

        // Fall back to the active interface with the most traffic so far.
        const InterfaceCandidate* best = nullptr;
        unsigned long long bestTraffic = 0;
        for (const auto& c : candidates) {
            if (!c.up) {
                continue;
            }
            unsigned long long traffic = c.inOctets + c.outOctets;
            if (!best || traffic > bestTraffic) {
                best = &c;
                bestTraffic = traffic;
            }
        }
        if (best) {
            if (!m_loggedAutoFallback) {
                Wh_Log(L"Auto mode: default-route lookup unavailable, using busiest active interface '%s'",
                       best->alias.c_str());
                m_loggedAutoFallback = true;
            }
            result.push_back(*best);
        }
        return result;
    }

    static NET_IFINDEX GetInternetInterfaceIndex() {
        SOCKADDR_IN dest{};
        dest.sin_family = AF_INET;
        dest.sin_addr.S_un.S_addr = htonl(0x08080808);  // 8.8.8.8, no traffic sent
        DWORD index = 0;
        if (GetBestInterfaceEx((sockaddr*)&dest, &index) == NO_ERROR) {
            return (NET_IFINDEX)index;
        }
        return 0;
    }

    static std::wstring GetIPv4(NET_IFINDEX index) {
        ULONG size = 0;
        const ULONG flags = GAA_FLAG_SKIP_ANYCAST | GAA_FLAG_SKIP_MULTICAST |
                            GAA_FLAG_SKIP_DNS_SERVER | GAA_FLAG_SKIP_FRIENDLY_NAME;
        if (GetAdaptersAddresses(AF_INET, flags, nullptr, nullptr, &size) !=
                ERROR_BUFFER_OVERFLOW ||
            size == 0) {
            return L"";
        }

        std::vector<BYTE> buffer(size);
        IP_ADAPTER_ADDRESSES* adapters = (IP_ADAPTER_ADDRESSES*)buffer.data();
        if (GetAdaptersAddresses(AF_INET, flags, nullptr, adapters, &size) !=
            NO_ERROR) {
            return L"";
        }

        for (IP_ADAPTER_ADDRESSES* adapter = adapters; adapter;
             adapter = adapter->Next) {
            if (adapter->IfIndex != index) {
                continue;
            }
            for (IP_ADAPTER_UNICAST_ADDRESS* unicast =
                     adapter->FirstUnicastAddress;
                 unicast; unicast = unicast->Next) {
                if (!unicast->Address.lpSockaddr ||
                    unicast->Address.lpSockaddr->sa_family != AF_INET) {
                    continue;
                }
                sockaddr_in* addr = (sockaddr_in*)unicast->Address.lpSockaddr;
                BYTE* octets = (BYTE*)&addr->sin_addr;
                if (octets[0] == 169 && octets[1] == 254) {
                    continue;  // link-local, not useful
                }
                wchar_t text[32];
                swprintf_s(text, L"%u.%u.%u.%u", octets[0], octets[1], octets[2],
                           octets[3]);
                return text;
            }
        }
        return L"";
    }

    TrackedInterface* FindTracked(const NET_LUID& luid) {
        for (auto& tracked : m_tracked) {
            if (tracked.luid.Value == luid.Value) {
                return &tracked;
            }
        }
        return nullptr;
    }

    NetSnapshot MakeDisconnectedSnapshot() const {
        NetSnapshot snapshot;
        snapshot.downBytesPerSec = 0.0;
        snapshot.upBytesPerSec = 0.0;
        snapshot.totalDown = m_totalDown;
        snapshot.totalUp = m_totalUp;
        snapshot.ifName = L"No Network";
        snapshot.connected = false;
        snapshot.haveInterface = false;
        return snapshot;
    }

    bool SnapshotDiffers(const NetSnapshot& snapshot) const {
        auto differsBy = [](double a, double b) {
            double diff = a > b ? a - b : b - a;
            double scale = a > b ? a : b;
            return diff > 512.0 || diff > scale * 0.01;
        };
        return differsBy(snapshot.downBytesPerSec,
                         m_lastPublished.downBytesPerSec) ||
               differsBy(snapshot.upBytesPerSec, m_lastPublished.upBytesPerSec) ||
               snapshot.totalDown != m_lastPublished.totalDown ||
               snapshot.totalUp != m_lastPublished.totalUp ||
               snapshot.connected != m_lastPublished.connected ||
               snapshot.ifName != m_lastPublished.ifName ||
               snapshot.ipv4 != m_lastPublished.ipv4;
    }

    void LogInterfaceChange(const NetSnapshot& snapshot,
                            const std::vector<InterfaceCandidate>& selected) {
        std::wstring signature;
        for (const auto& c : selected) {
            signature += c.alias;
            signature += L'|';
        }
        if (signature == m_lastSignature) {
            return;
        }
        m_lastSignature = signature;
        m_loggedAutoFallback = false;
        m_loggedSpecificMiss = false;

        if (selected.empty()) {
            Wh_Log(L"Network interface changed: none active (%s)",
                   snapshot.ifName.c_str());
        } else {
            Wh_Log(L"Selected interface: %s (%s, %d total) ipv4=%s",
                   snapshot.ifName.c_str(), snapshot.ifKind.c_str(),
                   snapshot.ifCount,
                   snapshot.ipv4.empty() ? L"n/a" : snapshot.ipv4.c_str());
        }
    }

    // Batched, atomic persistence: at most one write every 30 seconds, plus one
    // on unload / reset.
    void FlushTotals(bool force) {
        ModSettings s = GetSettings();
        if (s.counterMode != CounterMode::Persistent && !force) {
            return;
        }
        if (s.counterMode != CounterMode::Persistent && force) {
            return;  // session mode never touches the file
        }

        ULONGLONG now = GetTickCount64();
        if (!force && now - m_lastSave < 30000) {
            return;
        }
        if (!force && m_totalDown == m_savedDown && m_totalUp == m_savedUp) {
            return;
        }

        PersistentTotals totals;
        totals.down = m_totalDown;
        totals.up = m_totalUp;
        if (SavePersistentTotals(totals)) {
            m_savedDown = m_totalDown;
            m_savedUp = m_totalUp;
            m_lastSave = now;
            Wh_Log(L"Traffic counter saved: down=%llu up=%llu", totals.down,
                   totals.up);
        }
    }

    LARGE_INTEGER m_frequency{};
    LARGE_INTEGER m_lastCounter{};
    bool m_haveBaseline = false;
    bool m_loggedTableFailure = false;
    bool m_loggedAutoFallback = false;
    bool m_loggedSpecificMiss = false;

    std::vector<TrackedInterface> m_tracked;
    double m_downRate = 0.0;
    double m_upRate = 0.0;
    unsigned long long m_totalDown = 0;
    unsigned long long m_totalUp = 0;
    unsigned long long m_savedDown = ULLONG_MAX;
    unsigned long long m_savedUp = ULLONG_MAX;
    ULONGLONG m_lastSave = 0;
    CounterMode m_lastCounterMode = CounterMode::Session;
    NetSnapshot m_lastPublished;
    std::wstring m_lastSignature;
};

static NetworkMonitor g_monitor;

// Worker thread: samples the counters and only wakes the UI when something
// visible changed. Waits on events, so it never busy-waits.
static void NetworkWorkerThread() {
    g_monitor.Initialize();

    HANDLE waitHandles[2] = {g_stopEvent, g_wakeEvent};

    for (;;) {
        NetSnapshot snapshot;
        bool changed = false;
        try {
            changed = g_monitor.Sample(&snapshot);
        } catch (...) {
            Wh_Log(L"Exception while sampling network counters");
            snapshot = NetSnapshot();
            snapshot.ifName = L"No Network";
            changed = true;
        }

        {
            std::lock_guard<std::mutex> guard(g_snapshotMutex);
            g_snapshot = snapshot;
        }

        if (changed && g_hWidget) {
            PostMessage(g_hWidget, APP_WM_DATA_UPDATED, 0, 0);
        }

        int interval = GetSettings().updateInterval;
        DWORD wait = WaitForMultipleObjects(2, waitHandles, FALSE, (DWORD)interval);
        if (wait == WAIT_OBJECT_0) {
            break;  // stop event
        }
        // WAIT_OBJECT_0 + 1 (wake) or WAIT_TIMEOUT: sample again immediately.
    }

    g_monitor.Shutdown();
}
