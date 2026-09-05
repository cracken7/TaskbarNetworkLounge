// --- Theme / appearance ----------------------------------------------------
static bool IsSystemLightMode() {
    DWORD value = 0;
    DWORD size = sizeof(value);
    if (RegGetValueW(HKEY_CURRENT_USER,
                     L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                     L"SystemUsesLightTheme", RRF_RT_DWORD, nullptr, &value,
                     &size) == ERROR_SUCCESS) {
        return value != 0;
    }
    return false;
}

static DWORD GetCurrentTextColor(const ModSettings& s) {
    if (s.autoTheme) {
        return IsSystemLightMode() ? 0xFF000000 : 0xFFFFFFFF;
    }
    return s.manualTextColor;
}

static void UpdateAppearance(HWND hwnd) {
    ModSettings s = GetSettings();

    DWM_WINDOW_CORNER_PREFERENCE preference = DWMWCP_ROUND;
    DwmSetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &preference,
                          sizeof(preference));

    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    if (!user32) {
        return;
    }
    auto setCompositionAttribute = (pSetWindowCompositionAttribute)GetProcAddress(
        user32, "SetWindowCompositionAttribute");
    if (!setCompositionAttribute) {
        return;
    }

    DWORD tint;
    if (s.bgOpacity > 0) {
        DWORD base = s.autoTheme ? (IsSystemLightMode() ? 0xFFFFFF : 0x000000)
                                 : 0xFFFFFF;
        tint = ((DWORD)s.bgOpacity << 24) | base;
    } else if (s.autoTheme) {
        tint = IsSystemLightMode() ? 0x40FFFFFF : 0x40000000;
    } else {
        tint = 0x00FFFFFF;
    }

    ACCENT_POLICY policy = {ACCENT_ENABLE_ACRYLICBLURBEHIND, 0, tint, 0};
    WINDOWCOMPOSITIONATTRIBDATA data = {WCA_ACCENT_POLICY, &policy,
                                        sizeof(ACCENT_POLICY)};
    setCompositionAttribute(hwnd, &data);
}

static void AddRoundedRect(GraphicsPath& path,
                           REAL x,
                           REAL y,
                           REAL w,
                           REAL h,
                           REAL r) {
    REAL d = r * 2;
    if (d > w) {
        d = w;
    }
    if (d > h) {
        d = h;
    }
    path.AddArc(x, y, d, d, 180, 90);
    path.AddArc(x + w - d, y, d, d, 270, 90);
    path.AddArc(x + w - d, y + h - d, d, d, 0, 90);
    path.AddArc(x, y + h - d, d, d, 90, 90);
    path.CloseFigure();
}

// DPI of the monitor the widget lives on; used to scale everything so the widget
// looks identical at 100% / 125% / 150% / 175% / 200%.
static double GetScaleForWindow(HWND hwnd) {
    ModSettings s = GetSettings();
    if (!s.dpiScaling) {
        return 1.0;
    }
    UINT dpi = 0;
    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    if (user32) {
        using GetDpiForWindow_t = UINT(WINAPI*)(HWND);
        auto getDpiForWindow =
            (GetDpiForWindow_t)GetProcAddress(user32, "GetDpiForWindow");
        if (getDpiForWindow && hwnd) {
            dpi = getDpiForWindow(hwnd);
        }
        if (!dpi) {
            using GetDpiForSystem_t = UINT(WINAPI*)();
            auto getDpiForSystem =
                (GetDpiForSystem_t)GetProcAddress(user32, "GetDpiForSystem");
            if (getDpiForSystem) {
                dpi = getDpiForSystem();
            }
        }
    }
    if (!dpi) {
        HDC screen = GetDC(nullptr);
        if (screen) {
            dpi = (UINT)GetDeviceCaps(screen, LOGPIXELSX);
            ReleaseDC(nullptr, screen);
        }
    }
    if (!dpi) {
        dpi = 96;
    }
    return (double)dpi / 96.0;
}

