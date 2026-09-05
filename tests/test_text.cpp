// Objective text-sharpness comparison: renders the same strings with several
// GDI+ TextRenderingHints, font sizes and weights onto an acrylic-like grey, then
// measures how crisp each variant is. Used to pick the widget's text settings
// instead of guessing from screenshots.
//
//   crisp%   share of glyph pixels at >=92% of full intensity (mushy text has few)
//   edge     mean gradient magnitude along glyph borders (higher = sharper)
//   fringe   max |R-B| on glyph pixels (>8 means visible colour fringing)

#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
#include <cmath>
#include <cstdio>
#include <string>
#include <vector>

using namespace Gdiplus;

struct Variant {
    const char* name;
    TextRenderingHint hint;
    REAL size;
    INT style;
    bool shadow;
};

struct Metrics {
    double crispPct;
    double edge;
    int fringe;
};

// Background luminance close to what the taskbar acrylic produces.
static const BYTE kBg = 96;

static Metrics Measure(BitmapData& data, int w, int h) {
    auto at = [&](int x, int y) -> BYTE* {
        return (BYTE*)data.Scan0 + y * data.Stride + x * 4;  // BGRA
    };

    int glyph = 0, crisp = 0, fringe = 0;
    double edgeSum = 0;
    int edgeCount = 0;

    for (int y = 1; y < h - 1; y++) {
        for (int x = 1; x < w - 1; x++) {
            BYTE* p = at(x, y);
            int lum = (p[0] + p[1] + p[2]) / 3;
            if (lum <= kBg + 6) {
                continue;  // background
            }
            glyph++;
            if (lum >= (int)(255 * 0.92)) {
                crisp++;
            }
            int diff = abs((int)p[2] - (int)p[0]);
            if (diff > fringe) {
                fringe = diff;
            }

            BYTE* left = at(x - 1, y);
            BYTE* right = at(x + 1, y);
            BYTE* up = at(x, y - 1);
            BYTE* down = at(x, y + 1);
            int lx = abs((left[0] + left[1] + left[2]) / 3 -
                         (right[0] + right[1] + right[2]) / 3);
            int ly = abs((up[0] + up[1] + up[2]) / 3 -
                         (down[0] + down[1] + down[2]) / 3);
            edgeSum += sqrt((double)(lx * lx + ly * ly));
            edgeCount++;
        }
    }

    Metrics m{};
    m.crispPct = glyph ? 100.0 * crisp / glyph : 0.0;
    m.edge = edgeCount ? edgeSum / edgeCount : 0.0;
    m.fringe = fringe;
    return m;
}

