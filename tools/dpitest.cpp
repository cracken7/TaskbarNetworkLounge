// dpitest.cpp - report the DPI the widget's monitor is using and how the mod's
// scaling maps to physical pixels, then compare with the widget's real size.
//
// Usage: dpitest.exe
#include <windows.h>
#include <shellscalingapi.h>
#include <stdio.h>

typedef UINT(WINAPI* GetDpiForWindow_t)(HWND);
typedef UINT(WINAPI* GetDpiForSystem_t)();
typedef DPI_AWARENESS_CONTEXT(WINAPI* SetThreadDpiAwarenessContext_t)(
    DPI_AWARENESS_CONTEXT);

int main() {
    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    auto setCtx = (SetThreadDpiAwarenessContext_t)GetProcAddress(
        user32, "SetThreadDpiAwarenessContext");
    if (setCtx) {
        setCtx(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    }
    auto getDpiForWindow =
        (GetDpiForWindow_t)GetProcAddress(user32, "GetDpiForWindow");
    auto getDpiForSystem =
        (GetDpiForSystem_t)GetProcAddress(user32, "GetDpiForSystem");

    HWND widget = FindWindowW(L"WindhawkNetworkLoungeWidget", nullptr);
    HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr);

    printf("GetDpiForSystem      : %u\n", getDpiForSystem ? getDpiForSystem() : 0);

    if (tray) {
        UINT dpi = getDpiForWindow ? getDpiForWindow(tray) : 0;
        RECT r{};
        GetWindowRect(tray, &r);
        printf("taskbar dpi          : %u (%.0f%%)  rect=(%ld,%ld)-(%ld,%ld)\n",
               dpi, dpi * 100.0 / 96.0, r.left, r.top, r.right, r.bottom);
    }

    if (widget) {
        UINT dpi = getDpiForWindow ? getDpiForWindow(widget) : 0;
        RECT r{};
        GetWindowRect(widget, &r);
        double scale = dpi / 96.0;
        int w = (int)(r.right - r.left);
        int h = (int)(r.bottom - r.top);
        printf("widget dpi           : %u (%.0f%%)\n", dpi, scale * 100.0);
        printf("widget physical size : %dx%d\n", w, h);
        printf("implied logical size : %.1fx%.1f (physical / scale)\n", w / scale,
               h / scale);
    } else {
        printf("widget               : NOT FOUND\n");
    }

    // Monitor-by-monitor DPI, so multi-monitor setups can be inspected.
    struct Ctx {
        int index;
    } ctx{0};
    EnumDisplayMonitors(
        nullptr, nullptr,
        [](HMONITOR monitor, HDC, LPRECT, LPARAM param) -> BOOL {
            Ctx* c = (Ctx*)param;
            MONITORINFOEXW info{};
            info.cbSize = sizeof(info);
            GetMonitorInfoW(monitor, &info);
            UINT dpiX = 0, dpiY = 0;
            HMODULE shcore = LoadLibraryW(L"shcore.dll");
            if (shcore) {
                using GetDpiForMonitor_t =
                    HRESULT(WINAPI*)(HMONITOR, MONITOR_DPI_TYPE, UINT*, UINT*);
                auto fn = (GetDpiForMonitor_t)GetProcAddress(shcore,
                                                             "GetDpiForMonitor");
                if (fn) {
                    fn(monitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY);
                }
                FreeLibrary(shcore);
            }
            wprintf(L"monitor %d           : %ls dpi=%u (%.0f%%) rect=(%ld,%ld)-(%ld,%ld) primary=%d\n",
                    c->index++, info.szDevice, dpiX, dpiX * 100.0 / 96.0,
                    info.rcMonitor.left, info.rcMonitor.top, info.rcMonitor.right,
                    info.rcMonitor.bottom,
                    (info.dwFlags & MONITORINFOF_PRIMARY) ? 1 : 0);
            return TRUE;
        },
        (LPARAM)&ctx);

    return widget ? 0 : 1;
}
