// Render every arrow style into a labelled grid -> docs/arrow-styles.png
// Uses the REAL arrow code from the mod (tools/arrow_core.inc, extracted from
// src/p5_render.inc) so the picture can never drift from what the mod draws.
#include <windows.h>
#include <gdiplus.h>
#include <cstdio>
#include <vector>
using namespace Gdiplus;

enum class ArrowStyle {
    Solid, Chevron, Triangle, Rounded, Circle,
    Double, Triple, Outline, Thin, Fat, Head, Ring, Square, Fade, Dashed,
    Barb, Needle, Dot, Feather, Layered, Tray, Plus, None };

static const WCHAR* kFontName = L"Segoe UI";

static Color BlendColor(const Color& base, BYTE alpha) {
    return Color(alpha, base.GetRed(), base.GetGreen(), base.GetBlue());
}

static void AddRoundedRect(GraphicsPath& path, REAL x, REAL y, REAL w, REAL h,
                           REAL r) {
    REAL d = r * 2;
    if (d > w) d = w;
    if (d > h) d = h;
    path.AddArc(x, y, d, d, 180, 90);
    path.AddArc(x + w - d, y, d, d, 270, 90);
    path.AddArc(x + w - d, y + h - d, d, d, 0, 90);
    path.AddArc(x, y + h - d, d, d, 90, 90);
    path.CloseFigure();
}

#include "arrow_core.inc"

struct Entry { ArrowStyle style; const wchar_t* name; };

int wmain() {
    SetProcessDPIAware();
    ULONG_PTR token = 0;
    GdiplusStartupInput si;
    GdiplusStartup(&token, &si, nullptr);

    static const Entry entries[] = {
        {ArrowStyle::Solid, L"solid"},      {ArrowStyle::Rounded, L"rounded"},
        {ArrowStyle::Chevron, L"chevron"},  {ArrowStyle::Triangle, L"triangle"},
        {ArrowStyle::Circle, L"circle"},    {ArrowStyle::Double, L"double"},
        {ArrowStyle::Triple, L"triple"},    {ArrowStyle::Outline, L"outline"},
        {ArrowStyle::Thin, L"thin"},        {ArrowStyle::Fat, L"fat"},
        {ArrowStyle::Head, L"head"},        {ArrowStyle::Ring, L"ring"},
        {ArrowStyle::Square, L"square"},    {ArrowStyle::Fade, L"fade"},
        {ArrowStyle::Dashed, L"dashed"},    {ArrowStyle::Barb, L"barb"},
        {ArrowStyle::Needle, L"needle"},    {ArrowStyle::Dot, L"dot"},
        {ArrowStyle::Feather, L"feather"},  {ArrowStyle::Layered, L"layered"},
        {ArrowStyle::Tray, L"tray"},        {ArrowStyle::Plus, L"plus"},
    };
    const int n = sizeof(entries) / sizeof(entries[0]);
    const int cols = 4;
    const int rows = (n + cols - 1) / cols;
    const int cellW = 240, cellH = 120;
    const int headerH = 64;
    const int W = cols * cellW, H = headerH + rows * cellH;

    Bitmap canvas(W, H, PixelFormat32bppARGB);
    Graphics g(&canvas);
    g.SetSmoothingMode(SmoothingModeAntiAlias);
    g.SetTextRenderingHint(TextRenderingHintAntiAliasGridFit);

    // Dark acrylic-like background, same family as the widget.
    SolidBrush bg(Color(255, 24, 32, 44));
    g.FillRectangle(&bg, 0, 0, W, H);
    // Header
    FontFamily fam(kFontName);
    Font title(&fam, 26, FontStyleBold, UnitPixel);
    Font label(&fam, 15, FontStyleRegular, UnitPixel);
    Font labelB(&fam, 15, FontStyleBold, UnitPixel);
    StringFormat lf(StringFormat::GenericTypographic());
    lf.SetAlignment(StringAlignmentNear);
    lf.SetLineAlignment(StringAlignmentCenter);
    SolidBrush white(Color(255, 240, 244, 248));
    SolidBrush dim(Color(180, 240, 244, 248));
    g.DrawString(L"Arrow styles - download (blue, down) / upload (green, up)",
                 -1, &title, RectF(24, 0, (REAL)W - 48, (REAL)headerH),
                 &lf, &white);

    Color downColor(255, 140, 224, 255);
    Color upColor(255, 138, 238, 158);

    for (int i = 0; i < n; i++) {
        int cx0 = (i % cols) * cellW;
        int cy0 = headerH + (i / cols) * cellH;
        // cell separator
        Pen sep(Color(40, 255, 255, 255), 1);
        g.DrawLine(&sep, cx0, cy0, cx0, cy0 + cellH);
        g.DrawLine(&sep, cx0, cy0, cx0 + cellW, cy0);

        const Entry& e = entries[i];
        REAL arrowSize = 34;
        REAL midY = cy0 + cellH / 2.0f;
        // down arrow + sample number
        DrawArrow(g, downColor, cx0 + 34, midY, arrowSize, true, e.style);
        RectF dr(cx0 + 58, midY - 20, 70, 40);
        g.DrawString(L"12.4", -1, &labelB, dr, &lf, &white);
        // up arrow + sample number
        DrawArrow(g, upColor, cx0 + 138, midY, arrowSize, false, e.style);
        RectF ur(cx0 + 162, midY - 20, 70, 40);
        g.DrawString(L"3.1", -1, &labelB, ur, &lf, &white);
        // style name bottom-left
        RectF nr(cx0 + 12, cy0 + cellH - 26, (REAL)cellW - 24, 24);
        g.DrawString(e.name, -1, &label, nr, &lf, &dim);
    }

    CLSID pngClsid{};
    UINT num = 0, size = 0;
    GetImageEncodersSize(&num, &size);
    std::vector<BYTE> buf(size);
    ImageCodecInfo* infos = (ImageCodecInfo*)buf.data();
    GetImageEncoders(num, size, infos);
    for (UINT i = 0; i < num; i++) {
        if (wcscmp(infos[i].MimeType, L"image/png") == 0) {
            pngClsid = infos[i].Clsid;
            break;
        }
    }
    Status st = canvas.Save(L"C:\\Users\\Admin\\AppData\\Local\\Temp\\tnl_repo\\docs\\arrow-styles.png",
                            &pngClsid, nullptr);
    GdiplusShutdown(token);
    wprintf(L"saved=%d cells=%d\n", (int)st, n);
    return st == Ok ? 0 : 1;
}