int main() {
    setvbuf(stdout, nullptr, _IONBF, 0);  // keep output if we crash
    GdiplusStartupInput input;
    ULONG_PTR token = 0;
    Status startup = GdiplusStartup(&token, &input, nullptr);
    printf("gdiplus startup=%d\n", (int)startup);

    const wchar_t* samples[] = {L"12.4 MB/s", L"4.82 GB", L"850 KB/s", L"1.02 GB/s"};

    Variant variants[] = {
        {"AntiAlias      11 bold", TextRenderingHintAntiAlias, 11.0f, FontStyleBold, false},
        {"AntiAliasGrid  11 bold", TextRenderingHintAntiAliasGridFit, 11.0f, FontStyleBold, false},
        {"ClearTypeGrid  11 bold", TextRenderingHintClearTypeGridFit, 11.0f, FontStyleBold, false},
        {"SingleBitGrid  11 bold", TextRenderingHintSingleBitPerPixelGridFit, 11.0f, FontStyleBold, false},
        {"AntiAliasGrid  12 bold", TextRenderingHintAntiAliasGridFit, 12.0f, FontStyleBold, false},
        {"ClearTypeGrid  12 bold", TextRenderingHintClearTypeGridFit, 12.0f, FontStyleBold, false},
        {"AntiAliasGrid  13 bold", TextRenderingHintAntiAliasGridFit, 13.0f, FontStyleBold, false},
        {"ClearTypeGrid  13 bold", TextRenderingHintClearTypeGridFit, 13.0f, FontStyleBold, false},
        {"AntiAliasGrid  12 reg ", TextRenderingHintAntiAliasGridFit, 12.0f, FontStyleRegular, false},
        {"AAGrid 12 bold+shadow ", TextRenderingHintAntiAliasGridFit, 12.0f, FontStyleBold, true},
        {"AAGrid 13 bold+shadow ", TextRenderingHintAntiAliasGridFit, 13.0f, FontStyleBold, true},
    };

    const int cellW = 120;
    const int cellH = 26;
    const int cols = (int)(sizeof(samples) / sizeof(samples[0]));
    const int rows = (int)(sizeof(variants) / sizeof(variants[0]));

    Bitmap strip(cellW * cols, cellH * rows, PixelFormat32bppARGB);
    Graphics stripGraphics(&strip);
    SolidBrush bgBrush(Color(255, kBg, kBg, kBg));
    stripGraphics.FillRectangle(&bgBrush, 0, 0, cellW * cols, cellH * rows);

    FontFamily family(L"Segoe UI", nullptr);
    SolidBrush white(Color(255, 255, 255, 255));
    SolidBrush shadowBrush(Color(120, 0, 0, 0));

    auto drawSample = [&](Graphics& g, const Variant& v, Font& font,
                          const wchar_t* text, REAL x, REAL y) {
        if (v.shadow) {
            g.DrawString(text, -1, &font, PointF(x + 1.0f, y + 1.0f), &shadowBrush);
        }
        g.DrawString(text, -1, &font, PointF(x, y), &white);
    };

    printf("%-24s %8s %8s %8s\n", "variant", "crisp%", "edge", "fringe");
    printf("%s\n", std::string(52, '-').c_str());

    for (int r = 0; r < rows; r++) {
        const Variant& v = variants[r];

        // Metrics on an isolated bitmap so neighbouring cells cannot pollute them.
        Bitmap probe(cellW * cols, cellH, PixelFormat32bppARGB);
        Graphics probeGraphics(&probe);
        probeGraphics.FillRectangle(&bgBrush, 0, 0, cellW * cols, cellH);
        probeGraphics.SetTextRenderingHint(v.hint);
        Font font(&family, v.size, v.style, UnitPixel);
        for (int c = 0; c < cols; c++) {
            drawSample(probeGraphics, v, font, samples[c], (REAL)(c * cellW + 6),
                       5.0f);
        }

        BitmapData data{};
        Rect rect(0, 0, cellW * cols, cellH);
        probe.LockBits(&rect, ImageLockModeRead, PixelFormat32bppARGB, &data);
        Metrics m = Measure(data, cellW * cols, cellH);
        probe.UnlockBits(&data);

        printf("%-24s %7.1f%% %8.1f %8d%s\n", v.name, m.crispPct, m.edge, m.fringe,
               m.fringe > 8 ? "  <- fringing" : "");

        // Same render into the visual strip.
        stripGraphics.SetTextRenderingHint(v.hint);
        for (int c = 0; c < cols; c++) {
            drawSample(stripGraphics, v, font, samples[c], (REAL)(c * cellW + 6),
                       (REAL)(r * cellH + 5));
        }
    }

    CLSID pngClsid;
    UINT num = 0, size = 0;
    GetImageEncodersSize(&num, &size);
    std::vector<BYTE> buffer(size);
    ImageCodecInfo* codecs = (ImageCodecInfo*)buffer.data();
    GetImageEncoders(num, size, codecs);
    for (UINT i = 0; i < num; i++) {
        if (wcscmp(codecs[i].MimeType, L"image/png") == 0) {
            pngClsid = codecs[i].Clsid;
            break;
        }
    }
    strip.Save(L"text_variants.png", &pngClsid, nullptr);
    printf("\nvisual strip: text_variants.png (rows in the order above)\n");
    fflush(stdout);
    // GdiplusShutdown crashes in the mingw static-teardown path; the work is done.
    ExitProcess(0);
}
