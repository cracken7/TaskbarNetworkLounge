// ==WindhawkMod==
// @id              taskbar-network-lounge
// @name            Taskbar Network Lounge
// @description     A compact, native network monitor docked on the taskbar: live download/upload speed plus total traffic.
// @version         1.0.0
// @author          cracken7
// @github          https://github.com/cracken7
// @include         explorer.exe
// @compilerOptions -lole32 -ldwmapi -lgdi32 -luser32 -lshcore -lgdiplus -lshell32 -lcomctl32 -liphlpapi -lws2_32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Network Lounge

A tiny, native-looking network meter that lives on the Windows 11 taskbar, in the
same spot and with the same acrylic / rounded / theme-aware styling as
**Taskbar Music Lounge**.

```
  v  12.4 MB/s     v  4.82 GB
  ^  1.8 MB/s      ^  1.17 GB
```

## Features
* **Live speed** - download and upload, measured from real interface counters
  (`GetIfTable2`, IP Helper API) divided by real elapsed time
  (`QueryPerformanceCounter`). No `ipconfig`/`netstat`/PowerShell parsing.
* **Total traffic** - session totals, or persistent totals that survive Explorer /
  Windhawk / Windows restarts (saved to a small file in `%LOCALAPPDATA%`).
* **Smart adapter selection** - Auto (the adapter that actually carries your
  internet traffic), Ethernet, Wi-Fi, all active adapters, or one specific
  adapter. Loopback / tunnels / VMware / Hyper-V / Docker / VPN adapters are
  filtered out by default (configurable).
* **Native styling** - GDI+ anti-aliased drawing, DWM rounded corners, acrylic
  blur, Segoe UI, automatic light/dark theme.
* **Details panel** - click the widget for an acrylic panel with interface,
  status, IPv4, speeds, totals and a **Reset** button.
* **Rich tooltip** on hover, **context menu** on right click (refresh, reset,
  open Windows network settings, hide).
* **Bytes vs bits** - `MB/s` = Megabytes per second, `Mbps` = Megabits per
  second (1 byte = 8 bits). Pick either; the unit is always drawn next to the
  number so the two can never be confused.
* Cheap: one `GetIfTable2` call per interval on a worker thread, repaint only
  when the numbers actually change, no busy waiting.

## Requirements
* Windows 11 (rounded corners + acrylic). Works on Windows 10 with squared corners.
* Taskbar Widgets should be off, otherwise they overlap: Taskbar Settings -> Widgets -> Off.

## Notes / limitations
* The widget attaches to the **primary** taskbar (`Shell_TrayWnd`). Secondary
  taskbars are used only as a fallback when no primary taskbar exists.
* Totals count only the interfaces that are being monitored, from the moment the
  mod started (Session) or from the stored value (Persistent).
* Speeds are sampled, so they can differ from Task Manager by a few percent
  within one sampling window; the running average over a second matches.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- Appearance:
  - PanelWidth: 200
    $name: Panel width
    $description: Width of the widget in pixels (at 100% scaling).
  - PanelHeight: 48
    $name: Panel height
    $description: Height of the widget in pixels (at 100% scaling).
  - FontSize: 11
    $name: Font size
  - LayoutMode: full
    $name: Layout
    $options:
    - full: Speeds + totals (two columns)
    - speeds: Speeds only (two rows)
    - oneline: Speeds only (single line)
  - OffsetX: 12
    $name: X offset
    $description: Distance from the left edge of the taskbar (from the top edge for a vertical taskbar).
  - OffsetY: 0
    $name: Y offset
  - DpiScaling: true
    $name: Scale with DPI
    $description: Multiply sizes by the monitor scaling (125%, 150%, 175%, 200%...).
  - AutoTheme: true
    $name: Auto theme
    $description: Follow the Windows light/dark theme.
  - TextColor: 0xFFFFFF
    $name: Manual text color (hex)
    $description: Used only when Auto theme is off.
  - ColorArrows: true
    $name: Colored arrows
    $description: Blue for download, green for upload. Off = monochrome.
  - BgOpacity: 0
    $name: Acrylic tint opacity (0-255)
    $description: Keep 0 for pure glass.
  $name: Appearance
- Network:
  - InterfaceMode: auto
    $name: Interface mode
    $options:
    - auto: Auto (adapter used for the internet)
    - ethernet: Ethernet only
    - wifi: Wi-Fi only
    - all: All active interfaces
    - specific: Specific interface (below)
  - SelectedInterface: ""
    $name: Specific interface
    $description: Name or part of the name/description of the adapter, e.g. "Ethernet" or "Realtek".
  - ExcludeVirtual: true
    $name: Ignore virtual adapters
    $description: Skip loopback, tunnels, VPN, VMware, Hyper-V, Docker, TAP and other non-physical adapters.
  - UpdateInterval: 1000
    $name: Update interval (ms)
    $description: 250-5000 ms. 1000 ms is a good balance.
  - SpeedUnit: auto
    $name: Speed unit
    $options:
    - auto: Auto (bytes per second, e.g. 12.4 MB/s)
    - bytes: Bytes per second (MB/s)
    - bits: Bits per second (Mbps)
  - BinaryUnits: true
    $name: Use 1024-based byte units
    $description: On = 1 MB is 1048576 bytes (like Explorer). Off = 1 MB is 1000000 bytes. Bit units are always 1000-based.
  $name: Network
- Traffic:
  - CounterMode: session
    $name: Traffic counter mode
    $options:
    - session: Session (reset when the mod starts)
    - persistent: Persistent (saved to disk)
  - ResetCounters: none
    $name: Reset traffic counters
    $description: Pick a value and save to zero the counters. It is applied once, then remembered; the details panel and the right click menu can reset too.
    $options:
    - none: Do not reset
    - download: Reset download
    - upload: Reset upload
    - both: Reset both
  $name: Traffic
- Behavior:
  - ShowTooltip: true
    $name: Show tooltip on hover
  - ShowDetailsOnClick: true
    $name: Show details panel on click
  - HideFullscreen: false
    $name: Hide when fullscreen
  - StartEnabled: true
    $name: Start enabled
    $description: Off = the widget stays hidden until this is turned back on.
  $name: Behavior
*/
// ==/WindhawkModSettings==
// ---------------------------------------------------------------------------
// Taskbar Network Lounge - implementation
//
// Design notes:
//  * Architecture mirrors Taskbar Music Lounge: a Windhawk *tool mod* that owns
//    a native layered popup window docked over Shell_TrayWnd, drawn with GDI+,
//    acrylic via SetWindowCompositionAttribute, rounded via DWM.
//  * All media/GSMTC logic is replaced by IP Helper network counters.
//  * winsock2.h MUST be included before iphlpapi.h/netioapi.h, otherwise
//    mingw-w64 hides MIB_IF_ROW2 / GetIfTable2 (they are only declared when
//    _WS2IPDEF_ is defined).
// ---------------------------------------------------------------------------

// Windhawk force-includes windhawk_api.h (which pulls in windows.h) before this
// file, so winsock2.h emits its "include me before windows.h" warning. The
// declarations we need are unaffected, so the warning is silenced locally.
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-W#warnings"
#include <winsock2.h>
#pragma clang diagnostic pop
#include <ws2ipdef.h>
#include <iphlpapi.h>
#include <netioapi.h>

#include <windows.h>
#include <commctrl.h>
#include <dwmapi.h>
#include <gdiplus.h>
#include <shellapi.h>
#include <shlobj.h>
#include <shobjidl.h>
#include <windowsx.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdio>
#include <cwctype>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

using namespace Gdiplus;

// --- Constants -------------------------------------------------------------
static const WCHAR* kFontName = L"Segoe UI";
static const WCHAR* kWidgetClass = L"WindhawkNetworkLoungeWidget";
static const WCHAR* kPanelClass = L"WindhawkNetworkLoungePanel";
static const WCHAR* kStorageDirName = L"TaskbarNetworkLounge";
static const WCHAR* kStorageFileName = L"traffic.dat";

// Timers / messages
#define IDT_WATCHDOG 1001
#define IDT_ANIM 1002
#define APP_WM_CLOSE (WM_APP + 0)
#define APP_WM_REPOSITION (WM_APP + 10)
#define APP_WM_DATA_UPDATED (WM_APP + 11)
#define APP_WM_TOGGLE_PANEL (WM_APP + 12)

// Context menu ids
#define IDM_REFRESH 100
#define IDM_RESET_DL 101
#define IDM_RESET_UL 102
#define IDM_RESET_BOTH 103
#define IDM_NET_SETTINGS 104
#define IDM_WH_SETTINGS 105
#define IDM_HIDE 106