// --- Arrow glyphs ----------------------------------------------------------
static Color BlendColor(const Color& base, BYTE alpha) {
    return Color(alpha, base.GetRed(), base.GetGreen(), base.GetBlue());
}

// Five vector styles, all drawn as paths so they stay crisp at any DPI and need
// no font glyph. `size` is the total arrow height; `cx`/`cy` is its centre.
static void DrawArrow(Graphics& graphics,
                      const Color& color,
                      REAL cx,
                      REAL cy,
                      REAL size,
                      bool down,
                      ArrowStyle style) {
    REAL half = size / 2.0f;
    REAL dir = down ? 1.0f : -1.0f;
    SolidBrush brush(color);

    switch (style) {
        case ArrowStyle::Chevron: {
            // Two strokes meeting at the tip, rounded caps - the Fluent look.
            REAL w = size * 0.40f;
            Pen pen(color, size * 0.20f);
            pen.SetStartCap(LineCapRound);
            pen.SetEndCap(LineCapRound);
            pen.SetLineJoin(LineJoinRound);
            PointF points[3] = {PointF(cx - w, cy - dir * half * 0.55f),
                                PointF(cx, cy + dir * half * 0.55f),
                                PointF(cx + w, cy - dir * half * 0.55f)};
            graphics.DrawLines(&pen, points, 3);
            return;
        }

        case ArrowStyle::Triangle: {
            // Plain filled triangle, no stem.
            REAL w = size * 0.46f;
            PointF points[3] = {PointF(cx - w, cy - dir * half * 0.8f),
                                PointF(cx + w, cy - dir * half * 0.8f),
                                PointF(cx, cy + dir * half * 0.8f)};
            graphics.FillPolygon(&brush, points, 3);
            return;
        }

        case ArrowStyle::Rounded: {
            // Thick rounded stem plus a chevron head: the friendliest shape and
            // the one that survives smallest. Slightly narrower head than the
            // solid style so the numbers stay dominant.
            REAL stem = size * 0.17f;
            Pen stemPen(color, stem);
            stemPen.SetStartCap(LineCapRound);
            stemPen.SetEndCap(LineCapRound);
            graphics.DrawLine(&stemPen, cx, cy - dir * half * 0.78f, cx,
                              cy + dir * half * 0.40f);
            Pen headPen(color, stem);
            headPen.SetStartCap(LineCapRound);
            headPen.SetEndCap(LineCapRound);
            headPen.SetLineJoin(LineJoinRound);
            REAL w = size * 0.30f;
            PointF head[3] = {PointF(cx - w, cy + dir * half * 0.14f),
                              PointF(cx, cy + dir * half * 0.80f),
                              PointF(cx + w, cy + dir * half * 0.14f)};
            graphics.DrawLines(&headPen, head, 3);
            return;
        }

        case ArrowStyle::Circle: {
            // Tinted disc with a small arrow punched out of it - reads as an icon
            // rather than a glyph, which suits a wider widget.
            REAL r = size * 0.5f;
            SolidBrush disc(BlendColor(color, 62));
            graphics.FillEllipse(&disc, cx - r, cy - r, r * 2, r * 2);
            Pen ring(BlendColor(color, 150), size * 0.07f);
            graphics.DrawEllipse(&ring, cx - r, cy - r, r * 2, r * 2);
            REAL inner = size * 0.62f;
            REAL ih = inner / 2.0f;
            REAL headHalf = inner * 0.40f;
            REAL headLen = inner * 0.48f;
            REAL stemHalf = inner * 0.13f;
            REAL tipY = cy + dir * ih;
            REAL headY = tipY - dir * headLen;
            REAL stemY = cy - dir * ih;
            PointF points[7] = {
                PointF(cx - stemHalf, stemY), PointF(cx + stemHalf, stemY),
                PointF(cx + stemHalf, headY), PointF(cx + headHalf, headY),
                PointF(cx, tipY),             PointF(cx - headHalf, headY),
                PointF(cx - stemHalf, headY),
            };
            graphics.FillPolygon(&brush, points, 7);
            return;
        }

        case ArrowStyle::Solid:
        default: {
            // Stem + wide head as one filled polygon: the sturdiest shape, the
            // easiest to read at small sizes.
            REAL headHalf = size * 0.40f;
            REAL headLen = size * 0.48f;
            REAL stemHalf = size * 0.145f;
            REAL tipY = cy + dir * half;
            REAL headY = tipY - dir * headLen;
            REAL stemY = cy - dir * half;
            PointF points[7] = {
                PointF(cx - stemHalf, stemY), PointF(cx + stemHalf, stemY),
                PointF(cx + stemHalf, headY), PointF(cx + headHalf, headY),
                PointF(cx, tipY),             PointF(cx - headHalf, headY),
                PointF(cx - stemHalf, headY),
            };
            graphics.FillPolygon(&brush, points, 7);
            return;
        }
    }
}

