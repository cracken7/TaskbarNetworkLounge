// Render + leak harness for the drawing layer of Taskbar Network Lounge.
// Draws the widget and the details panel hundreds of times into an offscreen DC
// and checks that GDI/USER object counts do not grow (GDI+ / GDI leak check),
// verifies the DPI scaling math, and writes sample PNGs for visual inspection.
//
// Usage: test_render.exe [iterations]
#include <windows.h>

#include <cstdarg>
#include <cstdio>
#include <map>
#include <string>

static std::map<std::wstring, int> g_intSettings;
static std::map<std::wstring, std::wstring> g_strSettings;
static std::map<std::wstring, std::wstring> g_storage;

static void Wh_Log(const wchar_t*, ...) {}

static int Wh_GetIntSetting(const wchar_t* name, ...) {
    auto it = g_intSettings.find(name);
    return it == g_intSettings.end() ? 0 : it->second;
}

static const wchar_t* Wh_GetStringSetting(const wchar_t* name, ...) {
    auto it = g_strSettings.find(name);
    const std::wstring& value =
        it == g_strSettings.end() ? std::wstring() : it->second;
    wchar_t* copy = new wchar_t[value.size() + 1];
    wcscpy_s(copy, value.size() + 1, value.c_str());
    return copy;
}

static void Wh_FreeStringSetting(const wchar_t* value) { delete[] value; }

static size_t Wh_GetStringValue(const wchar_t* name, wchar_t* buffer,
                                size_t chars) {
    auto it = g_storage.find(name);
    if (it == g_storage.end() || it->second.size() + 1 > chars) {
        if (chars) buffer[0] = L'\0';
        return 0;
    }
    wcscpy_s(buffer, chars, it->second.c_str());
    return it->second.size();
}

static BOOL Wh_SetStringValue(const wchar_t* name, const wchar_t* value) {
    g_storage[name] = value;
    return TRUE;
}

// Mod-local int storage (used by the divider-offset persistence in p3/p6/p7).
static std::map<std::wstring, int> g_intStorage;

[[maybe_unused]] static int Wh_GetIntValue(const wchar_t* name, int defaultValue) {
    auto it = g_intStorage.find(name);
    return it == g_intStorage.end() ? defaultValue : it->second;
}

[[maybe_unused]] static BOOL Wh_SetIntValue(const wchar_t* name, int value) {
    g_intStorage[name] = value;
    return TRUE;
}

#include "../src/p2_core.inc"
#include "../src/p3_settings.inc"
#include "../src/p5_render.inc"

static void Defaults(const wchar_t* layout) {
    g_intSettings[L"Appearance.PanelWidth"] = 200;
    g_intSettings[L"Appearance.PanelHeight"] = 48;
    g_intSettings[L"Appearance.FontSize"] = 11;
    g_intSettings[L"Appearance.OffsetX"] = 12;
    g_intSettings[L"Appearance.DpiScaling"] = 1;
    g_intSettings[L"Appearance.AutoTheme"] = 1;
    g_intSettings[L"Appearance.ColorArrows"] = 1;
    g_intSettings[L"Appearance.BgOpacity"] = 0;
    g_intSettings[L"Network.ExcludeVirtual"] = 1;
    g_intSettings[L"Network.UpdateInterval"] = 1000;
    g_intSettings[L"Network.BinaryUnits"] = 1;
    g_intSettings[L"Behavior.StartEnabled"] = 1;
    g_strSettings[L"Appearance.LayoutMode"] = layout;
    g_strSettings[L"Appearance.TextColor"] = L"0xFFFFFF";
    g_strSettings[L"Network.InterfaceMode"] = L"auto";
    g_strSettings[L"Network.SpeedUnit"] = L"auto";
    g_strSettings[L"Traffic.CounterMode"] = L"session";
    g_strSettings[L"Traffic.ResetCounters"] = L"none";
    LoadSettings();
}

static UINT GuiObjects(UINT flags) {
    return GetGuiResources(GetCurrentProcess(), flags);
}

static bool SaveBitmapPng(HBITMAP bitmap, const wchar_t* path) {
    UINT num = 0, size = 0;
    GetImageEncodersSize(&num, &size);
    if (!size) return false;
    ImageCodecInfo* info = (ImageCodecInfo*)malloc(size);
    if (!info) return false;
    GetImageEncoders(num, size, info);
    CLSID clsid{};
    bool found = false;
    for (UINT i = 0; i < num; i++) {
        if (wcscmp(info[i].MimeType, L"image/png") == 0) {
            clsid = info[i].Clsid;
            found = true;
            break;
        }
    }
    free(info);
    if (!found) return false;
    Bitmap* image = Bitmap::FromHBITMAP(bitmap, nullptr);
    bool ok = image && image->Save(path, &clsid, nullptr) == Ok;
    delete image;
    return ok;
}