// --- Undocumented composition / z-band APIs (same as the reference mod) ----
typedef enum _WINDOWCOMPOSITIONATTRIB { WCA_ACCENT_POLICY = 19 } WINDOWCOMPOSITIONATTRIB;
typedef enum _ACCENT_STATE {
    ACCENT_DISABLED = 0,
    ACCENT_ENABLE_BLURBEHIND = 3,
    ACCENT_ENABLE_ACRYLICBLURBEHIND = 4,
    ACCENT_INVALID_STATE = 5
} ACCENT_STATE;
typedef struct _ACCENT_POLICY {
    ACCENT_STATE AccentState;
    DWORD AccentFlags;
    DWORD GradientColor;
    DWORD AnimationId;
} ACCENT_POLICY;
typedef struct _WINDOWCOMPOSITIONATTRIBDATA {
    WINDOWCOMPOSITIONATTRIB Attribute;
    PVOID Data;
    SIZE_T SizeOfData;
} WINDOWCOMPOSITIONATTRIBDATA;
typedef BOOL(WINAPI* pSetWindowCompositionAttribute)(HWND, WINDOWCOMPOSITIONATTRIBDATA*);

enum ZBID {
    ZBID_DEFAULT = 0,
    ZBID_DESKTOP = 1,
    ZBID_UIACCESS = 2,
    ZBID_IMMERSIVE_IHM = 3,
    ZBID_IMMERSIVE_NOTIFICATION = 4,
    ZBID_IMMERSIVE_APPCHROME = 5,
    ZBID_IMMERSIVE_MOGO = 6,
    ZBID_IMMERSIVE_EDGY = 7,
    ZBID_IMMERSIVE_INACTIVEMOBODY = 8,
    ZBID_IMMERSIVE_INACTIVEDOCK = 9,
    ZBID_IMMERSIVE_ACTIVEMOBODY = 10,
    ZBID_IMMERSIVE_ACTIVEDOCK = 11,
    ZBID_IMMERSIVE_BACKGROUND = 12,
    ZBID_IMMERSIVE_SEARCH = 13,
    ZBID_GENUINE_WINDOWS = 14,
    ZBID_IMMERSIVE_RESTRICTED = 15,
    ZBID_SYSTEM_TOOLS = 16,
    ZBID_LOCK = 17,
    ZBID_ABOVELOCK_UX = 18,
};

typedef HWND(WINAPI* pCreateWindowInBand)(DWORD dwExStyle,
                                          LPCWSTR lpClassName,
                                          LPCWSTR lpWindowName,
                                          DWORD dwStyle,
                                          int x,
                                          int y,
                                          int nWidth,
                                          int nHeight,
                                          HWND hWndParent,
                                          HMENU hMenu,
                                          HINSTANCE hInstance,
                                          LPVOID lpParam,
                                          DWORD dwBand);

// --- Settings --------------------------------------------------------------
enum class InterfaceMode { Auto, Ethernet, Wifi, All, Specific };
enum class SpeedUnit { Auto, Bytes, Bits };
enum class CounterMode { Session, Persistent };
enum class LayoutMode { Full, Speeds, OneLine };
enum class ResetRequest { None, Download, Upload, Both };

struct ModSettings {
    // Appearance
    int width = 200;
    int height = 48;
    int fontSize = 11;
    LayoutMode layout = LayoutMode::Full;
    int offsetX = 12;
    int offsetY = 0;
    bool dpiScaling = true;
    bool autoTheme = true;
    DWORD manualTextColor = 0xFFFFFFFF;
    bool colorArrows = true;
    int bgOpacity = 0;
    // Network
    InterfaceMode ifMode = InterfaceMode::Auto;
    std::wstring selectedInterface;
    bool excludeVirtual = true;
    int updateInterval = 1000;
    SpeedUnit speedUnit = SpeedUnit::Auto;
    bool binaryUnits = true;
    // Traffic
    CounterMode counterMode = CounterMode::Session;
    ResetRequest resetRequest = ResetRequest::None;
    // Behavior
    bool showTooltip = true;
    bool showDetails = true;
    bool hideFullscreen = false;
    bool startEnabled = true;
};

static ModSettings g_settings;
static std::mutex g_settingsMutex;  // guards g_settings against UI/worker races

// --- Shared network state --------------------------------------------------
struct NetSnapshot {
    double downBytesPerSec = 0.0;
    double upBytesPerSec = 0.0;
    unsigned long long totalDown = 0;
    unsigned long long totalUp = 0;
    std::wstring ifName = L"Detecting...";
    std::wstring ifKind = L"";  // Ethernet / Wi-Fi / ...
    std::wstring ipv4 = L"";
    bool connected = false;
    bool haveInterface = false;
    int ifCount = 0;
};

static NetSnapshot g_snapshot;
static std::mutex g_snapshotMutex;

// Windows / threads
static HWND g_hWidget = nullptr;
static HWND g_hPanel = nullptr;
static HWND g_hTooltip = nullptr;
static std::thread* g_uiThread = nullptr;
static std::thread* g_workerThread = nullptr;
static HANDLE g_stopEvent = nullptr;   // manual reset: shut worker down
static HANDLE g_wakeEvent = nullptr;   // auto reset: sample immediately
static std::atomic<bool> g_running{false};
static std::atomic<bool> g_panelVisible{false};
static std::atomic<unsigned long long> g_panelHiddenAt{0};  // GetTickCount64
static std::atomic<int> g_hoverState{0};  // 0 none, 1 widget, 2 reset button
static std::atomic<bool> g_hiddenByUser{false};
static std::atomic<unsigned long long> g_pendingReset{0};  // bit0 dl, bit1 ul
static UINT g_taskbarCreatedMsg = 0;
static HWINEVENTHOOK g_taskbarHook = nullptr;
static HWND g_hookedTaskbar = nullptr;
static ULONG_PTR g_gdiplusToken = 0;
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
// --- Theme / appearance ----------------------------------------------------
static bool IsSystemLightMode() {
    DWORD value = 0;
    DWORD size = sizeof(value);
    if (RegGetValueW(HKEY_CURRENT_USER,
                     L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                     L"SystemUsesLightTheme", RRF_RT_DWORD, nullptr, &value,
                     &size) == ERROR_SUCCESS) {
        return value != 0;
    }
    return false;
}

static DWORD GetCurrentTextColor(const ModSettings& s) {
    if (s.autoTheme) {
        return IsSystemLightMode() ? 0xFF000000 : 0xFFFFFFFF;
    }
    return s.manualTextColor;
}

static void UpdateAppearance(HWND hwnd) {
    ModSettings s = GetSettings();

    DWM_WINDOW_CORNER_PREFERENCE preference = DWMWCP_ROUND;
    DwmSetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &preference,
                          sizeof(preference));

    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    if (!user32) {
        return;
    }
    auto setCompositionAttribute = (pSetWindowCompositionAttribute)GetProcAddress(
        user32, "SetWindowCompositionAttribute");
    if (!setCompositionAttribute) {
        return;
    }

    DWORD tint;
    if (s.bgOpacity > 0) {
        DWORD base = s.autoTheme ? (IsSystemLightMode() ? 0xFFFFFF : 0x000000)
                                 : 0xFFFFFF;
        tint = ((DWORD)s.bgOpacity << 24) | base;
    } else if (s.autoTheme) {
        tint = IsSystemLightMode() ? 0x40FFFFFF : 0x40000000;
    } else {
        tint = 0x00FFFFFF;
    }

    ACCENT_POLICY policy = {ACCENT_ENABLE_ACRYLICBLURBEHIND, 0, tint, 0};
    WINDOWCOMPOSITIONATTRIBDATA data = {WCA_ACCENT_POLICY, &policy,
                                        sizeof(ACCENT_POLICY)};
    setCompositionAttribute(hwnd, &data);
}

static void AddRoundedRect(GraphicsPath& path,
                           REAL x,
                           REAL y,
                           REAL w,
                           REAL h,
                           REAL r) {
    REAL d = r * 2;
    if (d > w) {
        d = w;
    }
    if (d > h) {
        d = h;
    }
    path.AddArc(x, y, d, d, 180, 90);
    path.AddArc(x + w - d, y, d, d, 270, 90);
    path.AddArc(x + w - d, y + h - d, d, d, 0, 90);
    path.AddArc(x, y + h - d, d, d, 90, 90);
    path.CloseFigure();
}

