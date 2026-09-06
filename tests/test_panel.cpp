// Layout regression test for the details panel: renders DrawDetailsPanel into an
// offscreen DC and measures, from the pixels, that
//
//   * all four values (download / upload speed, download / upload total) start on
//     the same left text edge,
//   * the upload total ends with the same padding from the right edge as the left
//     column has from the left edge,
//   * nothing is drawn outside the panel and no row touches another,
//   * the panel is at least as tall as its content needs at every DPI scale.
//
// The point is that "tidy" becomes a measured assertion instead of an eyeball
// judgement on a screenshot.
//
//   test_panel.exe        -> panel.png + measurements, non-zero exit on failure

#include <windows.h>

#include <cstdarg>
#include <cstdio>
#include <map>
#include <string>
#include <vector>

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

static void Defaults() {
    g_intSettings[L"Appearance.PanelWidth"] = 220;
    g_intSettings[L"Appearance.PanelHeight"] = 52;
    g_intSettings[L"Appearance.FontSize"] = 13;
    g_intSettings[L"Appearance.BoldText"] = 1;
    g_intSettings[L"Appearance.ArrowScale"] = 120;
    g_intSettings[L"Appearance.ShowColumnLabels"] = 1;
    g_intSettings[L"Appearance.DividerPos"] = 50;
    g_intSettings[L"Appearance.DividerOpacity"] = 46;
    g_intSettings[L"Appearance.DetailsWidth"] = 240;
    g_intSettings[L"Appearance.DetailsHeight"] = 276;
    g_intSettings[L"Appearance.OffsetX"] = 12;
    g_intSettings[L"Appearance.DpiScaling"] = 0;  // fixed 1.0, deterministic
    g_intSettings[L"Appearance.AutoTheme"] = 0;   // force the manual colour
    g_intSettings[L"Appearance.ColorArrows"] = 1;
    g_intSettings[L"Appearance.BgOpacity"] = 0;
    g_intSettings[L"Network.ExcludeVirtual"] = 1;
    g_intSettings[L"Network.UpdateInterval"] = 1000;
    g_intSettings[L"Network.BinaryUnits"] = 1;
    g_intSettings[L"Behavior.StartEnabled"] = 1;
    g_strSettings[L"Appearance.LayoutMode"] = L"full";
    g_strSettings[L"Appearance.TextColor"] = L"0xFFFFFF";
    g_strSettings[L"Appearance.TextWeight"] = L"bold";
    g_strSettings[L"Appearance.ArrowStyle"] = L"rounded";
    g_strSettings[L"Network.InterfaceMode"] = L"auto";
    g_strSettings[L"Network.SpeedUnit"] = L"auto";
    g_strSettings[L"Traffic.CounterMode"] = L"session";
    g_strSettings[L"Traffic.ResetCounters"] = L"none";
    LoadSettings();
}

// One horizontal band of ink, with the extents of its neutral (text) pixels and
// of its saturated (arrow) pixels measured separately.
struct Band {
    int top = 0;
    int bottom = 0;
    int textLeft = -1;
    int textRight = -1;
    int arrowLeft = -1;
    int arrowRight = -1;
};

static bool SavePng(Bitmap& bitmap, const wchar_t* path) {
    UINT num = 0, size = 0;
    GetImageEncodersSize(&num, &size);
    if (!size) return false;
    std::vector<BYTE> buffer(size);
    ImageCodecInfo* info = (ImageCodecInfo*)buffer.data();
    GetImageEncoders(num, size, info);
    for (UINT i = 0; i < num; i++) {
        if (wcscmp(info[i].MimeType, L"image/png") == 0) {
            return bitmap.Save(path, &info[i].Clsid, nullptr) == Ok;
        }
    }
    return false;
}