int main(int argc, char** argv) {
    int iterations = argc > 1 ? atoi(argv[1]) : 400;
    SetProcessDPIAware();

    GdiplusStartupInput input;
    ULONG_PTR token = 0;
    if (GdiplusStartup(&token, &input, nullptr) != Ok) {
        printf("GdiplusStartup failed\n");
        return 1;
    }

    Defaults(L"full");

    // Feed the drawing code a realistic snapshot.
    {
        std::lock_guard<std::mutex> guard(g_snapshotMutex);
        g_snapshot.downBytesPerSec = 12.4 * 1024 * 1024;
        g_snapshot.upBytesPerSec = 1.8 * 1024 * 1024;
        g_snapshot.totalDown = (unsigned long long)(4.82 * 1024 * 1024 * 1024);
        g_snapshot.totalUp = (unsigned long long)(1.17 * 1024 * 1024 * 1024);
        g_snapshot.ifName = L"Ethernet";
        g_snapshot.ifKind = L"Ethernet";
        g_snapshot.ipv4 = L"192.168.1.88";
        g_snapshot.connected = true;
        g_snapshot.haveInterface = true;
        g_snapshot.ifCount = 1;
    }

    printf("--- DPI scaling math (ComputePanelMetrics) ---\n");
    const double scales[] = {1.0, 1.25, 1.5, 1.75, 2.0};
    for (double scale : scales) {
        PanelMetrics m = ComputePanelMetrics(scale);
        int buttonHeight = m.resetButton.bottom - m.resetButton.top;
        int buttonWidth = m.resetButton.right - m.resetButton.left;
        bool inside = m.resetButton.left > 0 && m.resetButton.right < m.width &&
                      m.resetButton.top > 0 && m.resetButton.bottom <= m.height;
        printf("  %3.0f%%: panel=%dx%d resetButton=%dx%d at (%ld,%ld) inside=%s\n",
               scale * 100, m.width, m.height, buttonWidth, buttonHeight,
               m.resetButton.left, m.resetButton.top, inside ? "yes" : "NO");
    }

    HDC screen = GetDC(nullptr);
    printf("\n--- draw loop: %d iterations of widget + panel ---\n", iterations);

    UINT gdiBefore = GuiObjects(GR_GDIOBJECTS);
    UINT userBefore = GuiObjects(GR_USEROBJECTS);
    printf("  before: gdi=%u user=%u\n", gdiBefore, userBefore);

    const wchar_t* layouts[] = {L"full", L"speeds", L"oneline"};
    for (int i = 0; i < iterations; i++) {
        Defaults(layouts[i % 3]);
        ModSettings s = GetSettings();

        int w = s.width + (i % 7) * 8;
        int h = s.height + (i % 3) * 4;

        HDC memDC = CreateCompatibleDC(screen);
        HBITMAP bitmap = CreateCompatibleBitmap(screen, w, h);
        HBITMAP old = (HBITMAP)SelectObject(memDC, bitmap);
        DrawNetworkPanel(memDC, w, h, nullptr);
        SelectObject(memDC, old);
        if (i == 0) {
            SaveBitmapPng(bitmap, L"render_widget.png");
        }
        DeleteObject(bitmap);
        DeleteDC(memDC);

        PanelMetrics metrics = ComputePanelMetrics(1.0);
        HDC panelDC = CreateCompatibleDC(screen);
        HBITMAP panelBitmap =
            CreateCompatibleBitmap(screen, metrics.width, metrics.height);
        HBITMAP oldPanel = (HBITMAP)SelectObject(panelDC, panelBitmap);
        DrawDetailsPanel(panelDC, metrics.width, metrics.height, nullptr);
        SelectObject(panelDC, oldPanel);
        if (i == 0) {
            SaveBitmapPng(panelBitmap, L"render_panel.png");
        }
        DeleteObject(panelBitmap);
        DeleteDC(panelDC);
    }

    UINT gdiAfter = GuiObjects(GR_GDIOBJECTS);
    UINT userAfter = GuiObjects(GR_USEROBJECTS);
    printf("  after : gdi=%u user=%u  (delta gdi=%+d user=%+d)\n", gdiAfter,
           userAfter, (int)gdiAfter - (int)gdiBefore,
           (int)userAfter - (int)userBefore);
    printf("  verdict: %s\n",
           ((int)gdiAfter - (int)gdiBefore) <= 2 &&
                   ((int)userAfter - (int)userBefore) <= 2
               ? "no GDI/USER leak"
               : "POSSIBLE LEAK");

    // Disconnected state must not divide by zero or draw garbage.
    {
        std::lock_guard<std::mutex> guard(g_snapshotMutex);
        g_snapshot.haveInterface = false;
        g_snapshot.connected = false;
        g_snapshot.ifName = L"No Network";
        g_snapshot.downBytesPerSec = 0;
        g_snapshot.upBytesPerSec = 0;
    }
    Defaults(L"full");
    HDC memDC = CreateCompatibleDC(screen);
    HBITMAP bitmap = CreateCompatibleBitmap(screen, 200, 48);
    HBITMAP old = (HBITMAP)SelectObject(memDC, bitmap);
    DrawNetworkPanel(memDC, 200, 48, nullptr);
    SelectObject(memDC, old);
    SaveBitmapPng(bitmap, L"render_nonetwork.png");
    DeleteObject(bitmap);
    DeleteDC(memDC);
    printf("\n  disconnected state drawn without crashing\n");

    // Extreme sizes / zero size must not crash.
    for (int size : {1, 2, 8, 1000}) {
        HDC dc = CreateCompatibleDC(screen);
        HBITMAP bm = CreateCompatibleBitmap(screen, size, size);
        HBITMAP prev = (HBITMAP)SelectObject(dc, bm);
        DrawNetworkPanel(dc, size, size, nullptr);
        DrawDetailsPanel(dc, size, size, nullptr);
        SelectObject(dc, prev);
        DeleteObject(bm);
        DeleteDC(dc);
    }
    printf("  degenerate sizes (1,2,8,1000) drawn without crashing\n");

    ReleaseDC(nullptr, screen);
    GdiplusShutdown(token);
    printf("\ndone\n");
    return 0;
}
