// Contact-sheet test for the widget's appearance settings: renders the real
// DrawNetworkPanel() with every arrow style x every text weight into one PNG and
// measures the ink in each cell. A missing font family or a broken path draws
// nothing in GDI+ without raising an error, so the ink check is the actual
// assertion; the PNG is for eyeballing the styles side by side.
//
//   test_styles.exe   ->  styles.png, exits non-zero if any cell is blank

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

#include "../src/p2_core.inc"
#include "../src/p3_settings.inc"
#include "../src/p5_render.inc"

static const wchar_t* kArrowStyles[] = {L"solid", L"rounded", L"chevron",
                                        L"triangle", L"circle"};
static const wchar_t* kWeights[] = {L"bold", L"black", L"semibold", L"regular"};

static const int kCellW = 220;
static const int kCellH = 52;
static const int kLabelW = 84;
static const int kBgGrey = 96;  // stands in for the taskbar acrylic

static void Defaults() {
    g_intSettings[L"Appearance.PanelWidth"] = kCellW;
    g_intSettings[L"Appearance.PanelHeight"] = kCellH;
    g_intSettings[L"Appearance.FontSize"] = 13;
    g_intSettings[L"Appearance.BoldText"] = 1;
    g_intSettings[L"Appearance.ArrowScale"] = 145;
    g_intSettings[L"Appearance.ShowColumnLabels"] = 1;
    g_intSettings[L"Appearance.DividerPos"] = 55;
    g_intSettings[L"Appearance.DividerOpacity"] = 46;
    g_intSettings[L"Appearance.DetailsWidth"] = 240;
    g_intSettings[L"Appearance.DetailsHeight"] = 252;
    g_intSettings[L"Appearance.OffsetX"] = 12;
    g_intSettings[L"Appearance.DpiScaling"] = 0;  // fixed 1.0 for a stable sheet
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
    g_strSettings[L"Appearance.ArrowStyle"] = L"solid";
    g_strSettings[L"Network.InterfaceMode"] = L"auto";
    g_strSettings[L"Network.SpeedUnit"] = L"auto";
    g_strSettings[L"Traffic.CounterMode"] = L"session";
    g_strSettings[L"Traffic.ResetCounters"] = L"none";
    LoadSettings();
}

static bool SavePng(Bitmap& bitmap, const wchar_t* path) {
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
    return found && bitmap.Save(path, &clsid, nullptr) == Ok;
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

    // Realistic worst-case data so the strings are as wide as they ever get.
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

    const int cols = (int)(sizeof(kArrowStyles) / sizeof(kArrowStyles[0]));
    const int rows = (int)(sizeof(kWeights) / sizeof(kWeights[0]));

    HDC screen = GetDC(nullptr);
    HDC sheetDC = CreateCompatibleDC(screen);
    HBITMAP sheetBitmap =
        CreateCompatibleBitmap(screen, kLabelW + kCellW * cols, kCellH * rows);
    HBITMAP oldSheet = (HBITMAP)SelectObject(sheetDC, sheetBitmap);

    RECT all{0, 0, kLabelW + kCellW * cols, kCellH * rows};
    HBRUSH grey = CreateSolidBrush(RGB(kBgGrey, kBgGrey, kBgGrey));
    FillRect(sheetDC, &all, grey);

    printf("ink coverage per cell (blank cell = broken style)\n\n");
    printf("%-10s", "weight");
    for (int c = 0; c < cols; c++) {
        printf("%-11ls", kArrowStyles[c]);
    }
    printf("\n%s\n", std::string(10 + 11 * cols, '-').c_str());

    int failures = 0;

    for (int r = 0; r < rows; r++) {
        printf("%-10ls", kWeights[r]);
        for (int c = 0; c < cols; c++) {
            g_strSettings[L"Appearance.TextWeight"] = kWeights[r];
            g_strSettings[L"Appearance.ArrowStyle"] = kArrowStyles[c];
            LoadSettings();

            HDC memDC = CreateCompatibleDC(screen);
            HBITMAP bitmap = CreateCompatibleBitmap(screen, kCellW, kCellH);
            HBITMAP oldBitmap = (HBITMAP)SelectObject(memDC, bitmap);

            // DrawNetworkPanel starts with Clear(transparent), which lands as
            // black on an opaque DIB, so the cell background IS black here - the
            // same thing the layered window shows as acrylic. Ink is therefore
            // anything brighter than black, not anything different from grey.
            RECT cell{0, 0, kCellW, kCellH};
            FillRect(memDC, &cell, grey);
            DrawNetworkPanel(memDC, kCellW, kCellH, nullptr);

            // Count pixels that differ from the flat background.
            Bitmap* probe = Bitmap::FromHBITMAP(bitmap, nullptr);
            int ink = 0;
            if (probe) {
                BitmapData data{};
                Rect lock(0, 0, kCellW, kCellH);
                if (probe->LockBits(&lock, ImageLockModeRead, PixelFormat32bppARGB,
                                    &data) == Ok) {
                    for (int y = 0; y < kCellH; y++) {
                        BYTE* row = (BYTE*)data.Scan0 + y * data.Stride;
                        for (int x = 0; x < kCellW; x++) {
                            BYTE* p = row + x * 4;
                            int lum = (p[0] + p[1] + p[2]) / 3;
                            if (lum > 40) {
                                ink++;
                            }
                        }
                    }
                    probe->UnlockBits(&data);
                }
                delete probe;
            }

            double pct = 100.0 * ink / (kCellW * kCellH);
            printf("%6.2f%%    ", pct);
            if (pct < 1.0) {
                failures++;
            }

            BitBlt(sheetDC, kLabelW + c * kCellW, r * kCellH, kCellW, kCellH, memDC,
                   0, 0, SRCCOPY);

            SelectObject(memDC, oldBitmap);
            DeleteObject(bitmap);
            DeleteDC(memDC);
        }
        printf("\n");
    }

    // Row labels last, so they sit on top of the grey.
    SetBkMode(sheetDC, TRANSPARENT);
    SetTextColor(sheetDC, RGB(255, 255, 255));
    HFONT font = CreateFontW(-13, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                             DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                             CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                             DEFAULT_PITCH, L"Segoe UI");
    HFONT oldFont = (HFONT)SelectObject(sheetDC, font);
    for (int r = 0; r < rows; r++) {
        TextOutW(sheetDC, 6, r * kCellH + kCellH / 2 - 8, kWeights[r],
                 (int)wcslen(kWeights[r]));
    }
    SelectObject(sheetDC, oldFont);
    DeleteObject(font);

    Bitmap* sheet = Bitmap::FromHBITMAP(sheetBitmap, nullptr);
    bool saved = sheet && SavePng(*sheet, L"styles.png");
    delete sheet;

    SelectObject(sheetDC, oldSheet);
    DeleteObject(sheetBitmap);
    DeleteDC(sheetDC);
    DeleteObject(grey);
    ReleaseDC(nullptr, screen);

    printf("\ncontact sheet: %s\n", saved ? "styles.png" : "SAVE FAILED");
    if (failures) {
        printf("FAIL: %d of %d cells drew nothing\n", failures, rows * cols);
    } else {
        printf("all %d combinations drew content\n", rows * cols);
    }

    fflush(stdout);
    // GdiplusShutdown crashes in the mingw static-teardown path; work is done.
    ExitProcess(failures ? 1 : 0);
}