// DPI of the monitor the widget lives on; used to scale everything so the widget
// looks identical at 100% / 125% / 150% / 175% / 200%.
static double GetScaleForWindow(HWND hwnd) {
    ModSettings s = GetSettings();
    if (!s.dpiScaling) {
        return 1.0;
    }
    UINT dpi = 0;
    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    if (user32) {
        using GetDpiForWindow_t = UINT(WINAPI*)(HWND);
        auto getDpiForWindow =
            (GetDpiForWindow_t)GetProcAddress(user32, "GetDpiForWindow");
        if (getDpiForWindow && hwnd) {
            dpi = getDpiForWindow(hwnd);
        }
        if (!dpi) {
            using GetDpiForSystem_t = UINT(WINAPI*)();
            auto getDpiForSystem =
                (GetDpiForSystem_t)GetProcAddress(user32, "GetDpiForSystem");
            if (getDpiForSystem) {
                dpi = getDpiForSystem();
            }
        }
    }
    if (!dpi) {
        HDC screen = GetDC(nullptr);
        if (screen) {
            dpi = (UINT)GetDeviceCaps(screen, LOGPIXELSX);
            ReleaseDC(nullptr, screen);
        }
    }
    if (!dpi) {
        dpi = 96;
    }
    return (double)dpi / 96.0;
}

// Down/up chevron glyphs drawn as vectors so they stay crisp at any DPI and do
// not depend on a font containing the arrow characters.
static void DrawArrow(Graphics& graphics,
                      const Color& color,
                      REAL cx,
                      REAL cy,
                      REAL size,
                      bool down) {
    REAL half = size / 2.0f;
    REAL stem = size * 0.22f;
    Pen pen(color, size * 0.16f);
    pen.SetStartCap(LineCapRound);
    pen.SetEndCap(LineCapRound);

    if (down) {
        graphics.DrawLine(&pen, cx, cy - half, cx, cy + half * 0.35f);
        PointF points[3] = {PointF(cx - stem, cy + half * 0.05f),
                            PointF(cx, cy + half),
                            PointF(cx + stem, cy + half * 0.05f)};
        SolidBrush brush(color);
        graphics.FillPolygon(&brush, points, 3);
    } else {
        graphics.DrawLine(&pen, cx, cy + half, cx, cy - half * 0.35f);
        PointF points[3] = {PointF(cx - stem, cy - half * 0.05f),
                            PointF(cx, cy - half),
                            PointF(cx + stem, cy - half * 0.05f)};
        SolidBrush brush(color);
        graphics.FillPolygon(&brush, points, 3);
    }
}

static Color BlendColor(const Color& base, BYTE alpha) {
    return Color(alpha, base.GetRed(), base.GetGreen(), base.GetBlue());
}

// --- Widget painting -------------------------------------------------------
// WM_PAINT only draws data that is already in g_snapshot; it never calls a
// network API.
static void DrawNetworkPanel(HDC hdc, int width, int height, HWND hwnd) {
    ModSettings s = GetSettings();

    NetSnapshot snapshot;
    {
        std::lock_guard<std::mutex> guard(g_snapshotMutex);
        snapshot = g_snapshot;
    }

    Graphics graphics(hdc);
    graphics.SetSmoothingMode(SmoothingModeAntiAlias);
    graphics.SetTextRenderingHint(TextRenderingHintClearTypeGridFit);
    graphics.Clear(Color(0, 0, 0, 0));

    const double scale = GetScaleForWindow(hwnd);
    Color textColor{GetCurrentTextColor(s)};

    Color downColor = textColor;
    Color upColor = textColor;
    if (s.colorArrows) {
        bool light = s.autoTheme ? IsSystemLightMode() : false;
        downColor = light ? Color(255, 0, 95, 184) : Color(255, 96, 205, 255);
        upColor = light ? Color(255, 16, 124, 16) : Color(255, 108, 219, 132);
    }

    // Hover background (only for the widget's own hover state, not the panel's
    // Reset button hover, which shares the same variable).
    if (g_hoverState.load() == 1) {
        GraphicsPath hoverPath;
        AddRoundedRect(hoverPath, 1.0f, 1.0f, (REAL)width - 2.0f,
                       (REAL)height - 2.0f, (REAL)(8.0 * scale));
        SolidBrush hoverBrush(BlendColor(textColor, 26));
        graphics.FillPath(&hoverBrush, &hoverPath);
    }

    FontFamily fontFamily(kFontName, nullptr);
    REAL fontSize = (REAL)(s.fontSize * scale);
    Font fontValue(&fontFamily, fontSize, FontStyleBold, UnitPixel);
    Font fontTotal(&fontFamily, fontSize * 0.92f, FontStyleRegular, UnitPixel);
    SolidBrush textBrush(textColor);
    SolidBrush dimBrush(BlendColor(textColor, 200));

    StringFormat format;
    format.SetAlignment(StringAlignmentNear);
    format.SetLineAlignment(StringAlignmentCenter);
    format.SetFormatFlags(StringFormatFlagsNoWrap);

    const REAL padding = (REAL)(10.0 * scale);
    const REAL arrowSize = fontSize * 1.05f;
    const REAL arrowGap = (REAL)(5.0 * scale);

    if (!snapshot.haveInterface) {
        // Disconnected / no adapter: single centered line, no fake numbers.
        StringFormat centered;
        centered.SetAlignment(StringAlignmentCenter);
        centered.SetLineAlignment(StringAlignmentCenter);
        RectF area(0, 0, (REAL)width, (REAL)height);
        const wchar_t* message =
            snapshot.ifName.empty() ? L"No Network" : snapshot.ifName.c_str();
        graphics.DrawString(message, -1, &fontValue, area, &centered, &textBrush);
        return;
    }

    std::wstring downSpeed = FormatSpeed(snapshot.downBytesPerSec, s);
    std::wstring upSpeed = FormatSpeed(snapshot.upBytesPerSec, s);
    std::wstring downTotal = FormatBytes(snapshot.totalDown, s);
    std::wstring upTotal = FormatBytes(snapshot.totalUp, s);

    auto drawRow = [&](REAL x, REAL centerY, bool down, const std::wstring& text,
                       Font& font, SolidBrush& brush, REAL maxWidth) {
        DrawArrow(graphics, down ? downColor : upColor, x + arrowSize / 2.0f,
                  centerY, arrowSize, down);
        RectF textRect(x + arrowSize + arrowGap, centerY - fontSize,
                       maxWidth - arrowSize - arrowGap, fontSize * 2.0f);
        graphics.DrawString(text.c_str(), -1, &font, textRect, &format, &brush);
    };

    if (s.layout == LayoutMode::OneLine) {
        REAL centerY = height / 2.0f;
        REAL columnWidth = ((REAL)width - padding * 2.0f) / 2.0f;
        drawRow(padding, centerY, true, downSpeed, fontValue, textBrush,
                columnWidth);
        drawRow(padding + columnWidth, centerY, false, upSpeed, fontValue,
                textBrush, columnWidth);
        return;
    }

    REAL topY = height * 0.30f;
    REAL bottomY = height * 0.72f;

    if (s.layout == LayoutMode::Speeds) {
        REAL columnWidth = (REAL)width - padding * 2.0f;
        drawRow(padding, topY, true, downSpeed, fontValue, textBrush, columnWidth);
        drawRow(padding, bottomY, false, upSpeed, fontValue, textBrush,
                columnWidth);
        return;
    }

    // Full layout: speeds on the left, totals on the right.
    REAL columnGap = (REAL)(8.0 * scale);
    REAL columnWidth = ((REAL)width - padding * 2.0f - columnGap) / 2.0f;
    REAL rightX = padding + columnWidth + columnGap;

    drawRow(padding, topY, true, downSpeed, fontValue, textBrush, columnWidth);
    drawRow(padding, bottomY, false, upSpeed, fontValue, textBrush, columnWidth);
    drawRow(rightX, topY, true, downTotal, fontTotal, dimBrush, columnWidth);
    drawRow(rightX, bottomY, false, upTotal, fontTotal, dimBrush, columnWidth);
}

// --- Details panel painting ------------------------------------------------
struct PanelMetrics {
    int width = 240;
    int height = 250;
    RECT resetButton{};
};

static PanelMetrics ComputePanelMetrics(double scale) {
    PanelMetrics metrics;
    metrics.width = (int)(240 * scale);
    metrics.height = (int)(252 * scale);
    int margin = (int)(16 * scale);
    int buttonHeight = (int)(32 * scale);
    metrics.resetButton.left = margin;
    metrics.resetButton.right = metrics.width - margin;
    metrics.resetButton.bottom = metrics.height - margin;
    metrics.resetButton.top = metrics.resetButton.bottom - buttonHeight;
    return metrics;
}

