// --- Small helpers ---------------------------------------------------------
static std::wstring ToLower(std::wstring s) {
    for (auto& c : s) {
        c = (wchar_t)towlower(c);
    }
    return s;
}

static bool Contains(const std::wstring& haystackLower, const wchar_t* needleLower) {
    return haystackLower.find(needleLower) != std::wstring::npos;
}

static int ClampInt(int v, int lo, int hi) {
    return v < lo ? lo : (v > hi ? hi : v);
}

// --- Settings --------------------------------------------------------------
static std::wstring ReadStringSetting(PCWSTR name) {
    std::wstring result;
    PCWSTR value = Wh_GetStringSetting(name);
    if (value) {
        result = value;
        Wh_FreeStringSetting(value);
    }
    return result;
}

static bool StringSettingIs(const std::wstring& value, const wchar_t* expected) {
    return _wcsicmp(value.c_str(), expected) == 0;
}

static void LoadSettings() {
    ModSettings s;

    s.width = ClampInt(Wh_GetIntSetting(L"Appearance.PanelWidth"), 80, 1200);
    s.height = ClampInt(Wh_GetIntSetting(L"Appearance.PanelHeight"), 20, 200);
    s.fontSize = ClampInt(Wh_GetIntSetting(L"Appearance.FontSize"), 6, 40);
    s.boldText = Wh_GetIntSetting(L"Appearance.BoldText") != 0;

    std::wstring weight = ReadStringSetting(L"Appearance.TextWeight");
    if (StringSettingIs(weight, L"black")) {
        s.textWeight = TextWeight::Black;
    } else if (StringSettingIs(weight, L"semibold")) {
        s.textWeight = TextWeight::Semibold;
    } else if (StringSettingIs(weight, L"regular")) {
        s.textWeight = TextWeight::Regular;
    } else {
        s.textWeight = TextWeight::Bold;
    }
    // BoldText=false still forces regular, so the old switch keeps working.
    if (!s.boldText) {
        s.textWeight = TextWeight::Regular;
    }

    std::wstring arrow = ReadStringSetting(L"Appearance.ArrowStyle");
    if (StringSettingIs(arrow, L"chevron")) {
        s.arrowStyle = ArrowStyle::Chevron;
    } else if (StringSettingIs(arrow, L"triangle")) {
        s.arrowStyle = ArrowStyle::Triangle;
    } else if (StringSettingIs(arrow, L"rounded")) {
        s.arrowStyle = ArrowStyle::Rounded;
    } else if (StringSettingIs(arrow, L"circle")) {
        s.arrowStyle = ArrowStyle::Circle;
    } else {
        s.arrowStyle = ArrowStyle::Solid;
    }

    s.arrowScale = ClampInt(Wh_GetIntSetting(L"Appearance.ArrowScale"), 60, 400);
    s.showColumnLabels = Wh_GetIntSetting(L"Appearance.ShowColumnLabels") != 0;
    s.dividerPos = ClampInt(Wh_GetIntSetting(L"Appearance.DividerPos"), 20, 80);
    s.dividerOpacity =
        ClampInt(Wh_GetIntSetting(L"Appearance.DividerOpacity"), 0, 255);
    s.detailsWidth = ClampInt(Wh_GetIntSetting(L"Appearance.DetailsWidth"), 140, 900);
    s.detailsHeight =
        ClampInt(Wh_GetIntSetting(L"Appearance.DetailsHeight"), 140, 900);

    std::wstring layout = ReadStringSetting(L"Appearance.LayoutMode");
    if (StringSettingIs(layout, L"speeds")) {
        s.layout = LayoutMode::Speeds;
    } else if (StringSettingIs(layout, L"oneline")) {
        s.layout = LayoutMode::OneLine;
    } else {
        s.layout = LayoutMode::Full;
    }

    s.offsetX = ClampInt(Wh_GetIntSetting(L"Appearance.OffsetX"), -4000, 4000);
    s.offsetY = ClampInt(Wh_GetIntSetting(L"Appearance.OffsetY"), -4000, 4000);
    s.dpiScaling = Wh_GetIntSetting(L"Appearance.DpiScaling") != 0;
    s.autoTheme = Wh_GetIntSetting(L"Appearance.AutoTheme") != 0;

    std::wstring colorText = ReadStringSetting(L"Appearance.TextColor");
    DWORD rgb = 0xFFFFFF;
    if (!colorText.empty()) {
        const wchar_t* p = colorText.c_str();
        while (*p == L' ' || *p == L'\t') {
            p++;
        }
        if (*p == L'#') {
            p++;
        } else if (p[0] == L'0' && (p[1] == L'x' || p[1] == L'X')) {
            p += 2;
        }
        wchar_t* end = nullptr;
        unsigned long parsed = wcstoul(p, &end, 16);
        if (end != p) {
            rgb = (DWORD)(parsed & 0xFFFFFF);
        }
    }
    s.manualTextColor = 0xFF000000 | rgb;

    s.colorArrows = Wh_GetIntSetting(L"Appearance.ColorArrows") != 0;
    s.bgOpacity = ClampInt(Wh_GetIntSetting(L"Appearance.BgOpacity"), 0, 255);

    std::wstring mode = ReadStringSetting(L"Network.InterfaceMode");
    if (StringSettingIs(mode, L"ethernet")) {
        s.ifMode = InterfaceMode::Ethernet;
    } else if (StringSettingIs(mode, L"wifi")) {
        s.ifMode = InterfaceMode::Wifi;
    } else if (StringSettingIs(mode, L"all")) {
        s.ifMode = InterfaceMode::All;
    } else if (StringSettingIs(mode, L"specific")) {
        s.ifMode = InterfaceMode::Specific;
    } else {
        s.ifMode = InterfaceMode::Auto;
    }

    s.selectedInterface = ReadStringSetting(L"Network.SelectedInterface");
    s.excludeVirtual = Wh_GetIntSetting(L"Network.ExcludeVirtual") != 0;
    s.updateInterval = ClampInt(Wh_GetIntSetting(L"Network.UpdateInterval"), 250, 5000);

    std::wstring unit = ReadStringSetting(L"Network.SpeedUnit");
    if (StringSettingIs(unit, L"bits")) {
        s.speedUnit = SpeedUnit::Bits;
    } else if (StringSettingIs(unit, L"bytes")) {
        s.speedUnit = SpeedUnit::Bytes;
    } else {
        s.speedUnit = SpeedUnit::Auto;  // Auto == bytes per second
    }
    s.binaryUnits = Wh_GetIntSetting(L"Network.BinaryUnits") != 0;

    std::wstring counter = ReadStringSetting(L"Traffic.CounterMode");
    s.counterMode = StringSettingIs(counter, L"persistent") ? CounterMode::Persistent
                                                            : CounterMode::Session;

    std::wstring reset = ReadStringSetting(L"Traffic.ResetCounters");
    if (StringSettingIs(reset, L"download")) {
        s.resetRequest = ResetRequest::Download;
    } else if (StringSettingIs(reset, L"upload")) {
        s.resetRequest = ResetRequest::Upload;
    } else if (StringSettingIs(reset, L"both")) {
        s.resetRequest = ResetRequest::Both;
    } else {
        s.resetRequest = ResetRequest::None;
    }

    s.showTooltip = Wh_GetIntSetting(L"Behavior.ShowTooltip") != 0;
    s.showDetails = Wh_GetIntSetting(L"Behavior.ShowDetailsOnClick") != 0;
    s.hideFullscreen = Wh_GetIntSetting(L"Behavior.HideFullscreen") != 0;
    s.startEnabled = Wh_GetIntSetting(L"Behavior.StartEnabled") != 0;

    {
        std::lock_guard<std::mutex> guard(g_settingsMutex);
        g_settings = std::move(s);
    }
}

