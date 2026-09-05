// Fit check: measures the real pixel width of the widest strings the widget can
// display and reports the minimum widget width needed so nothing is ever clipped.
//
// Two things matter and are easy to get wrong:
//   * DrawString with a default StringFormat adds ~1/6 em of padding on each side,
//     so a string that "measures" 62 px needs ~68 px of rect. GenericTypographic
//     removes that padding. Both are reported below.
//   * The full layout splits the widget into two columns, so each column only gets
//     (width - 2*padding - columnGap)/2 pixels.

#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
#include <cmath>
#include <cstdio>
#include <string>
#include <vector>

using namespace Gdiplus;

int main() {
    setvbuf(stdout, nullptr, _IONBF, 0);
    GdiplusStartupInput input;
    ULONG_PTR token = 0;
    GdiplusStartup(&token, &input, nullptr);

    Bitmap bitmap(600, 60, PixelFormat32bppARGB);
    Graphics graphics(&bitmap);
    graphics.SetTextRenderingHint(TextRenderingHintAntiAliasGridFit);
    FontFamily family(L"Segoe UI", nullptr);

    // Worst realistic cases: 3 significant digits + widest unit, bits mode included.
    const wchar_t* speeds[] = {L"999 KB/s", L"12.4 MB/s", L"1.02 GB/s", L"999 Mbps",
                               L"9.99 Gbps"};
    const wchar_t* totals[] = {L"999 KB", L"4.82 GB", L"99.9 GB", L"1.02 TB"};

    StringFormat plain;  // what the mod used to use
    plain.SetFormatFlags(StringFormatFlagsNoWrap);
    StringFormat typographic(StringFormat::GenericTypographic());
    typographic.SetFormatFlags(typographic.GetFormatFlags() |
                               StringFormatFlagsNoWrap);

    auto widest = [&](const wchar_t** items, int count, Font& font,
                      StringFormat& format, const wchar_t** which) {
        REAL best = 0;
        for (int i = 0; i < count; i++) {
            RectF bounds;
            graphics.MeasureString(items[i], -1, &font, RectF(0, 0, 1000, 100),
                                   &format, &bounds);
            if (bounds.Width > best) {
                best = bounds.Width;
                if (which) {
                    *which = items[i];
                }
            }
        }
        return best;
    };

    const REAL padding = 9.0f;
    const REAL columnGap = 8.0f;
    const REAL gap = 6.0f;

    printf("full layout: two columns, each column = (W - 2*%.0f - %.0f) / 2\n\n",
           padding, columnGap);
    printf("%-5s %-22s %-22s %s\n", "font", "plain format (old)",
           "typographic (new)", "widest string");
    printf("%s\n", std::string(78, '-').c_str());

    for (int fontSize = 10; fontSize <= 15; fontSize++) {
        Font fontValue(&family, (REAL)fontSize, FontStyleBold, UnitPixel);
        Font fontTotal(&family, fontSize * 0.94f, FontStyleBold, UnitPixel);
        REAL arrow = fontSize * 1.45f;

        const wchar_t* worstSpeed = nullptr;
        REAL speedPlain = widest(speeds, 5, fontValue, plain, &worstSpeed);
        REAL totalPlain = widest(totals, 4, fontTotal, plain, nullptr);
        REAL speedTypo = widest(speeds, 5, fontValue, typographic, nullptr);
        REAL totalTypo = widest(totals, 4, fontTotal, typographic, nullptr);

        auto needed = [&](REAL speedW, REAL totalW) {
            return padding * 2 + columnGap + (arrow + gap + speedW) +
                   (arrow + gap + totalW);
        };

        printf("%-5d needW=%-4.0f col=%-4.0f  needW=%-4.0f col=%-4.0f  %ls\n",
               fontSize, ceil(needed(speedPlain, totalPlain)),
               ceil(arrow + gap + speedPlain),
               ceil(needed(speedTypo, totalTypo)), ceil(arrow + gap + speedTypo),
               worstSpeed ? worstSpeed : L"");
    }

    printf("\nverdict for the shipped default (width=200, font=13):\n");
    {
        int fontSize = 13;
        Font fontValue(&family, (REAL)fontSize, FontStyleBold, UnitPixel);
        REAL arrow = fontSize * 1.45f;
        REAL column = (200.0f - padding * 2 - columnGap) / 2.0f;
        REAL avail = column - arrow - gap;
        for (int i = 0; i < 5; i++) {
            RectF plainBounds, typoBounds;
            graphics.MeasureString(speeds[i], -1, &fontValue, RectF(0, 0, 1000, 100),
                                   &plain, &plainBounds);
            graphics.MeasureString(speeds[i], -1, &fontValue, RectF(0, 0, 1000, 100),
                                   &typographic, &typoBounds);
            printf("  %-10ls avail=%.1f  plain=%.1f %-7s  typo=%.1f %s\n", speeds[i],
                   avail, plainBounds.Width,
                   plainBounds.Width > avail ? "CLIPS" : "ok", typoBounds.Width,
                   typoBounds.Width > avail ? "CLIPS" : "ok");
        }
    }

    printf("\nheight check (two rows must fit the widget height):\n");
    for (int fontSize = 11; fontSize <= 15; fontSize++) {
        Font font(&family, (REAL)fontSize, FontStyleBold, UnitPixel);
        RectF bounds;
        graphics.MeasureString(L"12.4 MB/s", -1, &font, RectF(0, 0, 1000, 100),
                              &typographic, &bounds);
        printf("  font=%d lineHeight=%.1f -> widget height >= %.0f px\n", fontSize,
               bounds.Height, ceil(bounds.Height / 0.42f));
    }

    printf("\nnote: GdiplusShutdown is skipped on purpose - the mingw runtime\n"
           "      crashes in GDI+ static teardown and the measurements above are\n"
           "      already complete.\n");
    fflush(stdout);
    ExitProcess(0);
}
