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

// A window that covers its whole monitor without being a real D3D fullscreen
// (browser kiosk/F11, video players, some games in "borderless" mode) does not
// raise the notification states above, so check the foreground window's rect
// against its monitor as well.
static bool IsForegroundWindowFullscreen() {
    HWND fg = GetForegroundWindow();
    if (!fg || fg == g_hWidget || fg == g_hPanel || fg == g_hTooltip) {
        return false;
    }
    // Never treat the desktop or the taskbar itself as "fullscreen".
    wchar_t className[64] = {};
    GetClassNameW(fg, className, ARRAYSIZE(className));
    if (!wcscmp(className, L"WorkerW") || !wcscmp(className, L"Progman") ||
        !wcscmp(className, L"Shell_TrayWnd") ||
        !wcscmp(className, L"Windows.UI.Core.CoreWindow") ||
        !wcscmp(className, L"XamlExplorerHostIslandWindow")) {
        return false;
    }
    RECT window{};
    if (!GetWindowRect(fg, &window)) {
        return false;
    }
    HMONITOR monitor = MonitorFromWindow(fg, MONITOR_DEFAULTTONEAREST);
    if (!monitor) {
        return false;
    }
    MONITORINFO info{sizeof(info)};
    if (!GetMonitorInfoW(monitor, &info)) {
        return false;
    }
    // A window is "fullscreen" when it covers the monitor completely.
    return window.left <= info.rcMonitor.left &&
           window.top <= info.rcMonitor.top &&
           window.right >= info.rcMonitor.right &&
           window.bottom >= info.rcMonitor.bottom;
}

static bool ShouldHideForFullscreen() {
    if (!GetSettings().hideFullscreen) {
        return false;
    }
    QUERY_USER_NOTIFICATION_STATE state;
    if (SUCCEEDED(SHQueryUserNotificationState(&state)) &&
        (state == QUNS_BUSY || state == QUNS_RUNNING_D3D_FULL_SCREEN ||
         state == QUNS_PRESENTATION_MODE)) {
        return true;
    }
    return IsForegroundWindowFullscreen();
}

static void LogFullscreenTransition(bool hidden) {
    HWND fg = GetForegroundWindow();
    wchar_t className[64] = {};
    wchar_t title[80] = {};
    if (fg) {
        GetClassNameW(fg, className, ARRAYSIZE(className));
        GetWindowTextW(fg, title, ARRAYSIZE(title));
    }
    Wh_Log(L"Fullscreen hide %s (foreground: %s '%s')",
           hidden ? L"engaged" : L"released", className, title);
}

// --- fullscreen state machine ----------------------------------------------
// The foreground window crosses the fullscreen boundary in moments: a
// notification stealing focus for an instant, an in-game overlay popup, the
// task switcher passing through. Flipping visibility on every check made the
// widget blink in and out, so a hide is confirmed only after two consecutive
// sightings and a restore the same way; single blips change nothing.
static int g_fsStrikes = 0;
static bool g_fullscreenHidden = false;

static void UpdateFullscreenHideState(HWND hwnd) {
    if (!GetSettings().hideFullscreen) {
        g_fsStrikes = 0;
        if (g_fullscreenHidden) {
            g_fullscreenHidden = false;
            LogFullscreenTransition(false);
            PostMessage(hwnd, APP_WM_REPOSITION, 0, 0);
        }
        return;
    }
    if (ShouldHideForFullscreen()) {
        if (g_fullscreenHidden) {
            g_fsStrikes = 2;
        } else if (++g_fsStrikes >= 2) {
            g_fullscreenHidden = true;
            LogFullscreenTransition(true);
            PostMessage(hwnd, APP_WM_REPOSITION, 0, 0);
        }
    } else if (g_fullscreenHidden && --g_fsStrikes <= 0) {
        g_fullscreenHidden = false;
        LogFullscreenTransition(false);
        PostMessage(hwnd, APP_WM_REPOSITION, 0, 0);
    } else if (!g_fullscreenHidden) {
        g_fsStrikes = 0;
    }
}