static void DrawDetailsPanel(HDC hdc, int width, int height, HWND hwnd) {
    ModSettings s = GetSettings();

    NetSnapshot snapshot;
    {
        std::lock_guard<std::mutex> guard(g_snapshotMutex);
        snapshot = g_snapshot;
    }

    Graphics graphics(hdc);
    graphics.SetSmoothingMode(SmoothingModeAntiAlias);
    graphics.SetTextRenderingHint(TextRenderingHintClearTypeGridFit);
    graphics.Clear(Color(0, 0, 0, 0));

    double scale = GetScaleForWindow(hwnd);
    Color textColor{GetCurrentTextColor(s)};
    bool light = s.autoTheme ? IsSystemLightMode() : false;

    Color downColor = s.colorArrows
                          ? (light ? Color(255, 0, 95, 184) : Color(255, 96, 205, 255))
                          : textColor;
    Color upColor = s.colorArrows
                        ? (light ? Color(255, 16, 124, 16) : Color(255, 108, 219, 132))
                        : textColor;

    FontFamily fontFamily(kFontName, nullptr);
    REAL base = (REAL)(12.0 * scale);
    Font fontTitle(&fontFamily, base * 0.85f, FontStyleBold, UnitPixel);
    Font fontLabel(&fontFamily, base * 0.85f, FontStyleRegular, UnitPixel);
    Font fontValue(&fontFamily, base * 1.35f, FontStyleBold, UnitPixel);
    Font fontSmall(&fontFamily, base * 0.95f, FontStyleRegular, UnitPixel);

    SolidBrush textBrush(textColor);
    SolidBrush labelBrush(BlendColor(textColor, 165));

    StringFormat left;
    left.SetAlignment(StringAlignmentNear);
    left.SetLineAlignment(StringAlignmentNear);
    left.SetFormatFlags(StringFormatFlagsNoWrap);

    REAL margin = (REAL)(16.0 * scale);
    REAL y = margin;

    graphics.DrawString(L"NETWORK", -1, &fontTitle,
                        RectF(margin, y, (REAL)width - margin * 2, base * 1.5f),
                        &left, &labelBrush);
    y += base * 1.7f;

    std::wstring headline = snapshot.ifName;
    if (!snapshot.ifKind.empty() && snapshot.ifKind != snapshot.ifName) {
        headline += L"  (" + snapshot.ifKind + L")";
    }
    graphics.DrawString(headline.c_str(), -1, &fontSmall,
                        RectF(margin, y, (REAL)width - margin * 2, base * 1.6f),
                        &left, &textBrush);
    y += base * 1.4f;

    std::wstring status = snapshot.connected ? L"Connected" : L"Disconnected";
    if (!snapshot.ipv4.empty()) {
        status += L"  -  " + snapshot.ipv4;
    }
    graphics.DrawString(status.c_str(), -1, &fontLabel,
                        RectF(margin, y, (REAL)width - margin * 2, base * 1.6f),
                        &left, &labelBrush);
    y += base * 2.0f;

    auto drawMetric = [&](const wchar_t* label, const std::wstring& value,
                          bool down) {
        REAL arrow = base * 1.1f;
        DrawArrow(graphics, down ? downColor : upColor, margin + arrow / 2.0f,
                  y + base * 0.55f, arrow, down);
        graphics.DrawString(label, -1, &fontLabel,
                            RectF(margin + arrow + (REAL)(6.0 * scale), y,
                                  (REAL)width - margin * 2, base * 1.4f),
                            &left, &labelBrush);
        y += base * 1.35f;
        graphics.DrawString(value.c_str(), -1, &fontValue,
                            RectF(margin, y, (REAL)width - margin * 2,
                                  base * 1.8f),
                            &left, &textBrush);
        y += base * 1.9f;
    };

    drawMetric(L"Download", FormatSpeed(snapshot.downBytesPerSec, s), true);
    drawMetric(L"Upload", FormatSpeed(snapshot.upBytesPerSec, s), false);

    graphics.DrawString(
        s.counterMode == CounterMode::Persistent ? L"Traffic (persistent)"
                                                 : L"Traffic (session)",
        -1, &fontLabel, RectF(margin, y, (REAL)width - margin * 2, base * 1.4f),
        &left, &labelBrush);
    y += base * 1.5f;

    REAL arrow = base;
    DrawArrow(graphics, downColor, margin + arrow / 2.0f, y + base * 0.55f, arrow,
              true);
    graphics.DrawString(FormatBytes(snapshot.totalDown, s).c_str(), -1, &fontSmall,
                        RectF(margin + arrow + (REAL)(6.0 * scale), y,
                              (REAL)width / 2, base * 1.5f),
                        &left, &textBrush);
    REAL midX = (REAL)width / 2.0f + (REAL)(4.0 * scale);
    DrawArrow(graphics, upColor, midX + arrow / 2.0f, y + base * 0.55f, arrow,
              false);
    graphics.DrawString(FormatBytes(snapshot.totalUp, s).c_str(), -1, &fontSmall,
                        RectF(midX + arrow + (REAL)(6.0 * scale), y,
                              (REAL)width / 2, base * 1.5f),
                        &left, &textBrush);

    // Reset button
    PanelMetrics metrics = ComputePanelMetrics(scale);
    GraphicsPath buttonPath;
    AddRoundedRect(buttonPath, (REAL)metrics.resetButton.left,
                   (REAL)metrics.resetButton.top,
                   (REAL)(metrics.resetButton.right - metrics.resetButton.left),
                   (REAL)(metrics.resetButton.bottom - metrics.resetButton.top),
                   (REAL)(6.0 * scale));
    bool hoverReset = g_hoverState.load() == 2;
    SolidBrush buttonBrush(BlendColor(textColor, hoverReset ? 56 : 30));
    graphics.FillPath(&buttonBrush, &buttonPath);
    Pen buttonPen(BlendColor(textColor, 70), (REAL)(1.0 * scale));
    graphics.DrawPath(&buttonPen, &buttonPath);

    StringFormat centered;
    centered.SetAlignment(StringAlignmentCenter);
    centered.SetLineAlignment(StringAlignmentCenter);
    graphics.DrawString(L"Reset", -1, &fontSmall,
                        RectF((REAL)metrics.resetButton.left,
                              (REAL)metrics.resetButton.top,
                              (REAL)(metrics.resetButton.right -
                                     metrics.resetButton.left),
                              (REAL)(metrics.resetButton.bottom -
                                     metrics.resetButton.top)),
                        &centered, &textBrush);
}
// --- Taskbar geometry ------------------------------------------------------
// The widget is positioned relative to the taskbar rectangle, never with fixed
// screen coordinates. Works for bottom/top/left/right taskbars, any DPI, and
// multiple monitors (primary taskbar preferred, secondary as fallback).
enum class TaskbarEdge { Bottom, Top, Left, Right };

// Pure function so it can be unit tested with synthetic rectangles: which screen
// edge is this taskbar docked to?
static TaskbarEdge ClassifyTaskbarEdge(const RECT& taskbar, const RECT& screen) {
    int width = taskbar.right - taskbar.left;
    int height = taskbar.bottom - taskbar.top;
    if (width >= height) {
        return (taskbar.top - screen.top) <= (screen.bottom - taskbar.bottom)
                   ? TaskbarEdge::Top
                   : TaskbarEdge::Bottom;
    }
    return (taskbar.left - screen.left) <= (screen.right - taskbar.right)
               ? TaskbarEdge::Left
               : TaskbarEdge::Right;
}

struct TaskbarInfo {
    HWND hwnd = nullptr;
    RECT rect{};
    bool visible = false;
    TaskbarEdge edge = TaskbarEdge::Bottom;
};

static bool GetTaskbarInfo(TaskbarInfo* out) {
    HWND taskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (!taskbar) {
        // Explorer may be restarting, or this monitor only has a secondary bar.
        taskbar = FindWindowW(L"Shell_SecondaryTrayWnd", nullptr);
    }
    if (!taskbar || !IsWindow(taskbar)) {
        return false;
    }

    RECT rect{};
    if (!GetWindowRect(taskbar, &rect)) {
        return false;
    }

    out->hwnd = taskbar;
    out->rect = rect;
    out->visible = IsWindowVisible(taskbar) != FALSE;

    HMONITOR monitor = MonitorFromWindow(taskbar, MONITOR_DEFAULTTONEAREST);
    MONITORINFO monitorInfo{sizeof(MONITORINFO)};
    if (GetMonitorInfoW(monitor, &monitorInfo)) {
        out->edge = ClassifyTaskbarEdge(rect, monitorInfo.rcMonitor);
    }
    return true;
}