// Text on acrylic has no guaranteed contrast, so every string is drawn twice: a
// 1px shadow in the opposite luminance first, then the glyphs. This is what makes
// small text readable over a bright wallpaper without heavier fonts.
static void DrawSharpString(Graphics& graphics,
                            const wchar_t* text,
                            Font& font,
                            const RectF& rect,
                            StringFormat& format,
                            const Brush& brush,
                            bool lightBackground,
                            REAL offset) {
    SolidBrush shadow(lightBackground ? Color(70, 255, 255, 255)
                                      : Color(120, 0, 0, 0));
    RectF shifted(rect.X + offset, rect.Y + offset, rect.Width, rect.Height);
    graphics.DrawString(text, -1, &font, shifted, &format, &shadow);
    graphics.DrawString(text, -1, &font, rect, &format, &brush);
}

// Segoe UI Semibold/Black are separate FAMILIES, not style bits, so the weight
// setting has to pick a family name as well as a style flag. Falls back to plain
// "Segoe UI" + FontStyleBold if a family is missing.
struct ResolvedFont {
    std::wstring family;
    INT style;
};

static ResolvedFont ResolveWeight(TextWeight weight) {
    switch (weight) {
        case TextWeight::Black:
            return {L"Segoe UI Black", FontStyleRegular};
        case TextWeight::Semibold:
            return {L"Segoe UI Semibold", FontStyleRegular};
        case TextWeight::Regular:
            return {kFontName, FontStyleRegular};
        case TextWeight::Bold:
        default:
            return {kFontName, FontStyleBold};
    }
}

// A FontFamily that is guaranteed to be installed: GDI+ silently produces an
// invalid family (and then draws nothing) for a missing name.
static void MakeFontFamily(const ResolvedFont& resolved,
                           std::unique_ptr<FontFamily>& out,
                           INT& styleOut) {
    out = std::make_unique<FontFamily>(resolved.family.c_str(), nullptr);
    styleOut = resolved.style;
    if (out->GetLastStatus() != Ok || !out->IsStyleAvailable(styleOut)) {
        out = std::make_unique<FontFamily>(kFontName, nullptr);
        styleOut = FontStyleBold;
        if (out->GetLastStatus() != Ok) {
            out = std::make_unique<FontFamily>(L"Tahoma", nullptr);
            styleOut = FontStyleBold;
        }
    }
}

