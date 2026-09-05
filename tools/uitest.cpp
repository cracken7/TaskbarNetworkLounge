// uitest.cpp - drive the Taskbar Network Lounge widget for testing:
// hover, left click, right click, key presses, and window queries.
//
// Usage:
//   uitest.exe find                     - print widget/panel geometry
//   uitest.exe hover                    - move the cursor onto the widget
//   uitest.exe click                    - real left click on the widget
//   uitest.exe rclick                   - real right click on the widget
//   uitest.exe key esc                  - send ESC to the foreground window
//   uitest.exe clickpanel <dx> <dy>     - click inside the details panel
//   uitest.exe away                     - park the cursor away from the widget
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static HWND FindByClass(const wchar_t* cls) {
    return FindWindowW(cls, nullptr);
}

static void PrintWindow(const wchar_t* label, HWND hwnd) {
    if (!hwnd) {
        wprintf(L"%ls: NOT FOUND\n", label);
        return;
    }
    RECT r{};
    GetWindowRect(hwnd, &r);
    wprintf(L"%ls: hwnd=%p visible=%d rect=(%ld,%ld)-(%ld,%ld) size=%ldx%ld\n",
            label, hwnd, IsWindowVisible(hwnd) ? 1 : 0, r.left, r.top, r.right,
            r.bottom, r.right - r.left, r.bottom - r.top);
}

static void MoveTo(int x, int y) {
    SetCursorPos(x, y);
    Sleep(120);
    // A tiny second move makes sure WM_MOUSEMOVE is delivered.
    SetCursorPos(x + 1, y);
    Sleep(120);
}

static void Center(HWND hwnd, int* x, int* y) {
    RECT r{};
    GetWindowRect(hwnd, &r);
    *x = r.left + (r.right - r.left) / 2;
    *y = r.top + (r.bottom - r.top) / 2;
}

int main(int argc, char** argv) {
    SetProcessDPIAware();
    const char* cmd = argc > 1 ? argv[1] : "find";

    HWND widget = FindByClass(L"WindhawkNetworkLoungeWidget");
    HWND panel = FindByClass(L"WindhawkNetworkLoungePanel");

    if (!strcmp(cmd, "find")) {
        PrintWindow(L"widget", widget);
        PrintWindow(L"panel", panel);
        HWND tray = FindByClass(L"Shell_TrayWnd");
        PrintWindow(L"taskbar", tray);
        return widget ? 0 : 1;
    }

    if (!strcmp(cmd, "away")) {
        MoveTo(GetSystemMetrics(SM_CXSCREEN) / 2,
               GetSystemMetrics(SM_CYSCREEN) / 3);
        printf("cursor parked\n");
        return 0;
    }

    if (!strcmp(cmd, "key")) {
        WORD vk = VK_ESCAPE;
        if (argc > 2 && !strcmp(argv[2], "enter")) vk = VK_RETURN;
        keybd_event((BYTE)vk, 0, 0, 0);
        keybd_event((BYTE)vk, 0, KEYEVENTF_KEYUP, 0);
        printf("key sent\n");
        return 0;
    }

    if (!widget) {
        printf("widget window not found\n");
        return 1;
    }

    int x = 0, y = 0;
    Center(widget, &x, &y);

    if (!strcmp(cmd, "hover")) {
        MoveTo(x, y);
        printf("hovering at %d,%d\n", x, y);
        return 0;
    }

    if (!strcmp(cmd, "click")) {
        MoveTo(x, y);
        mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
        Sleep(60);
        mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
        printf("left clicked at %d,%d\n", x, y);
        return 0;
    }

    if (!strcmp(cmd, "rclick")) {
        MoveTo(x, y);
        mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, 0);
        Sleep(60);
        mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);
        printf("right clicked at %d,%d\n", x, y);
        return 0;
    }

    if (!strcmp(cmd, "clickpanel")) {
        if (!panel) {
            printf("panel window not found\n");
            return 1;
        }
        RECT r{};
        GetWindowRect(panel, &r);
        int dx = argc > 2 ? atoi(argv[2]) : (r.right - r.left) / 2;
        int dy = argc > 3 ? atoi(argv[3]) : (r.bottom - r.top) - 32;
        int px = r.left + dx;
        int py = r.top + dy;
        MoveTo(px, py);
        mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
        Sleep(60);
        mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
        printf("clicked panel at %d,%d (offset %d,%d)\n", px, py, dx, dy);
        return 0;
    }

    printf("unknown command: %s\n", cmd);
    return 1;
}
