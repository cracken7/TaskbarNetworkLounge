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

// Parse "0xRRGGBB", "#RRGGBB" or "RRGGBB" into an RGB DWORD. Returns fallback on
// empty/invalid input.
static DWORD ParseHexColor(const std::wstring& text, DWORD fallback) {
    if (text.empty()) {
        return fallback;
    }
    const wchar_t* p = text.c_str();
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
    if (end == p) {
        return fallback;
    }
    return (DWORD)(parsed & 0xFFFFFF);
}

// Like ParseHexColor but treats empty / "auto" / "default" as "not set" (0), so
// the caller can tell "user picked a colour" from "leave it to the theme".
static DWORD ParseHexColorOrAuto(const std::wstring& text) {
    if (text.empty() || StringSettingIs(text, L"auto") ||
        StringSettingIs(text, L"default")) {
        return 0;
    }
    return 0xFF000000 | ParseHexColor(text, 0);
}

// The right-click menu toggles a few things the settings YAML also exposes
// (display mode, captions, arrows). Those live toggles are the source of truth
// while the mod runs; the settings act as defaults that are applied only when
// their value actually changes (or the first time they are ever seen). Without
// this, any unrelated LoadSettings() - a divider drag ending, saving a different
// setting - would stomp a mode the user picked from the menu. The last applied
// value is remembered in the mod's own storage, same trick as the reset
// dropdown. Returns true when the caller should apply the new setting value.
static bool SettingChangedSinceLastApply(PCWSTR settingName,
                                         PCWSTR lastAppliedKey,
                                         std::wstring* currentOut) {
    std::wstring current = ReadStringSetting(settingName);
    WCHAR previous[64] = {};
    Wh_GetStringValue(lastAppliedKey, previous, ARRAYSIZE(previous));
    const bool firstTime = previous[0] == L'\0';
    const bool changed =
        !firstTime && _wcsicmp(previous, current.c_str()) != 0;
    if (firstTime || changed) {
        Wh_SetStringValue(lastAppliedKey, current.c_str());
    }
    *currentOut = std::move(current);
    return firstTime || changed;
}

