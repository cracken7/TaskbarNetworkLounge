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
#define IDM_RESET_DIVIDER 107

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

enum class ArrowStyle { Solid, Chevron, Triangle, Rounded, Circle };

enum class TextWeight { Regular, Semibold, Bold, Black };

struct ModSettings {
    // Appearance
    int width = 220;
    int height = 52;
    int fontSize = 13;
    bool boldText = true;
    TextWeight textWeight = TextWeight::Bold;
    ArrowStyle arrowStyle = ArrowStyle::Rounded;
    int arrowScale = 120;       // percent of font size
    bool showColumnLabels = true;
    int dividerPos = 50;        // percent of the usable width
    int dividerOffset = 0;      // px nudge on top of dividerPos, set by dragging
    bool dividerDraggable = true;
    int dividerOpacity = 46;    // 0 = hidden
    int detailsWidth = 240;
    int detailsHeight = 276;
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
    bool resetOnSourceChange = true;
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
// Divider drag state. g_dividerHitX is written by the painter every frame so the
// hit test always matches the line that is actually on screen, whatever the
// layout/DPI. g_dividerOffset is the user's dragged offset in *logical* pixels
// (it is scaled at paint time), kept live here and persisted on mouse-up.
static std::atomic<float> g_dividerHitX{-1.0f};
static std::atomic<int> g_dividerOffset{0};
static std::atomic<bool> g_dividerDragging{false};
static UINT g_taskbarCreatedMsg = 0;
static HWINEVENTHOOK g_taskbarHook = nullptr;
static HWND g_hookedTaskbar = nullptr;
static ULONG_PTR g_gdiplusToken = 0;