// Where should the widget sit, given a taskbar rect and edge? Also pure, so the
// bottom/top/left/right placement can be verified without a real taskbar.
static void ComputeWidgetOrigin(const RECT& taskbar,
                                TaskbarEdge edge,
                                int width,
                                int height,
                                int offsetX,
                                int offsetY,
                                int* x,
                                int* y) {
    int taskbarWidth = taskbar.right - taskbar.left;
    int taskbarHeight = taskbar.bottom - taskbar.top;

    switch (edge) {
        case TaskbarEdge::Left:
        case TaskbarEdge::Right:
            // Vertical taskbar: stack the widget from the top, centered.
            *x = taskbar.left + (taskbarWidth - width) / 2 + offsetX;
            *y = taskbar.top + offsetY;
            break;
        case TaskbarEdge::Top:
        case TaskbarEdge::Bottom:
        default:
            *x = taskbar.left + offsetX;
            *y = taskbar.top + (taskbarHeight - height) / 2 + offsetY;
            break;
    }
}

// Widget size in physical pixels for the taskbar's monitor.
static void GetWidgetSize(HWND hwnd, int* width, int* height) {
    ModSettings s = GetSettings();
    double scale = GetScaleForWindow(hwnd);
    *width = (int)(s.width * scale);
    *height = (int)(s.height * scale);
}