static void RepositionWidget(HWND hwnd) {
    ModSettings s = GetSettings();

    TaskbarInfo taskbar;
    bool haveTaskbar = GetTaskbarInfo(&taskbar);

    bool shouldHide = g_hiddenByUser.load() || !s.startEnabled ||
                      !haveTaskbar || !taskbar.visible || g_fullscreenHidden;

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

// EVENT_OBJECT_LOCATIONCHANGE fires very often on the taskbar thread, so the cheap
// checks come first: the hooked window itself and the window object. Only then is
// the cross-process GetClassNameW inside IsTaskbarWindow worth paying for.
static void CALLBACK TaskbarEventProc(HWINEVENTHOOK,
                                      DWORD,
                                      HWND hwnd,
                                      LONG idObject,
                                      LONG,
                                      DWORD,
                                      DWORD) {
    if (!g_hWidget || idObject != OBJID_WINDOW) {
        return;
    }
    if (hwnd != g_hookedTaskbar && !IsTaskbarWindow(hwnd)) {
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
                KillTimer(hwnd, IDT_PANEL_DISMISS);
                ShowWindow(hwnd, SW_HIDE);
                g_panelVisible = false;
                // Clicking the widget while the panel is open deactivates the
                // panel first, so remember when that happened; TogglePanel uses
                // it to avoid immediately re-opening the panel.
                g_panelHiddenAt = GetTickCount64();
            }
            return 0;

        case WM_TIMER:
            if (wParam == IDT_PANEL_DISMISS) {
                // Dismiss on a mouse-down anywhere outside the panel and outside
                // the widget (the widget's own click toggles the panel itself).
                // This is the path that actually runs, because a background
                // process is normally refused foreground activation.
                if (!g_panelVisible.load()) {
                    KillTimer(hwnd, IDT_PANEL_DISMISS);
                    return 0;
                }
                if ((GetAsyncKeyState(VK_LBUTTON) & 0x8000) == 0 &&
                    (GetAsyncKeyState(VK_RBUTTON) & 0x8000) == 0) {
                    return 0;
                }
                POINT cursor{};
                GetCursorPos(&cursor);
                RECT panelRect{}, widgetRect{};
                GetWindowRect(hwnd, &panelRect);
                if (PtInRect(&panelRect, cursor)) {
                    return 0;
                }
                if (g_hWidget && GetWindowRect(g_hWidget, &widgetRect) &&
                    PtInRect(&widgetRect, cursor)) {
                    return 0;
                }
                KillTimer(hwnd, IDT_PANEL_DISMISS);
                ShowWindow(hwnd, SW_HIDE);
                g_panelVisible = false;
                g_panelHiddenAt = GetTickCount64();
            }
            return 0;

        case WM_MOUSEMOVE: {
            double scale = GetScaleForWindow(hwnd);
            PanelMetrics metrics = ComputePanelMetrics(scale);
            POINT point{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            int newState = PtInRect(&metrics.resetButton, point) ? 1 : 0;
            if (newState != g_panelHover.load()) {
                g_panelHover = newState;
                InvalidateRect(hwnd, nullptr, FALSE);
            }
            TRACKMOUSEEVENT track{sizeof(TRACKMOUSEEVENT), TME_LEAVE, hwnd, 0};
            TrackMouseEvent(&track);
            return 0;
        }

        case WM_MOUSELEAVE:
            if (g_panelHover.load() != 0) {
                g_panelHover = 0;
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
        KillTimer(g_hPanel, IDT_PANEL_DISMISS);
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
    // Try for activation (it makes WM_ACTIVATE dismissal work when it succeeds),
    // but do not depend on it: SetForegroundWindow is normally refused for a
    // background process, so a timer polls the mouse as the reliable path.
    SetForegroundWindow(g_hPanel);
    SetTimer(g_hPanel, IDT_PANEL_DISMISS, 120, nullptr);
}

// --- Divider drag hit testing ----------------------------------------------
// The base x is the un-nudged split point: it must be computed the same way the
// painter does, so a drag offset of 0 leaves the line exactly where the layout
// puts it.
static REAL DividerBaseX(HWND hwnd, int width) {
    ModSettings s = GetSettings();
    const double scale = GetScaleForWindow(hwnd);
    REAL padding = (REAL)(9.0 * scale);
    REAL columnGap = (REAL)(10.0 * scale);
    REAL usable = (REAL)width - padding * 2.0f - columnGap;
    REAL leftWidth = usable * (s.dividerPos / 100.0f);
    return padding + leftWidth + columnGap / 2.0f;
}

// True when the pointer is within the grab zone of the line that was last
// painted. Uses the painted x (published by the renderer) rather than
// recomputing it, so hover can never disagree with what is on screen.
static bool DividerHover(HWND hwnd, int mouseX, const ModSettings& s) {
    if (!s.dividerDraggable || s.layout != LayoutMode::Full) {
        return false;
    }
    float painted = g_dividerHitX.load();
    if (painted < 0.0f) {
        return false;  // no divider on screen (other layout / not painted yet)
    }
    const double scale = GetScaleForWindow(hwnd);
    // 5 logical px each side: wide enough to grab without stealing the click
    // that opens the details panel.
    const float slack = (float)(5.0 * scale);
    return mouseX >= painted - slack && mouseX <= painted + slack;
}

// --- Context menu ----------------------------------------------------------
// Apply a live display-mode change: atomic first (painters read it), then
// storage, then repaint. Settings are NOT reloaded - the setting is only the
// default and would stomp the menu choice.
static void ApplyDisplayMode(HWND hwnd, int mode) {
    g_displayMode = ClampInt(mode, 0, 2);
    Wh_SetIntValue(L"DisplayMode", g_displayMode.load());
    InvalidateRect(hwnd, nullptr, TRUE);
    if (g_hPanel && g_panelVisible.load()) {
        InvalidateRect(g_hPanel, nullptr, TRUE);
    }
}

static void ApplyCaptionBits(HWND hwnd, int bits) {
    g_captionBits = ClampInt(bits, 0, 3);
    Wh_SetIntValue(L"CaptionBits", g_captionBits.load());
    InvalidateRect(hwnd, nullptr, TRUE);
}

static void ApplyArrowsHidden(HWND hwnd, bool hidden) {
    g_arrowsHidden = hidden ? 1 : 0;
    Wh_SetIntValue(L"ArrowsHidden", g_arrowsHidden.load());
    InvalidateRect(hwnd, nullptr, TRUE);
    if (g_hPanel && g_panelVisible.load()) {
        InvalidateRect(g_hPanel, nullptr, TRUE);
    }
}

static void ShowContextMenu(HWND hwnd) {
    HMENU menu = CreatePopupMenu();
    if (!menu) {
        return;
    }

    AppendMenuW(menu, MF_STRING, IDM_REFRESH, L"Refresh");
    AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
    {
        // What the widget shows: speed, traffic, or both. Radio check marks so
        // the current choice is obvious and one click switches.
        const int mode = ClampInt(g_displayMode.load(), 0, 2);
        AppendMenuW(menu, MF_STRING | MF_RADIOCHECK | MF_GROUPCHECK |
                                (mode == 0 ? MF_CHECKED : MF_UNCHECKED),
                    IDM_SHOW_BOTH, L"Show both");
        AppendMenuW(menu, MF_STRING | MF_RADIOCHECK |
                                (mode == 1 ? MF_CHECKED : MF_UNCHECKED),
                    IDM_SHOW_SPEED, L"Show speed only");
        AppendMenuW(menu, MF_STRING | MF_RADIOCHECK |
                                (mode == 2 ? MF_CHECKED : MF_UNCHECKED),
                    IDM_SHOW_TRAFFIC, L"Show traffic only");
        AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
        // The small captions above the two halves, and the arrows themselves.
        const int bits = ClampInt(g_captionBits.load(), 0, 3);
        AppendMenuW(menu, MF_STRING |
                                (bits & 1 ? MF_CHECKED : MF_UNCHECKED),
                    IDM_TOGGLE_CAPTION_SPEED, L"Show SPEED caption");
        AppendMenuW(menu, MF_STRING |
                                (bits & 2 ? MF_CHECKED : MF_UNCHECKED),
                    IDM_TOGGLE_CAPTION_TOTAL, L"Show TOTAL caption");
        AppendMenuW(menu, MF_STRING |
                                (g_arrowsHidden.load() ? MF_UNCHECKED
                                                       : MF_CHECKED),
                    IDM_TOGGLE_ARROWS, L"Show arrows");
        AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
    }
    AppendMenuW(menu, MF_STRING, IDM_RESET_BOTH, L"Reset counters (both)");
    AppendMenuW(menu, MF_STRING, IDM_RESET_DL, L"Reset download");
    AppendMenuW(menu, MF_STRING, IDM_RESET_UL, L"Reset upload");
    AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
    {
        ModSettings s = GetSettings();
        // Only offered when there is something to undo, so the menu does not grow
        // an item that does nothing.
        if (s.dividerDraggable && s.layout == LayoutMode::Full &&
            g_dividerOffset.load() != 0) {
            AppendMenuW(menu, MF_STRING, IDM_RESET_DIVIDER,
                        L"Reset divider position");
            AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
        }
    }
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
    // The classic TrackPopupMenu fix: SetForegroundWindow may be refused on a
    // WS_EX_NOACTIVATE window, and the menu then lingers after a click elsewhere.
    // A dummy message to the owner nudges it out of menu mode.
    PostMessage(hwnd, WM_NULL, 0, 0);
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
        case IDM_RESET_DIVIDER:
            g_dividerOffset = 0;
            Wh_SetIntValue(L"DividerOffset", 0);
            LoadSettings();
            InvalidateRect(hwnd, nullptr, FALSE);
            break;
        case IDM_SHOW_BOTH:
            ApplyDisplayMode(hwnd, 0);
            break;
        case IDM_SHOW_SPEED:
            ApplyDisplayMode(hwnd, 1);
            break;
        case IDM_SHOW_TRAFFIC:
            ApplyDisplayMode(hwnd, 2);
            break;
        case IDM_TOGGLE_CAPTION_SPEED:
            ApplyCaptionBits(hwnd, g_captionBits.load() ^ 1);
            break;
        case IDM_TOGGLE_CAPTION_TOTAL:
            ApplyCaptionBits(hwnd, g_captionBits.load() ^ 2);
            break;
        case IDM_TOGGLE_ARROWS:
            ApplyArrowsHidden(hwnd, !g_arrowsHidden.load());
            break;
        case IDM_NET_SETTINGS:
            ShellExecuteW(nullptr, L"open", L"ms-settings:network", nullptr,
                          nullptr, SW_SHOWNORMAL);
            break;
        case IDM_WH_SETTINGS: {
            // The helper process is an explorer.exe image, so
            // GetModuleFileNameW(nullptr, ...) would return explorer.exe and just
            // open a File Explorer window. Resolve Windhawk's own path instead.
            WCHAR path[MAX_PATH];
            if (ExpandEnvironmentStringsW(L"%ProgramFiles%\\Windhawk\\windhawk.exe",
                                          path, ARRAYSIZE(path))) {
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