static void LoadSettings() {
    ModSettings s;

    s.width = ClampInt(Wh_GetIntSetting(L"Appearance.PanelWidth"), 80, 1200);
    s.height = ClampInt(Wh_GetIntSetting(L"Appearance.PanelHeight"), 20, 200);
    s.fontSize = ClampInt(Wh_GetIntSetting(L"Appearance.FontSize"), 6, 40);

    s.fontFamily = ReadStringSetting(L"Appearance.FontFamily");
    // Trim: a stray space makes GDI+ fail to find the family.
    while (!s.fontFamily.empty() && (s.fontFamily.back() == L' ' || s.fontFamily.front() == L' ')) {
        if (s.fontFamily.back() == L' ') {
            s.fontFamily.pop_back();
        } else {
            s.fontFamily.erase(0, 1);
        }
    }

    std::wstring weight = ReadStringSetting(L"Appearance.TextWeight");
    if (StringSettingIs(weight, L"black")) {
        s.textWeight = TextWeight::Black;
    } else if (StringSettingIs(weight, L"semibold")) {
        s.textWeight = TextWeight::Semibold;
    } else if (StringSettingIs(weight, L"regular")) {
        s.textWeight = TextWeight::Regular;
    } else if (StringSettingIs(weight, L"italic")) {
        s.textWeight = TextWeight::Italic;
    } else {
        s.textWeight = TextWeight::Bold;
    }

    // Arrow style: table-driven so the YAML option strings and the enum stay in
    // one place. Unknown values fall back to Solid, as before. "none" is the
    // hide-arrows choice: it sets the hidden flag but keeps the last shape, so
    // showing them again restores what the user had.
    static const struct { const wchar_t* name; ArrowStyle style; } kArrowStyles[] = {
        {L"solid", ArrowStyle::Solid},       {L"chevron", ArrowStyle::Chevron},
        {L"triangle", ArrowStyle::Triangle}, {L"rounded", ArrowStyle::Rounded},
        {L"circle", ArrowStyle::Circle},     {L"double", ArrowStyle::Double},
        {L"triple", ArrowStyle::Triple},     {L"outline", ArrowStyle::Outline},
        {L"thin", ArrowStyle::Thin},         {L"fat", ArrowStyle::Fat},
        {L"head", ArrowStyle::Head},         {L"ring", ArrowStyle::Ring},
        {L"square", ArrowStyle::Square},     {L"fade", ArrowStyle::Fade},
        {L"dashed", ArrowStyle::Dashed},     {L"barb", ArrowStyle::Barb},
        {L"needle", ArrowStyle::Needle},     {L"dot", ArrowStyle::Dot},
        {L"feather", ArrowStyle::Feather},   {L"layered", ArrowStyle::Layered},   {L"tray", ArrowStyle::Tray},
        {L"plus", ArrowStyle::Plus},
        {L"none", ArrowStyle::None},
    };
    std::wstring arrow;
    if (SettingChangedSinceLastApply(L"Appearance.ArrowStyle",
                                     L"LastArrowStyleApplied", &arrow)) {
        for (const auto& entry : kArrowStyles) {
            if (StringSettingIs(arrow, entry.name)) {
                if (entry.style == ArrowStyle::None) {
                    g_arrowsHidden = 1;
                } else {
                    g_arrowStyle = (int)entry.style;
                    g_arrowsHidden = 0;
                }
                Wh_SetIntValue(L"ArrowStyle", g_arrowStyle.load());
                Wh_SetIntValue(L"ArrowsHidden", g_arrowsHidden.load());
                break;
            }
        }
    }
    s.arrowStyle = g_arrowsHidden.load()
                       ? ArrowStyle::None
                       : (ArrowStyle)ClampInt(g_arrowStyle.load(), 0,
                                              (int)ArrowStyle::Plus);

    s.arrowScale = ClampInt(Wh_GetIntSetting(L"Appearance.ArrowScale"), 60, 400);
    std::wstring captions;
    if (SettingChangedSinceLastApply(L"Appearance.ShowColumnLabels",
                                     L"LastCaptionsApplied", &captions)) {
        int bits = 3;
        if (StringSettingIs(captions, L"speed")) {
            bits = 1;
        } else if (StringSettingIs(captions, L"total")) {
            bits = 2;
        } else if (StringSettingIs(captions, L"none")) {
            bits = 0;
        }
        g_captionBits = bits;
        Wh_SetIntValue(L"CaptionBits", bits);
    }
    const int captionBits = g_captionBits.load();
    s.captions = captionBits == 3   ? CaptionMode::Both
                 : captionBits == 1 ? CaptionMode::SpeedOnly
                 : captionBits == 2 ? CaptionMode::TotalOnly
                                    : CaptionMode::None;
    s.dividerPos = ClampInt(Wh_GetIntSetting(L"Appearance.DividerPos"), 20, 80);
    // The dragged offset lives in the mod's own storage, not in the settings YAML:
    // Windhawk's settings UI has no draggable control, and writing a setting from
    // the mod would fight the UI. The live value is in g_dividerOffset; this read
    // only seeds it once so a drag survives a restart.
    s.dividerDraggable =
        Wh_GetIntSetting(L"Appearance.DividerDraggable") != 0;
    if (!s.dividerDraggable) {
        g_dividerOffset = 0;  // going back to a fixed divider discards the drag
        Wh_SetIntValue(L"DividerOffset", 0);
    }
    s.dividerOffset = g_dividerOffset.load();
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

    // Display mode: same live-toggle model as arrows and captions - the setting
    // is applied only when it actually changes (or on first run), so a mode
    // picked from the right-click menu survives unrelated LoadSettings() calls
    // (e.g. a divider drag ending).
    std::wstring display;
    if (SettingChangedSinceLastApply(L"Appearance.DisplayMode",
                                     L"LastDisplayModeApplied", &display)) {
        int modeDefault = StringSettingIs(display, L"speed")
                              ? 1
                              : (StringSettingIs(display, L"traffic") ? 2 : 0);
        g_displayMode = modeDefault;
        Wh_SetIntValue(L"DisplayMode", modeDefault);
    }
    const int displayMode = ClampInt(g_displayMode.load(), 0, 2);
    s.display = displayMode == 1 ? LayoutMode::Speeds
                                 : (displayMode == 2 ? LayoutMode::OneLine
                                                     : LayoutMode::Full);
    s.cycleOnMiddleClick =
        Wh_GetIntSetting(L"Appearance.CycleOnMiddleClick") != 0;

    s.offsetX = ClampInt(Wh_GetIntSetting(L"Appearance.OffsetX"), -4000, 4000);
    s.offsetY = ClampInt(Wh_GetIntSetting(L"Appearance.OffsetY"), -4000, 4000);
    s.dpiScaling = Wh_GetIntSetting(L"Appearance.DpiScaling") != 0;
    s.autoTheme = Wh_GetIntSetting(L"Appearance.AutoTheme") != 0;

    s.manualTextColor = 0xFF000000 | ParseHexColor(
                            ReadStringSetting(L"Appearance.TextColor"), 0xFFFFFF);

    // Per-direction colours: empty / "auto" means "not set" (0) so the painter
    // keeps the theme accent; any hex recolours that half (arrow + numbers).
    s.downloadColor = ParseHexColorOrAuto(
        ReadStringSetting(L"Appearance.DownloadColor"));
    s.uploadColor =
        ParseHexColorOrAuto(ReadStringSetting(L"Appearance.UploadColor"));

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
    s.resetOnSourceChange =
        Wh_GetIntSetting(L"Network.ResetOnSourceChange") != 0;
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
// Stored in Windhawk's per-mod storage so the totals survive Explorer / Windhawk
// / Windows restarts and disappear with the mod - a mod's own state does not
// belong in a directory that outlives its uninstall. Writes are batched (every 30
// seconds and on unload).
struct PersistentTotals {
    unsigned long long down = 0;
    unsigned long long up = 0;
};

static bool LoadPersistentTotals(PersistentTotals* out) {
    PersistentTotals stored;
    size_t read = Wh_GetBinaryValue(L"TrafficTotals", &stored, sizeof(stored));
    if (read != sizeof(stored)) {
        return false;
    }
    *out = stored;
    return true;
}

static bool SavePersistentTotals(const PersistentTotals& totals) {
    return Wh_SetBinaryValue(L"TrafficTotals", &totals, sizeof(totals)) != FALSE;
}