static bool ComputeWidgetPosition(HWND hwnd, int* x, int* y, int* width,
                                  int* height) {
    TaskbarInfo taskbar;
    if (!GetTaskbarInfo(&taskbar)) {
        return false;
    }

    GetWidgetSize(hwnd, width, height);

    ModSettings s = GetSettings();
    double scale = GetScaleForWindow(hwnd);
    int offsetX = (int)(s.offsetX * scale);
    int offsetY = (int)(s.offsetY * scale);

    ComputeWidgetOrigin(taskbar.rect, taskbar.edge, *width, *height, offsetX,
                        offsetY, x, y);

    // Keep the widget on the taskbar's monitor.
    HMONITOR monitor = MonitorFromWindow(taskbar.hwnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFO monitorInfo{sizeof(MONITORINFO)};
    if (GetMonitorInfoW(monitor, &monitorInfo)) {
        const RECT& screen = monitorInfo.rcMonitor;
        if (*x + *width > screen.right) {
            *x = screen.right - *width;
        }
        if (*x < screen.left) {
            *x = screen.left;
        }
        if (*y + *height > screen.bottom) {
            *y = screen.bottom - *height;
        }
        if (*y < screen.top) {
            *y = screen.top;
        }
    }
    return true;
}

static bool ShouldHideForFullscreen() {
    if (!GetSettings().hideFullscreen) {
        return false;
    }
    QUERY_USER_NOTIFICATION_STATE state;
    if (SUCCEEDED(SHQueryUserNotificationState(&state))) {
        return state == QUNS_BUSY || state == QUNS_RUNNING_D3D_FULL_SCREEN ||
               state == QUNS_PRESENTATION_MODE;
    }
    return false;
}

static void RepositionWidget(HWND hwnd) {
    ModSettings s = GetSettings();

    TaskbarInfo taskbar;
    bool haveTaskbar = GetTaskbarInfo(&taskbar);

    bool shouldHide = g_hiddenByUser.load() || !s.startEnabled ||
                      !haveTaskbar || !taskbar.visible ||
                      ShouldHideForFullscreen();

    if (shouldHide) {
        if (IsWindowVisible(hwnd)) {
            ShowWindow(hwnd, SW_HIDE);
            if (g_hPanel && g_panelVisible.load()) {
                ShowWindow(g_hPanel, SW_HIDE);
                g_panelVisible = false;
            }
        }
        return;
    }

    int x = 0, y = 0, width = 0, height = 0;
    if (!ComputeWidgetPosition(hwnd, &x, &y, &width, &height)) {
        return;
    }

    RECT current{};
    GetWindowRect(hwnd, &current);
    if (current.left != x || current.top != y ||
        (current.right - current.left) != width ||
        (current.bottom - current.top) != height) {
        SetWindowPos(hwnd, HWND_TOPMOST, x, y, width, height,
                     SWP_NOACTIVATE | SWP_NOOWNERZORDER);
    }

    if (!IsWindowVisible(hwnd)) {
        ShowWindow(hwnd, SW_SHOWNOACTIVATE);
    }
}

// --- Taskbar event hook ----------------------------------------------------
static bool IsTaskbarWindow(HWND hwnd) {
    if (!hwnd) {
        return false;
    }
    WCHAR className[64] = {};
    GetClassNameW(hwnd, className, ARRAYSIZE(className));
    return wcscmp(className, L"Shell_TrayWnd") == 0 ||
           wcscmp(className, L"Shell_SecondaryTrayWnd") == 0;
}

static void CALLBACK TaskbarEventProc(HWINEVENTHOOK,
                                      DWORD,
                                      HWND hwnd,
                                      LONG,
                                      LONG,
                                      DWORD,
                                      DWORD) {
    if (!IsTaskbarWindow(hwnd) || !g_hWidget) {
        return;
    }
    PostMessage(g_hWidget, APP_WM_REPOSITION, 0, 0);
}

static void RegisterTaskbarHook(HWND hwnd) {
    if (g_taskbarHook) {
        UnhookWinEvent(g_taskbarHook);
        g_taskbarHook = nullptr;
    }

    HWND taskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (taskbar) {
        DWORD processId = 0;
        DWORD threadId = GetWindowThreadProcessId(taskbar, &processId);
        if (threadId) {
            g_taskbarHook = SetWinEventHook(
                EVENT_OBJECT_LOCATIONCHANGE, EVENT_OBJECT_LOCATIONCHANGE, nullptr,
                TaskbarEventProc, processId, threadId,
                WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
        }
        g_hookedTaskbar = taskbar;
    }
    PostMessage(hwnd, APP_WM_REPOSITION, 0, 0);
}

// --- Tooltip ---------------------------------------------------------------
static std::wstring BuildTooltipText() {
    ModSettings s = GetSettings();
    NetSnapshot snapshot;
    {
        std::lock_guard<std::mutex> guard(g_snapshotMutex);
        snapshot = g_snapshot;
    }

    std::wstring text = L"Network\n\n";
    text += L"Download: " + FormatSpeed(snapshot.downBytesPerSec, s) + L"\n";
    text += L"Upload: " + FormatSpeed(snapshot.upBytesPerSec, s) + L"\n\n";
    text += (s.counterMode == CounterMode::Persistent ? L"Total Download: "
                                                      : L"Session Download: ");
    text += FormatBytes(snapshot.totalDown, s) + L"\n";
    text += (s.counterMode == CounterMode::Persistent ? L"Total Upload: "
                                                      : L"Session Upload: ");
    text += FormatBytes(snapshot.totalUp, s) + L"\n\n";
    text += L"Interface: " + snapshot.ifName;
    if (!snapshot.ifKind.empty() && snapshot.ifKind != snapshot.ifName) {
        text += L" (" + snapshot.ifKind + L")";
    }
    text += L"\n";
    text += L"Status: ";
    text += snapshot.connected ? L"Connected" : L"Disconnected";
    if (!snapshot.ipv4.empty()) {
        text += L"\nIPv4: " + snapshot.ipv4;
    }
    return text;
}

static void CreateTooltipFor(HWND owner) {
    if (g_hTooltip) {
        return;
    }

    g_hTooltip = CreateWindowExW(WS_EX_TOPMOST, TOOLTIPS_CLASSW, nullptr,
                                 WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP, 0, 0, 0,
                                 0, owner, nullptr, GetModuleHandleW(nullptr),
                                 nullptr);
    if (!g_hTooltip) {
        return;
    }

    static std::wstring tooltipText;
    tooltipText = BuildTooltipText();

    TTTOOLINFOW info{};
    info.cbSize = sizeof(info);
    info.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
    info.hwnd = owner;
    info.uId = (UINT_PTR)owner;
    info.lpszText = (LPWSTR)tooltipText.c_str();
    SendMessageW(g_hTooltip, TTM_ADDTOOLW, 0, (LPARAM)&info);
    SendMessageW(g_hTooltip, TTM_SETMAXTIPWIDTH, 0, 320);
}

static void UpdateTooltipText(HWND owner) {
    if (!g_hTooltip) {
        return;
    }
    static std::wstring tooltipText;
    tooltipText = BuildTooltipText();

    TTTOOLINFOW info{};
    info.cbSize = sizeof(info);
    info.hwnd = owner;
    info.uId = (UINT_PTR)owner;
    info.lpszText = (LPWSTR)tooltipText.c_str();
    SendMessageW(g_hTooltip, TTM_UPDATETIPTEXTW, 0, (LPARAM)&info);
}

static void DestroyTooltip() {
    if (g_hTooltip) {
        DestroyWindow(g_hTooltip);
        g_hTooltip = nullptr;
    }
}

// --- Reset helpers ---------------------------------------------------------
static void RequestReset(bool download, bool upload) {
    unsigned long long flags = (download ? 1ull : 0ull) | (upload ? 2ull : 0ull);
    if (!flags) {
        return;
    }
    g_pendingReset.fetch_or(flags);
    if (g_wakeEvent) {
        SetEvent(g_wakeEvent);
    }
}

// --- Details panel window --------------------------------------------------
static LRESULT CALLBACK PanelWndProc(HWND hwnd,
                                     UINT msg,
                                     WPARAM wParam,
                                     LPARAM lParam) {
    switch (msg) {
        case WM_CREATE:
            UpdateAppearance(hwnd);
            return 0;

        case WM_ERASEBKGND:
            return 1;

        case WM_SETTINGCHANGE:
        case WM_THEMECHANGED:
        case WM_DPICHANGED:
            UpdateAppearance(hwnd);
            InvalidateRect(hwnd, nullptr, TRUE);
            return 0;

        case WM_ACTIVATE:
            if (LOWORD(wParam) == WA_INACTIVE) {
                ShowWindow(hwnd, SW_HIDE);
                g_panelVisible = false;
                // Clicking the widget while the panel is open deactivates the
                // panel first, so remember when that happened; TogglePanel uses
                // it to avoid immediately re-opening the panel.
                g_panelHiddenAt = GetTickCount64();
            }
            return 0;

        case WM_MOUSEMOVE: {
            double scale = GetScaleForWindow(hwnd);
            PanelMetrics metrics = ComputePanelMetrics(scale);
            POINT point{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            int newState = PtInRect(&metrics.resetButton, point) ? 2 : 0;
            if (newState != g_hoverState.load()) {
                g_hoverState = newState;
                InvalidateRect(hwnd, nullptr, FALSE);
            }
            TRACKMOUSEEVENT track{sizeof(TRACKMOUSEEVENT), TME_LEAVE, hwnd, 0};
            TrackMouseEvent(&track);
            return 0;
        }

        case WM_MOUSELEAVE:
            if (g_hoverState.load() == 2) {
                g_hoverState = 0;
                InvalidateRect(hwnd, nullptr, FALSE);
            }
            return 0;

        case WM_LBUTTONUP: {
            double scale = GetScaleForWindow(hwnd);
            PanelMetrics metrics = ComputePanelMetrics(scale);
            POINT point{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            if (PtInRect(&metrics.resetButton, point)) {
                RequestReset(true, true);
                Wh_Log(L"Traffic counters reset from the details panel");
                InvalidateRect(hwnd, nullptr, FALSE);
                if (g_hWidget) {
                    InvalidateRect(g_hWidget, nullptr, FALSE);
                }
            }
            return 0;
        }

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            RECT client{};
            GetClientRect(hwnd, &client);

            HDC memDC = CreateCompatibleDC(hdc);
            if (memDC) {
                HBITMAP bitmap =
                    CreateCompatibleBitmap(hdc, client.right, client.bottom);
                if (bitmap) {
                    HBITMAP oldBitmap = (HBITMAP)SelectObject(memDC, bitmap);
                    DrawDetailsPanel(memDC, client.right, client.bottom, hwnd);
                    BitBlt(hdc, 0, 0, client.right, client.bottom, memDC, 0, 0,
                           SRCCOPY);
                    SelectObject(memDC, oldBitmap);
                    DeleteObject(bitmap);
                }
                DeleteDC(memDC);
            }
            EndPaint(hwnd, &ps);
            return 0;
        }

        case WM_CLOSE:
            return 0;

        case APP_WM_CLOSE:
            DestroyWindow(hwnd);
            return 0;

        case WM_DESTROY:
            g_panelVisible = false;
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

static void TogglePanel(HWND widget) {
    if (!g_hPanel) {
        return;
    }

    if (g_panelVisible.load()) {
        ShowWindow(g_hPanel, SW_HIDE);
        g_panelVisible = false;
        g_panelHiddenAt = GetTickCount64();
        return;
    }

    // If the panel was dismissed by this very click (the panel loses activation
    // before the widget sees WM_LBUTTONUP), treat the click as "close" only.
    unsigned long long hiddenAt = g_panelHiddenAt.load();
    if (hiddenAt && GetTickCount64() - hiddenAt < 250) {
        return;
    }

    double scale = GetScaleForWindow(widget);
    PanelMetrics metrics = ComputePanelMetrics(scale);

    RECT widgetRect{};
    GetWindowRect(widget, &widgetRect);

    TaskbarInfo taskbar;
    bool haveTaskbar = GetTaskbarInfo(&taskbar);
    int gap = (int)(8 * scale);

    int x = widgetRect.left;
    int y = widgetRect.top - metrics.height - gap;

    if (haveTaskbar) {
        switch (taskbar.edge) {
            case TaskbarEdge::Top:
                y = taskbar.rect.bottom + gap;
                break;
            case TaskbarEdge::Left:
                x = taskbar.rect.right + gap;
                y = widgetRect.top;
                break;
            case TaskbarEdge::Right:
                x = taskbar.rect.left - metrics.width - gap;
                y = widgetRect.top;
                break;
            case TaskbarEdge::Bottom:
            default:
                y = taskbar.rect.top - metrics.height - gap;
                break;
        }
    }

    HMONITOR monitor = MonitorFromWindow(widget, MONITOR_DEFAULTTONEAREST);
    MONITORINFO monitorInfo{sizeof(MONITORINFO)};
    if (GetMonitorInfoW(monitor, &monitorInfo)) {
        const RECT& work = monitorInfo.rcWork;
        if (x + metrics.width > work.right) {
            x = work.right - metrics.width;
        }
        if (x < work.left) {
            x = work.left;
        }
        if (y < monitorInfo.rcMonitor.top) {
            y = monitorInfo.rcMonitor.top;
        }
    }

    SetWindowPos(g_hPanel, HWND_TOPMOST, x, y, metrics.width, metrics.height,
                 SWP_SHOWWINDOW | SWP_NOACTIVATE);
    g_panelVisible = true;
    InvalidateRect(g_hPanel, nullptr, TRUE);
    SetForegroundWindow(g_hPanel);  // so WM_ACTIVATE can dismiss it
}

// --- Context menu ----------------------------------------------------------
static void ShowContextMenu(HWND hwnd) {
    HMENU menu = CreatePopupMenu();
    if (!menu) {
        return;
    }

    AppendMenuW(menu, MF_STRING, IDM_REFRESH, L"Refresh");
    AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(menu, MF_STRING, IDM_RESET_BOTH, L"Reset counters (both)");
    AppendMenuW(menu, MF_STRING, IDM_RESET_DL, L"Reset download");
    AppendMenuW(menu, MF_STRING, IDM_RESET_UL, L"Reset upload");
    AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(menu, MF_STRING, IDM_NET_SETTINGS, L"Open Network Settings");
    AppendMenuW(menu, MF_STRING, IDM_WH_SETTINGS, L"Windhawk Settings");
    AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(menu, MF_STRING, IDM_HIDE, L"Hide widget");

    POINT cursor{};
    GetCursorPos(&cursor);

    SetForegroundWindow(hwnd);
    UINT command = TrackPopupMenu(menu,
                                  TPM_RIGHTBUTTON | TPM_RETURNCMD | TPM_NONOTIFY,
                                  cursor.x, cursor.y, 0, hwnd, nullptr);
    DestroyMenu(menu);

    switch (command) {
        case IDM_REFRESH:
            if (g_wakeEvent) {
                SetEvent(g_wakeEvent);
            }
            InvalidateRect(hwnd, nullptr, FALSE);
            break;
        case IDM_RESET_DL:
            RequestReset(true, false);
            break;
        case IDM_RESET_UL:
            RequestReset(false, true);
            break;
        case IDM_RESET_BOTH:
            RequestReset(true, true);
            break;
        case IDM_NET_SETTINGS:
            ShellExecuteW(nullptr, L"open", L"ms-settings:network", nullptr,
                          nullptr, SW_SHOWNORMAL);
            break;
        case IDM_WH_SETTINGS: {
            WCHAR path[MAX_PATH];
            if (GetModuleFileNameW(nullptr, path, ARRAYSIZE(path)) &&
                GetModuleFileNameW(nullptr, path, ARRAYSIZE(path)) < MAX_PATH) {
                ShellExecuteW(nullptr, L"open", path, nullptr, nullptr,
                              SW_SHOWNORMAL);
            }
            break;
        }
        case IDM_HIDE:
            g_hiddenByUser = true;
            ShowWindow(hwnd, SW_HIDE);
            if (g_hPanel && g_panelVisible.load()) {
                ShowWindow(g_hPanel, SW_HIDE);
                g_panelVisible = false;
            }
            Wh_Log(L"Widget hidden from the context menu; re-enable it by reloading the mod or changing settings");
            break;
        default:
            break;
    }
}
// --- Widget window ---------------------------------------------------------
static LRESULT CALLBACK WidgetWndProc(HWND hwnd,
                                      UINT msg,
                                      WPARAM wParam,
                                      LPARAM lParam) {
    switch (msg) {
        case WM_CREATE:
            UpdateAppearance(hwnd);
            // Watchdog: catches taskbar restarts, monitor/DPI/resolution changes
            // and fullscreen transitions that produce no WinEvent for us.
            SetTimer(hwnd, IDT_WATCHDOG, 2000, nullptr);
            RegisterTaskbarHook(hwnd);
            return 0;

        case WM_ERASEBKGND:
            return 1;

        case WM_CLOSE:
            return 0;

        case APP_WM_CLOSE:
            DestroyWindow(hwnd);
            return 0;

        case WM_DESTROY:
            KillTimer(hwnd, IDT_WATCHDOG);
            if (g_taskbarHook) {
                UnhookWinEvent(g_taskbarHook);
                g_taskbarHook = nullptr;
            }
            DestroyTooltip();
            PostQuitMessage(0);
            return 0;

        case WM_SETTINGCHANGE:
        case WM_THEMECHANGED:
        case WM_DWMCOMPOSITIONCHANGED:
            UpdateAppearance(hwnd);
            InvalidateRect(hwnd, nullptr, TRUE);
            if (g_hPanel) {
                UpdateAppearance(g_hPanel);
                InvalidateRect(g_hPanel, nullptr, TRUE);
            }
            return 0;

        case WM_DPICHANGED:
        case WM_DISPLAYCHANGE:
            PostMessage(hwnd, APP_WM_REPOSITION, 0, 0);
            InvalidateRect(hwnd, nullptr, TRUE);
            return 0;

        case WM_TIMER:
            if (wParam == IDT_WATCHDOG) {
                HWND taskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
                if (taskbar != g_hookedTaskbar) {
                    // Explorer restarted: re-hook and reposition.
                    Wh_Log(L"Taskbar window changed, re-registering hook");
                    RegisterTaskbarHook(hwnd);
                }
                RepositionWidget(hwnd);
            }
            return 0;

        case APP_WM_REPOSITION:
            RepositionWidget(hwnd);
            return 0;

        case APP_WM_DATA_UPDATED:
            InvalidateRect(hwnd, nullptr, FALSE);
            if (g_hPanel && g_panelVisible.load()) {
                InvalidateRect(g_hPanel, nullptr, FALSE);
            }
            if (GetSettings().showTooltip) {
                UpdateTooltipText(hwnd);
            }
            return 0;

        case APP_WM_TOGGLE_PANEL:
            TogglePanel(hwnd);
            return 0;

        case WM_MOUSEMOVE: {
            if (g_hoverState.load() != 1) {
                g_hoverState = 1;
                InvalidateRect(hwnd, nullptr, FALSE);
            }
            TRACKMOUSEEVENT track{sizeof(TRACKMOUSEEVENT), TME_LEAVE, hwnd, 0};
            TrackMouseEvent(&track);
            return 0;
        }

        case WM_MOUSELEAVE:
            if (g_hoverState.load() == 1) {
                g_hoverState = 0;
                InvalidateRect(hwnd, nullptr, FALSE);
            }
            return 0;

        case WM_LBUTTONUP:
            if (GetSettings().showDetails) {
                TogglePanel(hwnd);
            }
            return 0;

        case WM_RBUTTONUP:
            ShowContextMenu(hwnd);
            return 0;

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            RECT client{};
            GetClientRect(hwnd, &client);

            HDC memDC = CreateCompatibleDC(hdc);
            if (memDC) {
                HBITMAP bitmap =
                    CreateCompatibleBitmap(hdc, client.right, client.bottom);
                if (bitmap) {
                    HBITMAP oldBitmap = (HBITMAP)SelectObject(memDC, bitmap);
                    DrawNetworkPanel(memDC, client.right, client.bottom, hwnd);
                    BitBlt(hdc, 0, 0, client.right, client.bottom, memDC, 0, 0,
                           SRCCOPY);
                    SelectObject(memDC, oldBitmap);
                    DeleteObject(bitmap);
                }
                DeleteDC(memDC);
            }
            EndPaint(hwnd, &ps);
            return 0;
        }

        default:
            if (g_taskbarCreatedMsg && msg == g_taskbarCreatedMsg) {
                Wh_Log(L"TaskbarCreated received, re-attaching");
                RegisterTaskbarHook(hwnd);
                return 0;
            }
            break;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

// --- UI thread -------------------------------------------------------------
// Owns both windows, the GDI+ token and the message loop. All window handling
// happens here; the network worker only posts messages.
static void UiThread() {
    // Per-monitor DPI so GetDpiForWindow reports the real monitor scaling.
    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    if (user32) {
        using SetThreadDpiAwarenessContext_t =
            DPI_AWARENESS_CONTEXT(WINAPI*)(DPI_AWARENESS_CONTEXT);
        auto setThreadDpiAwarenessContext =
            (SetThreadDpiAwarenessContext_t)GetProcAddress(
                user32, "SetThreadDpiAwarenessContext");
        if (setThreadDpiAwarenessContext) {
            setThreadDpiAwarenessContext(
                DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
        }
    }

    HRESULT comInit = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    GdiplusStartupInput startupInput;
    if (GdiplusStartup(&g_gdiplusToken, &startupInput, nullptr) != Ok) {
        Wh_Log(L"GdiplusStartup failed");
        g_gdiplusToken = 0;
    }

    INITCOMMONCONTROLSEX iccex{sizeof(INITCOMMONCONTROLSEX), ICC_BAR_CLASSES};
    InitCommonControlsEx(&iccex);

    g_taskbarCreatedMsg = RegisterWindowMessageW(L"TaskbarCreated");

    HINSTANCE instance = GetModuleHandleW(nullptr);

    WNDCLASSW widgetClass{};
    widgetClass.lpfnWndProc = WidgetWndProc;
    widgetClass.hInstance = instance;
    widgetClass.lpszClassName = kWidgetClass;
    widgetClass.hCursor = LoadCursorW(nullptr, IDC_HAND);
    RegisterClassW(&widgetClass);

    WNDCLASSW panelClass{};
    panelClass.lpfnWndProc = PanelWndProc;
    panelClass.hInstance = instance;
    panelClass.lpszClassName = kPanelClass;
    panelClass.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    RegisterClassW(&panelClass);

    ModSettings settings = GetSettings();
    const DWORD exStyle = WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST |
                          WS_EX_NOACTIVATE;

    // Prefer CreateWindowInBand so the widget can sit above the taskbar band,
    // exactly like the reference mod; fall back to CreateWindowEx when the
    // undocumented export is unavailable.
    pCreateWindowInBand createWindowInBand = nullptr;
    if (user32) {
        createWindowInBand =
            (pCreateWindowInBand)GetProcAddress(user32, "CreateWindowInBand");
    }

    if (createWindowInBand) {
        g_hWidget = createWindowInBand(
            exStyle, kWidgetClass, L"Taskbar Network Lounge", WS_POPUP, 0, 0,
            settings.width, settings.height, nullptr, nullptr, instance, nullptr,
            ZBID_IMMERSIVE_NOTIFICATION);
        if (g_hWidget) {
            Wh_Log(L"Widget created in ZBID_IMMERSIVE_NOTIFICATION band");
        }
    }

    if (!g_hWidget) {
        Wh_Log(L"CreateWindowInBand unavailable, falling back to CreateWindowEx");
        g_hWidget =
            CreateWindowExW(exStyle, kWidgetClass, L"Taskbar Network Lounge",
                            WS_POPUP, 0, 0, settings.width, settings.height,
                            nullptr, nullptr, instance, nullptr);
    }

    if (!g_hWidget) {
        Wh_Log(L"Failed to create the widget window");
        if (g_gdiplusToken) {
            GdiplusShutdown(g_gdiplusToken);
            g_gdiplusToken = 0;
        }
        if (SUCCEEDED(comInit)) {
            CoUninitialize();
        }
        return;
    }

    SetLayeredWindowAttributes(g_hWidget, 0, 255, LWA_ALPHA);

    // Details panel: same acrylic treatment, created hidden. It is activatable
    // (no WS_EX_NOACTIVATE) so clicking elsewhere dismisses it.
    double scale = GetScaleForWindow(g_hWidget);
    PanelMetrics metrics = ComputePanelMetrics(scale);
    g_hPanel = CreateWindowExW(WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST,
                               kPanelClass, L"Network Details", WS_POPUP, 0, 0,
                               metrics.width, metrics.height, nullptr, nullptr,
                               instance, nullptr);
    if (g_hPanel) {
        SetLayeredWindowAttributes(g_hPanel, 0, 255, LWA_ALPHA);
    } else {
        Wh_Log(L"Failed to create the details panel window");
    }

    if (settings.showTooltip) {
        CreateTooltipFor(g_hWidget);
    }

    RepositionWidget(g_hWidget);

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    // Ordered teardown: panel, tooltip, widget, classes, GDI+, COM.
    if (g_hPanel) {
        DestroyWindow(g_hPanel);
        g_hPanel = nullptr;
    }
    DestroyTooltip();
    g_hWidget = nullptr;

    UnregisterClassW(kPanelClass, instance);
    UnregisterClassW(kWidgetClass, instance);

    if (g_gdiplusToken) {
        GdiplusShutdown(g_gdiplusToken);
        g_gdiplusToken = 0;
    }
    if (SUCCEEDED(comInit)) {
        CoUninitialize();
    }
}

// --- Windhawk tool mod callbacks -------------------------------------------
BOOL WhTool_ModInit() {
    Wh_Log(L"Taskbar Network Lounge starting");

    LoadSettings();
    ApplyResetSettingIfChanged();

    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_wakeEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (!g_stopEvent || !g_wakeEvent) {
        Wh_Log(L"CreateEvent failed");
        if (g_stopEvent) {
            CloseHandle(g_stopEvent);
            g_stopEvent = nullptr;
        }
        if (g_wakeEvent) {
            CloseHandle(g_wakeEvent);
            g_wakeEvent = nullptr;
        }
        return FALSE;
    }

    g_running = true;

    try {
        g_uiThread = new std::thread(UiThread);
        g_workerThread = new std::thread(NetworkWorkerThread);
    } catch (...) {
        Wh_Log(L"Failed to start threads");
        return FALSE;
    }

    return TRUE;
}

void WhTool_ModUninit() {
    Wh_Log(L"Taskbar Network Lounge stopping");
    g_running = false;

    if (g_stopEvent) {
        SetEvent(g_stopEvent);
    }
    if (g_wakeEvent) {
        SetEvent(g_wakeEvent);
    }

    if (g_workerThread) {
        if (g_workerThread->joinable()) {
            g_workerThread->join();
        }
        delete g_workerThread;
        g_workerThread = nullptr;
    }

    if (g_hWidget) {
        PostMessage(g_hWidget, APP_WM_CLOSE, 0, 0);
    }

    if (g_uiThread) {
        if (g_uiThread->joinable()) {
            g_uiThread->join();
        }
        delete g_uiThread;
        g_uiThread = nullptr;
    }

    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }
    if (g_wakeEvent) {
        CloseHandle(g_wakeEvent);
        g_wakeEvent = nullptr;
    }
}

void WhTool_ModSettingsChanged() {
    LoadSettings();
    ApplyResetSettingIfChanged();

    // Turning "Start enabled" back on un-hides a widget hidden from the menu.
    if (GetSettings().startEnabled) {
        g_hiddenByUser = false;
    }

    if (g_hWidget) {
        PostMessage(g_hWidget, WM_SETTINGCHANGE, 0, 0);
        PostMessage(g_hWidget, APP_WM_REPOSITION, 0, 0);
        PostMessage(g_hWidget, APP_WM_DATA_UPDATED, 0, 0);
    }
    if (g_wakeEvent) {
        SetEvent(g_wakeEvent);
    }
}

////////////////////////////////////////////////////////////////////////////////
// Windhawk tool mod implementation for mods which don't need to inject to other
// processes or hook other functions. Context:
// https://github.com/ramensoftware/windhawk/wiki/Mods-as-tools:-Running-mods-in-a-dedicated-process
//
// The mod will load and run in a dedicated windhawk.exe process.
//
// Paste the code below as part of the mod code, and use these callbacks:
// * WhTool_ModInit
// * WhTool_ModSettingsChanged
// * WhTool_ModUninit
//
// Currently, other callbacks are not supported.

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}

BOOL Wh_ModInit() {
    Wh_Log(L"Wh_ModInit in pid %lu, command line: %s", GetCurrentProcessId(),
           GetCommandLine());

    // Never run in the session 0 service instance.
    DWORD sessionId = 0;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) &&
        sessionId == 0) {
        Wh_Log(L"Session 0, not loading");
        return FALSE;
    }

    bool isService = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);
    if (!argv) {
        Wh_Log(L"CommandLineToArgvW failed");
        return FALSE;
    }

    for (int i = 1; i < argc; i++) {
        if (wcscmp(argv[i], L"-service") == 0 ||
            wcscmp(argv[i], L"-service-start") == 0 ||
            wcscmp(argv[i], L"-service-stop") == 0) {
            isService = true;
            break;
        }
    }

    for (int i = 1; i < argc - 1; i++) {
        if (wcscmp(argv[i], L"-tool-mod") == 0) {
            isToolModProcess = true;
            if (wcscmp(argv[i + 1], WH_MOD_ID) == 0) {
                isCurrentToolModProcess = true;
            }
            break;
        }
    }

    LocalFree(argv);

    if (isService) {
        return FALSE;
    }

    if (isCurrentToolModProcess) {
        g_toolModProcessMutex =
            CreateMutex(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
        if (!g_toolModProcessMutex) {
            Wh_Log(L"CreateMutex failed");
            ExitProcess(1);
        }

        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            Wh_Log(L"Tool mod already running (%s)", WH_MOD_ID);
            ExitProcess(1);
        }

        if (!WhTool_ModInit()) {
            ExitProcess(1);
        }

        IMAGE_DOS_HEADER* dosHeader =
            (IMAGE_DOS_HEADER*)GetModuleHandle(nullptr);
        IMAGE_NT_HEADERS* ntHeaders =
            (IMAGE_NT_HEADERS*)((BYTE*)dosHeader + dosHeader->e_lfanew);

        DWORD entryPointRVA = ntHeaders->OptionalHeader.AddressOfEntryPoint;
        void* entryPoint = (BYTE*)dosHeader + entryPointRVA;

        Wh_SetFunctionHook(entryPoint, (void*)EntryPoint_Hook, nullptr);
        return TRUE;
    }

    if (isToolModProcess) {
        return FALSE;
    }

    g_isToolModProcessLauncher = true;
    return TRUE;
}

void Wh_ModAfterInit() {
    if (!g_isToolModProcessLauncher) {
        return;
    }

    Wh_Log(L"Launching the tool mod process");

    WCHAR currentProcessPath[MAX_PATH];
    switch (GetModuleFileName(nullptr, currentProcessPath,
                              ARRAYSIZE(currentProcessPath))) {
        case 0:
        case ARRAYSIZE(currentProcessPath):
            Wh_Log(L"GetModuleFileName failed");
            return;
    }

    WCHAR
    commandLine[MAX_PATH + 2 +
                (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) - 1];
    swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"", currentProcessPath,
               WH_MOD_ID);

    HMODULE kernelModule = GetModuleHandle(L"kernelbase.dll");
    if (!kernelModule) {
        kernelModule = GetModuleHandle(L"kernel32.dll");
        if (!kernelModule) {
            Wh_Log(L"No kernelbase.dll/kernel32.dll");
            return;
        }
    }

    using CreateProcessInternalW_t = BOOL(WINAPI*)(
        HANDLE hUserToken, LPCWSTR lpApplicationName, LPWSTR lpCommandLine,
        LPSECURITY_ATTRIBUTES lpProcessAttributes,
        LPSECURITY_ATTRIBUTES lpThreadAttributes, WINBOOL bInheritHandles,
        DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory,
        LPSTARTUPINFOW lpStartupInfo,
        LPPROCESS_INFORMATION lpProcessInformation,
        PHANDLE hRestrictedUserToken);
    CreateProcessInternalW_t pCreateProcessInternalW =
        (CreateProcessInternalW_t)GetProcAddress(kernelModule,
                                                 "CreateProcessInternalW");
    if (!pCreateProcessInternalW) {
        Wh_Log(L"No CreateProcessInternalW");
        return;
    }

    STARTUPINFO si{
        .cb = sizeof(STARTUPINFO),
        .dwFlags = STARTF_FORCEOFFFEEDBACK,
    };
    PROCESS_INFORMATION pi;
    if (!pCreateProcessInternalW(nullptr, currentProcessPath, commandLine,
                                 nullptr, nullptr, FALSE, NORMAL_PRIORITY_CLASS,
                                 nullptr, nullptr, &si, &pi, nullptr)) {
        Wh_Log(L"CreateProcess failed");
        return;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

void Wh_ModSettingsChanged() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModSettingsChanged();
}

void Wh_ModUninit() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModUninit();
    ExitProcess(0);
}