int main() {
    setvbuf(stdout, nullptr, _IONBF, 0);
    SetProcessDPIAware();

    GdiplusStartupInput input;
    ULONG_PTR token = 0;
    if (GdiplusStartup(&token, &input, nullptr) != Ok) {
        printf("GdiplusStartup failed\n");
        return 1;
    }

    Defaults();

    {
        std::lock_guard<std::mutex> guard(g_snapshotMutex);
        g_snapshot.haveInterface = true;
        g_snapshot.connected = true;
        g_snapshot.ifName = L"Ethernet";
        g_snapshot.ifKind = L"Ethernet";
        g_snapshot.ipv4 = L"192.168.1.88";
        g_snapshot.downBytesPerSec = 12.4 * 1024 * 1024;
        g_snapshot.upBytesPerSec = 1.8 * 1024 * 1024;
        g_snapshot.totalDown = (unsigned long long)(4.82 * 1024 * 1024 * 1024);
        g_snapshot.totalUp = (unsigned long long)(1.17 * 1024 * 1024 * 1024);
    }

    int failures = 0;

    // --- 1. natural-height floor at every scale -----------------------------
    printf("--- panel height vs. its own content ---\n");
    const double scales[] = {1.0, 1.25, 1.5, 1.75, 2.0};
    for (double scale : scales) {
        PanelLayout layout = ComputePanelLayout(scale);
        PanelMetrics m = ComputePanelMetrics(scale);
        bool tallEnough = m.height + 1 >= (int)layout.naturalHeight;
        bool buttonInside = m.resetButton.top > 0 &&
                            m.resetButton.bottom <= m.height &&
                            m.resetButton.left > 0 &&
                            m.resetButton.right < m.width;
        printf("  %3.0f%%: panel=%dx%d needs=%.0f button=(%ld,%ld)-(%ld,%ld) %s%s\n",
               scale * 100, m.width, m.height, layout.naturalHeight,
               m.resetButton.left, m.resetButton.top, m.resetButton.right,
               m.resetButton.bottom, tallEnough ? "" : "TOO SHORT ",
               buttonInside ? "" : "BUTTON OUTSIDE");
        if (!tallEnough || !buttonInside) {
            failures++;
        }
    }

    // --- 2. render and measure the real pixels ------------------------------
    PanelMetrics metrics = ComputePanelMetrics(1.0);
    const int w = metrics.width;
    const int h = metrics.height;

    HDC screen = GetDC(nullptr);
    HDC memDC = CreateCompatibleDC(screen);
    HBITMAP bitmap = CreateCompatibleBitmap(screen, w, h);
    HBITMAP oldBitmap = (HBITMAP)SelectObject(memDC, bitmap);
    DrawDetailsPanel(memDC, w, h, nullptr);
    SelectObject(memDC, oldBitmap);

    Bitmap* image = Bitmap::FromHBITMAP(bitmap, nullptr);
    if (!image) {
        printf("FromHBITMAP failed\n");
        return 1;
    }
    SavePng(*image, L"panel.png");

    BitmapData data{};
    Rect lock(0, 0, w, h);
    if (image->LockBits(&lock, ImageLockModeRead, PixelFormat32bppARGB, &data) !=
        Ok) {
        printf("LockBits failed\n");
        return 1;
    }

    auto pixel = [&](int x, int y) -> BYTE* {
        return (BYTE*)data.Scan0 + y * data.Stride + x * 4;  // BGRA
    };

    // Bands of ink. Text is neutral (low saturation, bright); arrows are
    // saturated. The 1px shadow under the text is dark, so a brightness floor
    // separates glyphs from it.
    std::vector<Band> bands;
    Band current{};
    bool inBand = false;
    for (int y = 0; y < h; y++) {
        Band row{};
        row.top = row.bottom = y;
        for (int x = 0; x < w; x++) {
            BYTE* p = pixel(x, y);
            int mx = std::max({(int)p[0], (int)p[1], (int)p[2]});
            int mn = std::min({(int)p[0], (int)p[1], (int)p[2]});
            bool neutral = mx > 150 && (mx - mn) <= 40;
            bool saturated = mx > 110 && (mx - mn) > 45;
            if (neutral) {
                if (row.textLeft < 0) row.textLeft = x;
                row.textRight = x;
            } else if (saturated) {
                if (row.arrowLeft < 0) row.arrowLeft = x;
                row.arrowRight = x;
            }
        }
        bool ink = row.textLeft >= 0 || row.arrowLeft >= 0;
        if (ink && !inBand) {
            current = row;
            inBand = true;
        } else if (ink) {
            current.bottom = y;
            if (row.textLeft >= 0 &&
                (current.textLeft < 0 || row.textLeft < current.textLeft)) {
                current.textLeft = row.textLeft;
            }
            if (row.textRight > current.textRight) current.textRight = row.textRight;
            if (row.arrowLeft >= 0 &&
                (current.arrowLeft < 0 || row.arrowLeft < current.arrowLeft)) {
                current.arrowLeft = row.arrowLeft;
            }
            if (row.arrowRight > current.arrowRight) {
                current.arrowRight = row.arrowRight;
            }
        } else if (inBand) {
            bands.push_back(current);
            inBand = false;
        }
    }
    if (inBand) bands.push_back(current);
    image->UnlockBits(&data);

    printf("\n--- ink bands (y range, text extents, arrow extents) ---\n");
    for (size_t i = 0; i < bands.size(); i++) {
        const Band& b = bands[i];
        printf("  [%02zu] y=%3d-%3d  text=%4d..%-4d arrow=%4d..%-4d\n", i, b.top,
               b.bottom, b.textLeft, b.textRight, b.arrowLeft, b.arrowRight);
    }

    PanelLayout layout = ComputePanelLayout(1.0);
    const int margin = (int)layout.margin;
    const int expectedTextX = (int)(layout.margin + layout.arrow + layout.arrowGap);

    // Bands whose text starts at the indented column are the value blocks (the
    // label and its value merge into one band because they are adjacent rows).
    // A 2px tolerance absorbs glyph side bearing: "Download" starts with a 'D',
    // "4.82 GB" with a '4', and the two have different left bearings.
    printf("\n--- alignment ---\n");
    std::vector<int> indented;
    for (const Band& b : bands) {
        if (b.textLeft >= expectedTextX - 3 && b.textLeft <= expectedTextX + 3) {
            indented.push_back(b.textLeft);
        }
    }
    printf("  expected value column x=%d, bands on it: %zu\n", expectedTextX,
           indented.size());
    int spread = 0;
    for (int x : indented) {
        int delta = abs(x - indented.front());
        if (delta > spread) spread = delta;
    }
    printf("  left-edge spread across those bands: %dpx\n", spread);
    if (spread > 2) {
        printf("  FAIL: value columns do not share a left edge\n");
        failures++;
    }
    // Download block, upload block, and the totals row all sit on this column.
    if (indented.size() < 3) {
        printf("  FAIL: expected at least 3 bands on the value column\n");
        failures++;
    }

    // Right padding: with the upload pair right-aligned, the widest row should end
    // one left-margin away from the right edge.
    int rightmost = 0;
    for (const Band& b : bands) {
        int edge = b.textRight > b.arrowRight ? b.textRight : b.arrowRight;
        if (edge > rightmost) rightmost = edge;
    }
    int rightPadding = w - 1 - rightmost;
    printf("  left margin=%d, right padding of the widest row=%d (diff %d)\n",
           margin, rightPadding, abs(rightPadding - margin));
    if (abs(rightPadding - margin) > 3) {
        printf("  FAIL: right padding does not match the left margin\n");
        failures++;
    }

    // Nothing may touch the panel edges, and every arrow must sit on the same
    // left rail (glyph ink, not just the box).
    printf("\n--- bounds and separation ---\n");
    int minLeft = w, maxRight = 0, minTop = h, maxBottom = 0;
    std::vector<int> arrowLefts;
    for (const Band& b : bands) {
        int left = b.textLeft >= 0 ? b.textLeft : b.arrowLeft;
        if (b.arrowLeft >= 0 && b.arrowLeft < left) left = b.arrowLeft;
        int right = b.textRight > b.arrowRight ? b.textRight : b.arrowRight;
        if (left < minLeft) minLeft = left;
        if (right > maxRight) maxRight = right;
        if (b.top < minTop) minTop = b.top;
        if (b.bottom > maxBottom) maxBottom = b.bottom;
        if (b.arrowLeft >= 0 && b.arrowLeft < (int)(layout.margin + layout.arrow)) {
            arrowLefts.push_back(b.arrowLeft);
        }
    }
    printf("  content box: x=%d..%d y=%d..%d in a %dx%d panel\n", minLeft, maxRight,
           minTop, maxBottom, w, h);
    if (minLeft < 2 || maxRight > w - 3 || minTop < 2 || maxBottom > h - 3) {
        printf("  FAIL: content touches or overflows the panel edge\n");
        failures++;
    }

    int arrowSpread = 0;
    for (int x : arrowLefts) {
        int delta = abs(x - arrowLefts.front());
        if (delta > arrowSpread) arrowSpread = delta;
    }
    printf("  left-rail arrows: %zu, ink left-edge spread=%dpx\n", arrowLefts.size(),
           arrowSpread);
    if (arrowSpread > 2) {
        printf("  FAIL: arrows do not share one optical left rail\n");
        failures++;
    }

    SelectObject(memDC, oldBitmap);
    delete image;
    DeleteObject(bitmap);
    DeleteDC(memDC);
    ReleaseDC(nullptr, screen);

    printf("\npanel.png written\n");
    if (failures) {
        printf("FAIL: %d check(s) failed\n", failures);
    } else {
        printf("panel layout OK\n");
    }
    fflush(stdout);
    // GdiplusShutdown crashes in the mingw static-teardown path.
    ExitProcess(failures ? 1 : 0);
}
