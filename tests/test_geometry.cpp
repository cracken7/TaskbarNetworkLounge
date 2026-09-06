// Geometry unit tests: taskbar edge classification and widget placement for all
// four taskbar positions, several DPI scales and a two-monitor layout, using
// synthetic rectangles (no real taskbar needed).
//
// Usage: test_geometry.exe
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

static size_t Wh_GetStringValue(const wchar_t*, wchar_t* buffer, size_t chars) {
    if (chars) buffer[0] = L'\0';
    return 0;
}

static BOOL Wh_SetStringValue(const wchar_t*, const wchar_t*) { return TRUE; }

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
#include "../src/p4_network.inc"
#include "../src/p5_render.inc"
#include "../src/p6_window.inc"

static int g_failures = 0;

static void Check(bool condition, const char* what) {
    printf("  %-62s %s\n", what, condition ? "OK" : "FAILED");
    if (!condition) {
        g_failures++;
    }
}

static const char* EdgeName(TaskbarEdge edge) {
    switch (edge) {
        case TaskbarEdge::Top: return "Top";
        case TaskbarEdge::Bottom: return "Bottom";
        case TaskbarEdge::Left: return "Left";
        case TaskbarEdge::Right: return "Right";
    }
    return "?";
}

int main() {
    printf("--- taskbar edge classification ---\n");
    RECT screen{0, 0, 1920, 1080};

    struct EdgeCase {
        const char* label;
        RECT taskbar;
        TaskbarEdge expected;
    };
    const EdgeCase edges[] = {
        {"bottom 1920x48", {0, 1032, 1920, 1080}, TaskbarEdge::Bottom},
        {"bottom 1920x68 (win11 large)", {0, 1012, 1920, 1080}, TaskbarEdge::Bottom},
        {"top 1920x48", {0, 0, 1920, 48}, TaskbarEdge::Top},
        {"left 62x1080", {0, 0, 62, 1080}, TaskbarEdge::Left},
        {"right 62x1080", {1858, 0, 1920, 1080}, TaskbarEdge::Right},
    };
    for (const EdgeCase& test : edges) {
        TaskbarEdge actual = ClassifyTaskbarEdge(test.taskbar, screen);
        char message[160];
        _snprintf_s(message, sizeof(message), _TRUNCATE, "%s -> %s (want %s)",
                    test.label, EdgeName(actual), EdgeName(test.expected));
        Check(actual == test.expected, message);
    }

    printf("\n--- widget placement, 200x48 widget, offset 12,0 ---\n");
    struct PlaceCase {
        const char* label;
        RECT taskbar;
        TaskbarEdge edge;
        int expectedX;
        int expectedY;
    };
    const PlaceCase places[] = {
        // Bottom bar 1012..1080 (68 tall): centered vertically -> 1012+(68-48)/2
        {"bottom", {0, 1012, 1920, 1080}, TaskbarEdge::Bottom, 12, 1022},
        // Top bar 0..48: 0+(48-48)/2 = 0
        {"top", {0, 0, 1920, 48}, TaskbarEdge::Top, 12, 0},
        // Left bar 0..62 wide: centered horizontally -> (62-200)/2 = -69, +12
        {"left", {0, 0, 62, 1080}, TaskbarEdge::Left, -57, 0},
        // Right bar 1858..1920: 1858+(62-200)/2 = 1789, +12
        {"right", {1858, 0, 1920, 1080}, TaskbarEdge::Right, 1801, 0},
    };
    for (const PlaceCase& test : places) {
        int x = 0, y = 0;
        ComputeWidgetOrigin(test.taskbar, test.edge, 200, 48, 12, 0, &x, &y);
        char message[160];
        _snprintf_s(message, sizeof(message), _TRUNCATE,
                    "%-6s -> (%d,%d) want (%d,%d)", test.label, x, y,
                    test.expectedX, test.expectedY);
        Check(x == test.expectedX && y == test.expectedY, message);
    }

    printf("\n--- second monitor (offset desktop 1920..3840) ---\n");
    RECT screen2{1920, 0, 3840, 1080};
    RECT bar2{1920, 1032, 3840, 1080};
    TaskbarEdge edge2 = ClassifyTaskbarEdge(bar2, screen2);
    int x2 = 0, y2 = 0;
    ComputeWidgetOrigin(bar2, edge2, 200, 48, 12, 0, &x2, &y2);
    char message[160];
    _snprintf_s(message, sizeof(message), _TRUNCATE,
                "secondary bottom bar -> edge=%s pos=(%d,%d)", EdgeName(edge2),
                x2, y2);
    Check(edge2 == TaskbarEdge::Bottom && x2 == 1932 && y2 == 1032, message);

    printf("\n--- DPI-scaled placement (offsets and size scale together) ---\n");
    const double scales[] = {1.0, 1.25, 1.5, 1.75, 2.0};
    for (double scale : scales) {
        // A 68-logical-pixel Win11 taskbar at this scale.
        int barHeight = (int)(68 * scale);
        RECT bar{0, 1080 - barHeight, 1920, 1080};
        int w = (int)(200 * scale);
        int h = (int)(48 * scale);
        int offset = (int)(12 * scale);
        int x = 0, y = 0;
        ComputeWidgetOrigin(bar, TaskbarEdge::Bottom, w, h, offset, 0, &x, &y);
        int expectedY = bar.top + (barHeight - h) / 2;
        _snprintf_s(message, sizeof(message), _TRUNCATE,
                    "%3.0f%%: widget %dx%d at (%d,%d), inside bar=%s", scale * 100,
                    w, h, x, y,
                    (y >= bar.top && y + h <= bar.bottom) ? "yes" : "NO");
        Check(x == offset && y == expectedY && y >= bar.top &&
                  y + h <= bar.bottom,
              message);
    }

    printf("\n--- panel metrics stay inside the panel at every scale ---\n");
    for (double scale : scales) {
        PanelMetrics m = ComputePanelMetrics(scale);
        bool ok = m.resetButton.left > 0 && m.resetButton.right < m.width &&
                  m.resetButton.top > 0 && m.resetButton.bottom <= m.height &&
                  (m.resetButton.right - m.resetButton.left) > 40 * scale;
        _snprintf_s(message, sizeof(message), _TRUNCATE,
                    "%3.0f%%: panel %dx%d button (%ld,%ld)-(%ld,%ld)", scale * 100,
                    m.width, m.height, m.resetButton.left, m.resetButton.top,
                    m.resetButton.right, m.resetButton.bottom);
        Check(ok, message);
    }

    printf("\n%s (%d failure%s)\n", g_failures ? "FAILURES" : "all geometry tests passed",
           g_failures, g_failures == 1 ? "" : "s");
    return g_failures ? 1 : 0;
}
