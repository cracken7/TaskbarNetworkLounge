// taskbartest.cpp - non-destructive taskbar-recovery tests for Taskbar Network
// Lounge. Instead of killing explorer.exe, this reproduces the two events the
// mod actually reacts to:
//
//   broadcast  - broadcast the real "TaskbarCreated" message (what explorer
//                sends after the taskbar is recreated) -> mod must re-hook and
//                reposition.
//   hide       - hide Shell_TrayWnd for N seconds, then restore it -> mod must
//                hide the widget while the taskbar is gone and bring it back.
//
// Usage: taskbartest.exe broadcast
//        taskbartest.exe hide [seconds]
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void PrintWidget(const char* label) {
    HWND widget = FindWindowW(L"WindhawkNetworkLoungeWidget", nullptr);
    if (!widget) {
        printf("%s: widget NOT FOUND\n", label);
        return;
    }
    RECT r{};
    GetWindowRect(widget, &r);
    printf("%s: widget hwnd=%p visible=%d rect=(%ld,%ld) size=%ldx%ld\n", label,
           (void*)widget, IsWindowVisible(widget) ? 1 : 0, r.left, r.top,
           r.right - r.left, r.bottom - r.top);
}

int main(int argc, char** argv) {
    SetProcessDPIAware();
    const char* cmd = argc > 1 ? argv[1] : "broadcast";

    if (!strcmp(cmd, "broadcast")) {
        UINT msg = RegisterWindowMessageW(L"TaskbarCreated");
        PrintWidget("before");
        DWORD recipients = BSM_APPLICATIONS;
        BroadcastSystemMessageW(BSF_POSTMESSAGE | BSF_IGNORECURRENTTASK,
                                &recipients, msg, 0, 0);
        printf("TaskbarCreated broadcast (msg=0x%X)\n", msg);
        Sleep(2500);
        PrintWidget("after ");
        return 0;
    }

    if (!strcmp(cmd, "hide")) {
        int seconds = argc > 2 ? atoi(argv[2]) : 6;
        HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr);
        if (!tray) {
            printf("Shell_TrayWnd not found\n");
            return 1;
        }
        PrintWidget("before  ");
        ShowWindow(tray, SW_HIDE);
        printf("taskbar hidden for %d s\n", seconds);
        Sleep(1500);
        PrintWidget("hidden  ");
        Sleep((DWORD)seconds * 1000);
        ShowWindow(tray, SW_SHOW);
        printf("taskbar restored\n");
        Sleep(3000);
        PrintWidget("restored");
        // Safety: make sure the taskbar is visible even if something above threw.
        if (!IsWindowVisible(tray)) {
            ShowWindow(tray, SW_SHOW);
            printf("taskbar force-restored\n");
        }
        return 0;
    }

    printf("usage: taskbartest.exe broadcast | hide [seconds]\n");
    return 1;
}