static ModSettings GetSettings() {
    std::lock_guard<std::mutex> guard(g_settingsMutex);
    return g_settings;
}

// The Windhawk settings UI has no push buttons, so "Reset traffic counters" is a
// dropdown. Applying it on every settings read would keep the counters pinned at
// zero, so the last applied value is remembered in the mod's own storage and the
// reset only fires when the selection actually changes to a non-none value.
static void ApplyResetSettingIfChanged() {
    std::wstring current = ReadStringSetting(L"Traffic.ResetCounters");
    if (current.empty()) {
        current = L"none";
    }

    WCHAR previous[32] = {};
    Wh_GetStringValue(L"LastResetApplied", previous, ARRAYSIZE(previous));

    if (_wcsicmp(previous, current.c_str()) == 0) {
        return;  // already handled
    }

    Wh_SetStringValue(L"LastResetApplied", current.c_str());

    unsigned long long flags = 0;
    if (StringSettingIs(current, L"download")) {
        flags = 1;
    } else if (StringSettingIs(current, L"upload")) {
        flags = 2;
    } else if (StringSettingIs(current, L"both")) {
        flags = 3;
    }

    if (flags) {
        g_pendingReset.fetch_or(flags);
        Wh_Log(L"Reset requested from settings: %s", current.c_str());
        if (g_wakeEvent) {
            SetEvent(g_wakeEvent);
        }
    }
}

