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
            const int mouseX = GET_X_LPARAM(lParam);
            ModSettings settings = GetSettings();

            // --- divider drag ---------------------------------------------
            if (g_dividerDragging.load()) {
                RECT client{};
                GetClientRect(hwnd, &client);
                const double scale = GetScaleForWindow(hwnd);
                // The offset is stored in logical px so it stays correct if the
                // monitor scaling changes later.
                int base = (int)DividerBaseX(hwnd, client.right);
                int offset = (int)((mouseX - base) / (scale > 0 ? scale : 1.0));
                g_dividerOffset = offset;
                InvalidateRect(hwnd, nullptr, FALSE);
                return 0;
            }

            const int state = DividerHover(hwnd, mouseX, settings) ? 3 : 1;
            if (g_hoverState.load() != state) {
                g_hoverState = state;
                InvalidateRect(hwnd, nullptr, FALSE);
            }
            TRACKMOUSEEVENT track{sizeof(TRACKMOUSEEVENT), TME_LEAVE, hwnd, 0};
            TrackMouseEvent(&track);
            return 0;
        }

        case WM_SETCURSOR:
            // A west-east cursor over the grab zone is the only affordance that
            // says "this line can be dragged".
            if (LOWORD(lParam) == HTCLIENT &&
                (g_dividerDragging.load() || g_hoverState.load() == 3)) {
                SetCursor(LoadCursorW(nullptr, IDC_SIZEWE));
                return TRUE;
            }
            break;

        case WM_MOUSELEAVE:
            if (g_hoverState.load() != 0 && !g_dividerDragging.load()) {
                g_hoverState = 0;
                InvalidateRect(hwnd, nullptr, FALSE);
            }
            return 0;

        case WM_LBUTTONDOWN: {
            ModSettings settings = GetSettings();
            if (DividerHover(hwnd, GET_X_LPARAM(lParam), settings)) {
                g_dividerDragging = true;
                SetCapture(hwnd);
                InvalidateRect(hwnd, nullptr, FALSE);
                return 0;
            }
            break;
        }

        case WM_LBUTTONUP:
            if (g_dividerDragging.load()) {
                g_dividerDragging = false;
                ReleaseCapture();
                // Persist only on release: writing on every mouse move would hit
                // the registry dozens of times per drag.
                Wh_SetIntValue(L"DividerOffset", g_dividerOffset.load());
                LoadSettings();
                InvalidateRect(hwnd, nullptr, FALSE);
                return 0;  // a drag is not a click - don't open the panel
            }
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

    // Seed the live divider offset from storage BEFORE the first settings read,
    // so a dragged divider is already in place on the first paint.
    g_dividerOffset = Wh_GetIntValue(L"DividerOffset", 0);

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