// --- Widget painting -------------------------------------------------------
// WM_PAINT only draws data that is already in g_snapshot; it never calls a
// network API.
static void DrawNetworkPanel(HDC hdc, int width, int height, HWND hwnd) {
    ModSettings s = GetSettings();

    NetSnapshot snapshot;
    {
        std::lock_guard<std::mutex> guard(g_snapshotMutex);
        snapshot = g_snapshot;
    }

    Graphics graphics(hdc);
    graphics.SetSmoothingMode(SmoothingModeAntiAlias);
    // Greyscale AA + grid fitting: sharpest stems without the colour fringing
    // ClearType produces when the result is composited onto acrylic.
    graphics.SetTextRenderingHint(TextRenderingHintAntiAliasGridFit);
    graphics.Clear(Color(0, 0, 0, 0));

    const double scale = GetScaleForWindow(hwnd);
    Color textColor{GetCurrentTextColor(s)};
    const bool light = s.autoTheme ? IsSystemLightMode() : false;
    const REAL shadowOffset = (REAL)(scale >= 1.5 ? 2.0 : 1.0);

    Color downColor = textColor;
    Color upColor = textColor;
    if (s.colorArrows) {
        // Higher-luminance accents than the Windows defaults so the arrows keep
        // their contrast against a bright wallpaper showing through the glass,
        // and (in dark mode) against the dark acrylic of the details panel —
        // a saturated blue reads poorly there, so the download arrow leans cyan.
        downColor = light ? Color(255, 0, 82, 168) : Color(255, 140, 224, 255);
        upColor = light ? Color(255, 13, 106, 13) : Color(255, 138, 238, 158);
    }

    // Hover background (only for the widget's own hover state, not the panel's
    // Reset button hover, which shares the same variable).
    if (g_hoverState.load() == 1) {
        GraphicsPath hoverPath;
        AddRoundedRect(hoverPath, 1.0f, 1.0f, (REAL)width - 2.0f,
                       (REAL)height - 2.0f, (REAL)(8.0 * scale));
        SolidBrush hoverBrush(BlendColor(textColor, 26));
        graphics.FillPath(&hoverBrush, &hoverPath);
    }

    std::unique_ptr<FontFamily> family;
    INT weight = FontStyleBold;
    MakeFontFamily(ResolveWeight(s.textWeight), family, weight);
    FontFamily& fontFamily = *family;
    // Captions keep a fixed weight so the SPEED/TOTAL hierarchy does not collapse
    // when the user picks a lighter weight for the values.
    std::unique_ptr<FontFamily> captionFamily;
    INT captionWeight = FontStyleBold;
    MakeFontFamily(ResolveWeight(TextWeight::Bold), captionFamily, captionWeight);
    REAL fontSize = (REAL)(s.fontSize * scale);
    Font fontValue(&fontFamily, fontSize, weight, UnitPixel);
    Font fontTotal(&fontFamily, fontSize * 0.94f, weight, UnitPixel);
    Font fontCaption(captionFamily.get(), fontSize * 0.62f, captionWeight,
                     UnitPixel);
    SolidBrush textBrush(textColor);
    SolidBrush dimBrush(BlendColor(textColor, 250));
    SolidBrush captionBrush(BlendColor(textColor, 185));

    StringFormat format(StringFormat::GenericTypographic());
    // GenericTypographic drops the ~1/6 em of padding DrawString adds on each
    // side; without it a value like "12.4 MB/s" loses its trailing character in a
    // two-column layout.
    format.SetAlignment(StringAlignmentNear);
    format.SetLineAlignment(StringAlignmentCenter);
    format.SetFormatFlags(format.GetFormatFlags() | StringFormatFlagsNoWrap |
                          StringFormatFlagsNoClip);

    const REAL padding = (REAL)(9.0 * scale);
    // Row centres are a fixed fraction of the height apart; the arrow is capped to
    // that distance minus a small gap, so a large ArrowScale can never make the
    // down/up glyphs touch or spill outside the widget. Bigger arrows therefore
    // need a taller widget (Panel height), which the setting description says.
    REAL rowPitch = (REAL)height * (s.showColumnLabels ? 0.36f : 0.44f);
    REAL arrowLimit = rowPitch - (REAL)(4.0 * scale);
    if (arrowLimit < 4.0f) {
        arrowLimit = 4.0f;
    }
    REAL arrowWanted = fontSize * (s.arrowScale / 100.0f);
    const REAL arrowSize = arrowWanted < arrowLimit ? arrowWanted : arrowLimit;
    const REAL arrowGap = (REAL)(6.0 * scale);

    if (!snapshot.haveInterface) {
        // Disconnected / no adapter: single centered line, no fake numbers.
        StringFormat centered;
        centered.SetAlignment(StringAlignmentCenter);
        centered.SetLineAlignment(StringAlignmentCenter);
        RectF area(0, 0, (REAL)width, (REAL)height);
        const wchar_t* message =
            snapshot.ifName.empty() ? L"No Network" : snapshot.ifName.c_str();
        DrawSharpString(graphics, message, fontValue, area, centered, textBrush,
                        light, shadowOffset);
        return;
    }

    std::wstring downSpeed = FormatSpeed(snapshot.downBytesPerSec, s);
    std::wstring upSpeed = FormatSpeed(snapshot.upBytesPerSec, s);
    std::wstring downTotal = FormatBytes(snapshot.totalDown, s);
    std::wstring upTotal = FormatBytes(snapshot.totalUp, s);

    auto drawRow = [&](REAL x, REAL centerY, bool down, const std::wstring& text,
                       Font& font, SolidBrush& brush, REAL maxWidth) {
        DrawArrow(graphics, down ? downColor : upColor, x + arrowSize / 2.0f,
                  centerY, arrowSize, down, s.arrowStyle);
        RectF textRect(x + arrowSize + arrowGap, centerY - fontSize,
                       maxWidth - arrowSize - arrowGap, fontSize * 2.0f);
        DrawSharpString(graphics, text.c_str(), font, textRect, format, brush,
                        light, shadowOffset);
    };

    if (s.layout == LayoutMode::OneLine) {
        REAL centerY = height / 2.0f;
        REAL columnWidth = ((REAL)width - padding * 2.0f) / 2.0f;
        drawRow(padding, centerY, true, downSpeed, fontValue, textBrush,
                columnWidth);
        drawRow(padding + columnWidth, centerY, false, upSpeed, fontValue,
                textBrush, columnWidth);
        return;
    }

    REAL topY = height * 0.30f;
    REAL bottomY = height * 0.72f;

    if (s.layout == LayoutMode::Speeds) {
        REAL columnWidth = (REAL)width - padding * 2.0f;
        drawRow(padding, topY, true, downSpeed, fontValue, textBrush, columnWidth);
        drawRow(padding, bottomY, false, upSpeed, fontValue, textBrush,
                columnWidth);
        return;
    }

    // Full layout: live speed on the left, cumulative traffic on the right, with
    // captions and a divider so the two groups can't be confused. DividerPos
    // decides how the usable width is split.
    REAL columnGap = (REAL)(10.0 * scale);
    REAL usable = (REAL)width - padding * 2.0f - columnGap;
    REAL leftWidth = usable * (s.dividerPos / 100.0f);
    REAL columnWidth = usable - leftWidth;
    REAL rightX = padding + leftWidth + columnGap;

    if (s.showColumnLabels) {
        REAL captionY = height * 0.16f;
        topY = height * 0.46f;
        bottomY = height * 0.79f;

        DrawSharpString(graphics, L"SPEED", fontCaption,
                        RectF(padding, captionY - fontSize * 0.6f, leftWidth,
                              fontSize * 1.2f),
                        format, captionBrush, light, shadowOffset);
        DrawSharpString(graphics, L"TOTAL", fontCaption,
                        RectF(rightX, captionY - fontSize * 0.6f, columnWidth,
                              fontSize * 1.2f),
                        format, captionBrush, light, shadowOffset);
    }

    // Divider centred in the gutter, spanning the content block only.
    if (s.dividerOpacity > 0) {
        Pen divider(BlendColor(textColor, (BYTE)s.dividerOpacity),
                    (REAL)(1.0 * scale));
        REAL dividerX = padding + leftWidth + columnGap / 2.0f;
        REAL top = s.showColumnLabels ? height * 0.10f : height * 0.20f;
        REAL bottom = s.showColumnLabels ? height * 0.90f : height * 0.80f;
        graphics.DrawLine(&divider, dividerX, top, dividerX, bottom);
    }

    drawRow(padding, topY, true, downSpeed, fontValue, textBrush, leftWidth);
    drawRow(padding, bottomY, false, upSpeed, fontValue, textBrush, leftWidth);
    drawRow(rightX, topY, true, downTotal, fontTotal, dimBrush, columnWidth);
    drawRow(rightX, bottomY, false, upTotal, fontTotal, dimBrush, columnWidth);
}