// --- Number formatting -----------------------------------------------------
// MB/s  = MegaBYTES per second (1 byte = 8 bits)
// Mbps  = MegaBITS  per second
static std::wstring FormatScaled(double value, const wchar_t* unit) {
    wchar_t buffer[64];
    int decimals = (value >= 100.0) ? 0 : (value >= 10.0 ? 1 : 2);
    swprintf_s(buffer, L"%.*f", decimals, value);

    std::wstring text = buffer;
    if (decimals > 0 && text.find(L'.') != std::wstring::npos) {
        // 2.40 -> 2.4, 3.00 -> 3, but keep 1.02 intact.
        while (!text.empty() && text.back() == L'0') {
            text.pop_back();
        }
        if (!text.empty() && text.back() == L'.') {
            text.pop_back();
        }
    }

    text += L' ';
    text += unit;
    return text;
}

// Instantaneous rate. bytesPerSec is always measured in bytes; the unit setting
// only changes how it is presented.
static std::wstring FormatSpeed(double bytesPerSec, const ModSettings& s) {
    if (bytesPerSec < 0.0 || !std::isfinite(bytesPerSec)) {
        bytesPerSec = 0.0;
    }

    if (s.speedUnit == SpeedUnit::Bits) {
        static const wchar_t* kBitUnits[] = {L"bps", L"Kbps", L"Mbps", L"Gbps",
                                             L"Tbps"};
        double value = bytesPerSec * 8.0;  // 1 byte = 8 bits
        int index = 0;
        while (value >= 1000.0 && index < 4) {
            value /= 1000.0;
            index++;
        }
        if (index == 0) {
            wchar_t buffer[64];
            swprintf_s(buffer, L"%.0f bps", value);
            return buffer;
        }
        return FormatScaled(value, kBitUnits[index]);
    }

    static const wchar_t* kByteUnits[] = {L"B/s", L"KB/s", L"MB/s", L"GB/s",
                                          L"TB/s"};
    const double base = s.binaryUnits ? 1024.0 : 1000.0;
    double value = bytesPerSec;
    int index = 0;
    while (value >= base && index < 4) {
        value /= base;
        index++;
    }
    if (index == 0) {
        wchar_t buffer[64];
        swprintf_s(buffer, L"%.0f B/s", value);
        return buffer;
    }
    return FormatScaled(value, kByteUnits[index]);
}

// Cumulative volume: B / KB / MB / GB / TB / PB.
static std::wstring FormatBytes(unsigned long long bytes, const ModSettings& s) {
    static const wchar_t* kUnits[] = {L"B", L"KB", L"MB", L"GB", L"TB", L"PB"};
    const double base = s.binaryUnits ? 1024.0 : 1000.0;
    double value = (double)bytes;
    int index = 0;
    while (value >= base && index < 5) {
        value /= base;
        index++;
    }
    if (index == 0) {
        wchar_t buffer[64];
        swprintf_s(buffer, L"%.0f B", value);
        return buffer;
    }
    return FormatScaled(value, kUnits[index]);
}

// --- Persistent traffic counters ------------------------------------------
// Stored as a tiny text file in %LOCALAPPDATA%\TaskbarNetworkLounge so that it
// survives Explorer / Windhawk / Windows restarts. Writes are batched (every 30
// seconds and on unload) and atomic (temp file + MoveFileEx).
struct PersistentTotals {
    unsigned long long down = 0;
    unsigned long long up = 0;
};

static std::wstring GetStorageDir() {
    std::wstring dir;
    WCHAR buffer[MAX_PATH];
    DWORD length = GetEnvironmentVariableW(L"LOCALAPPDATA", buffer,
                                          ARRAYSIZE(buffer));
    if (length > 0 && length < ARRAYSIZE(buffer)) {
        dir = buffer;
    } else if (SUCCEEDED(SHGetFolderPathW(nullptr, CSIDL_LOCAL_APPDATA, nullptr,
                                          0, buffer))) {
        dir = buffer;
    }

    if (dir.empty()) {
        return dir;
    }

    if (dir.back() != L'\\') {
        dir += L'\\';
    }
    dir += kStorageDirName;
    return dir;
}

static std::wstring GetStorageFile() {
    std::wstring dir = GetStorageDir();
    if (dir.empty()) {
        return dir;
    }
    return dir + L"\\" + kStorageFileName;
}

static bool LoadPersistentTotals(PersistentTotals* out) {
    std::wstring path = GetStorageFile();
    if (path.empty()) {
        return false;
    }

    HANDLE file = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ,
                              nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,
                              nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        return false;
    }

    char buffer[256] = {};
    DWORD read = 0;
    BOOL ok = ReadFile(file, buffer, sizeof(buffer) - 1, &read, nullptr);
    CloseHandle(file);
    if (!ok || read == 0) {
        return false;
    }
    buffer[read] = '\0';

    unsigned long long down = 0, up = 0, checksum = 0;
    int version = 0;
    // Format: "TNLv<version> <down> <up> <checksum>"
    if (sscanf(buffer, "TNLv%d %llu %llu %llu", &version, &down, &up,
               &checksum) != 4) {
        Wh_Log(L"Traffic counter file is corrupt, ignoring it");
        return false;
    }
    if (version != 1 || checksum != (down ^ up ^ 0x5A5AA5A5ULL)) {
        Wh_Log(L"Traffic counter file failed validation, ignoring it");
        return false;
    }

    out->down = down;
    out->up = up;
    return true;
}

static bool SavePersistentTotals(const PersistentTotals& totals) {
    std::wstring dir = GetStorageDir();
    std::wstring path = GetStorageFile();
    if (dir.empty() || path.empty()) {
        return false;
    }

    CreateDirectoryW(dir.c_str(), nullptr);

    std::wstring tempPath = path + L".tmp";
    HANDLE file = CreateFileW(tempPath.c_str(), GENERIC_WRITE, 0, nullptr,
                              CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        return false;
    }

    char buffer[256];
    int length = _snprintf_s(buffer, sizeof(buffer), _TRUNCATE,
                             "TNLv1 %llu %llu %llu\n", totals.down, totals.up,
                             totals.down ^ totals.up ^ 0x5A5AA5A5ULL);
    if (length <= 0) {
        CloseHandle(file);
        DeleteFileW(tempPath.c_str());
        return false;
    }

    DWORD written = 0;
    BOOL ok = WriteFile(file, buffer, (DWORD)length, &written, nullptr) &&
              written == (DWORD)length;
    if (ok) {
        FlushFileBuffers(file);
    }
    CloseHandle(file);

    if (!ok) {
        DeleteFileW(tempPath.c_str());
        return false;
    }

    if (!MoveFileExW(tempPath.c_str(), path.c_str(), MOVEFILE_REPLACE_EXISTING |
                                                        MOVEFILE_WRITE_THROUGH)) {
        DeleteFileW(tempPath.c_str());
        return false;
    }
    return true;
}