// --- Details panel painting ------------------------------------------------
struct PanelMetrics {
    int width = 240;
    int height = 250;
    RECT resetButton{};
};

static PanelMetrics ComputePanelMetrics(double scale) {
    ModSettings s = GetSettings();
    PanelMetrics metrics;
    metrics.width = (int)(s.detailsWidth * scale);
    metrics.height = (int)(s.detailsHeight * scale);
    int margin = (int)(16 * scale);
    int buttonHeight = (int)(32 * scale);
    metrics.resetButton.left = margin;
    metrics.resetButton.right = metrics.width - margin;
    metrics.resetButton.bottom = metrics.height - margin;
    metrics.resetButton.top = metrics.resetButton.bottom - buttonHeight;
    return metrics;
}

static void DrawDetailsPanel(HDC hdc, int width, int height, HWND hwnd) {
    ModSettings s = GetSettings();

    NetSnapshot snapshot;
    {
        std::lock_guard<std::mutex> guard(g_snapshotMutex);
        snapshot = g_snapshot;
    }

    Graphics graphics(hdc);
    graphics.SetSmoothingMode(SmoothingModeAntiAlias);
    graphics.SetTextRenderingHint(TextRenderingHintAntiAliasGridFit);
    graphics.Clear(Color(0, 0, 0, 0));

    double scale = GetScaleForWindow(hwnd);
    Color textColor{GetCurrentTextColor(s)};
    bool light = s.autoTheme ? IsSystemLightMode() : false;
    const REAL shadowOffset = (REAL)(scale >= 1.5 ? 2.0 : 1.0);

    Color downColor = s.colorArrows
                          ? (light ? Color(255, 0, 82, 168) : Color(255, 160, 230, 255))
                          : textColor;
    Color upColor = s.colorArrows
                        ? (light ? Color(255, 13, 106, 13) : Color(255, 138, 238, 158))
                        : textColor;

    std::unique_ptr<FontFamily> familyPtr;
    INT weight = FontStyleBold;
    MakeFontFamily(ResolveWeight(s.textWeight), familyPtr, weight);
    FontFamily& fontFamily = *familyPtr;
    REAL base = (REAL)(12.0 * scale);
    Font fontTitle(&fontFamily, base * 0.9f, weight, UnitPixel);
    Font fontLabel(&fontFamily, base * 0.95f, weight, UnitPixel);
    Font fontValue(&fontFamily, base * 1.4f, weight, UnitPixel);
    Font fontSmall(&fontFamily, base * 1.05f, weight, UnitPixel);

    SolidBrush textBrush(textColor);
    SolidBrush labelBrush(BlendColor(textColor, 225));

    StringFormat left(StringFormat::GenericTypographic());
    left.SetAlignment(StringAlignmentNear);
    left.SetLineAlignment(StringAlignmentNear);
    left.SetFormatFlags(left.GetFormatFlags() | StringFormatFlagsNoWrap |
                        StringFormatFlagsNoClip);

    REAL margin = (REAL)(16.0 * scale);
    REAL y = margin;

    DrawSharpString(graphics, L"NETWORK", fontTitle,
                    RectF(margin, y, (REAL)width - margin * 2, base * 1.5f), left,
                    labelBrush, light, shadowOffset);
    y += base * 1.7f;

    std::wstring headline = snapshot.ifName;
    if (!snapshot.ifKind.empty() && snapshot.ifKind != snapshot.ifName) {
        headline += L"  (" + snapshot.ifKind + L")";
    }
    DrawSharpString(graphics, headline.c_str(), fontSmall,
                    RectF(margin, y, (REAL)width - margin * 2, base * 1.6f), left,
                    textBrush, light, shadowOffset);
    y += base * 1.4f;

    std::wstring status = snapshot.connected ? L"Connected" : L"Disconnected";
    if (!snapshot.ipv4.empty()) {
        status += L"  -  " + snapshot.ipv4;
    }
    DrawSharpString(graphics, status.c_str(), fontLabel,
                    RectF(margin, y, (REAL)width - margin * 2, base * 1.6f), left,
                    labelBrush, light, shadowOffset);
    y += base * 2.0f;

    auto drawMetric = [&](const wchar_t* label, const std::wstring& value,
                          bool down) {
        REAL arrow = base * 1.45f;
        DrawArrow(graphics, down ? downColor : upColor, margin + arrow / 2.0f,
                  y + base * 0.6f, arrow, down, s.arrowStyle);
        DrawSharpString(graphics, label, fontLabel,
                        RectF(margin + arrow + (REAL)(7.0 * scale), y,
                              (REAL)width - margin * 2, base * 1.4f),
                        left, labelBrush, light, shadowOffset);
        y += base * 1.45f;
        DrawSharpString(graphics, value.c_str(), fontValue,
                        RectF(margin, y, (REAL)width - margin * 2, base * 1.8f),
                        left, textBrush, light, shadowOffset);
        y += base * 1.85f;
    };

    drawMetric(L"Download", FormatSpeed(snapshot.downBytesPerSec, s), true);
    drawMetric(L"Upload", FormatSpeed(snapshot.upBytesPerSec, s), false);

    DrawSharpString(
        graphics,
        s.counterMode == CounterMode::Persistent ? L"Traffic (persistent)"
                                                 : L"Traffic (session)",
        fontLabel, RectF(margin, y, (REAL)width - margin * 2, base * 1.4f), left,
        labelBrush, light, shadowOffset);
    y += base * 1.5f;

    REAL arrow = base * 1.35f;
    DrawArrow(graphics, downColor, margin + arrow / 2.0f, y + base * 0.55f, arrow,
              true, s.arrowStyle);
    DrawSharpString(graphics, FormatBytes(snapshot.totalDown, s).c_str(), fontSmall,
                    RectF(margin + arrow + (REAL)(7.0 * scale), y,
                          (REAL)width / 2, base * 1.5f),
                    left, textBrush, light, shadowOffset);
    REAL midX = (REAL)width / 2.0f + (REAL)(4.0 * scale);
    DrawArrow(graphics, upColor, midX + arrow / 2.0f, y + base * 0.55f, arrow,
              false, s.arrowStyle);
    DrawSharpString(graphics, FormatBytes(snapshot.totalUp, s).c_str(), fontSmall,
                    RectF(midX + arrow + (REAL)(7.0 * scale), y, (REAL)width / 2,
                          base * 1.5f),
                    left, textBrush, light, shadowOffset);

    // Reset button
    PanelMetrics metrics = ComputePanelMetrics(scale);
    GraphicsPath buttonPath;
    AddRoundedRect(buttonPath, (REAL)metrics.resetButton.left,
                   (REAL)metrics.resetButton.top,
                   (REAL)(metrics.resetButton.right - metrics.resetButton.left),
                   (REAL)(metrics.resetButton.bottom - metrics.resetButton.top),
                   (REAL)(6.0 * scale));
    bool hoverReset = g_hoverState.load() == 2;
    SolidBrush buttonBrush(BlendColor(textColor, hoverReset ? 56 : 30));
    graphics.FillPath(&buttonBrush, &buttonPath);
    Pen buttonPen(BlendColor(textColor, 70), (REAL)(1.0 * scale));
    graphics.DrawPath(&buttonPen, &buttonPath);

    StringFormat centered;
    centered.SetAlignment(StringAlignmentCenter);
    centered.SetLineAlignment(StringAlignmentCenter);
    DrawSharpString(graphics, L"Reset", fontSmall,
                    RectF((REAL)metrics.resetButton.left,
                          (REAL)metrics.resetButton.top,
                          (REAL)(metrics.resetButton.right -
                                 metrics.resetButton.left),
                          (REAL)(metrics.resetButton.bottom -
                                 metrics.resetButton.top)),
                    centered, textBrush, light, shadowOffset);
}
