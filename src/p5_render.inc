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
            //
            // Rounded caps extend the stroke by half its width beyond the end
            // point, so the drawn ink would reach past +-size/2. The geometry
            // below is inset by half the stroke width to keep the visible ink
            // inside the requested box - otherwise this style's glyphs sit on a
            // different optical rail than the solid/triangle ones.
            REAL stem = size * 0.17f;
            REAL inset = stem / 2.0f;
            REAL top = cy - dir * (half - inset);
            REAL tip = cy + dir * (half - inset);
            Pen stemPen(color, stem);
            stemPen.SetStartCap(LineCapRound);
            stemPen.SetEndCap(LineCapRound);
            graphics.DrawLine(&stemPen, cx, top, cx, cy + dir * half * 0.35f);
            Pen headPen(color, stem);
            headPen.SetStartCap(LineCapRound);
            headPen.SetEndCap(LineCapRound);
            headPen.SetLineJoin(LineJoinRound);
            REAL w = size * 0.30f - inset;
            PointF head[3] = {PointF(cx - w, tip - dir * (w + inset)),
                              PointF(cx, tip),
                              PointF(cx + w, tip - dir * (w + inset))};
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

// Draws a value as two pieces on a fixed grid: the digits left-aligned in a
// fixed-width column, then the unit in a smaller, slightly dimmer font starting
// immediately after that column. Both left edges are therefore constant, so
// nothing slides horizontally as the number changes width ("491 KB/s" ->
// "51.2 KB/s"), and the number reads first because the unit is de-emphasised.
static void DrawValueWithFixedUnit(Graphics& graphics,
                                   const std::wstring& text,
                                   Font& numberFont,
                                   Font& unitFont,
                                   REAL x,
                                   REAL y,
                                   REAL width,
                                   REAL height,
                                   StringFormat& format,
                                   const Brush& numberBrush,
                                   const Brush& unitBrush,
                                   bool lightBackground,
                                   REAL shadowOffset,
                                   REAL numberColumnWidth) {
    size_t space = text.rfind(L' ');
    if (space == std::wstring::npos || numberColumnWidth <= 0.0f ||
        numberColumnWidth >= width) {
        DrawSharpString(graphics, text.c_str(), numberFont,
                        RectF(x, y, width, height), format, numberBrush,
                        lightBackground, shadowOffset);
        return;
    }

    std::wstring number = text.substr(0, space);
    std::wstring unit = text.substr(space + 1);

    DrawSharpString(graphics, number.c_str(), numberFont,
                    RectF(x, y, numberColumnWidth, height), format, numberBrush,
                    lightBackground, shadowOffset);

    // The unit's baseline is nudged down so the two sizes sit on one baseline.
    REAL baselineShift =
        (numberFont.GetSize() - unitFont.GetSize()) * 0.78f;
    DrawSharpString(graphics, unit.c_str(), unitFont,
                    RectF(x + numberColumnWidth, y + baselineShift,
                          width - numberColumnWidth, height),
                    format, unitBrush, lightBackground, shadowOffset);
}

// Width of the widest number part ("1023", "12.4", "9.99") in this font, used as
// the fixed digit column so the unit never moves.
static REAL MeasureNumberColumn(Graphics& graphics,
                                Font& font,
                                StringFormat& format) {
    static const wchar_t* kSamples[] = {L"1023", L"12.4", L"9.99", L"999"};
    REAL widest = 0;
    for (const wchar_t* sample : kSamples) {
        RectF bounds;
        graphics.MeasureString(sample, -1, &font, RectF(0, 0, 1000, 100), &format,
                               &bounds);
        if (bounds.Width > widest) {
            widest = bounds.Width;
        }
    }
    // One space of separation before the unit.
    RectF spaceBounds;
    graphics.MeasureString(L" ", -1, &font, RectF(0, 0, 1000, 100), &format,
                           &spaceBounds);
    return widest + spaceBounds.Width;
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
    // Only the two-column layout has a divider; clear the hit zone so the drag
    // handler cannot grab a line that is not drawn in the other layouts.
    g_dividerHitX = -1.0f;

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
    // decides how the usable width is split; the dragged offset shifts the LINE
    // ONLY, so grabbing it never moves the text with it.
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

    // Divider. Its x is the split point plus the user's dragged offset - and the
    // offset is applied HERE ONLY, never to leftWidth/rightX, so dragging the line
    // moves nothing but the line. The painted x is published for the hit test so
    // the grab zone always matches what is on screen.
    REAL dividerX = padding + leftWidth + columnGap / 2.0f;
    if (s.dividerDraggable) {
        // Read the LIVE atomic, not the settings snapshot: during a drag the
        // offset changes on every WM_MOUSEMOVE and settings are only reloaded on
        // mouse-up, so using s.dividerOffset here would make the line lag behind
        // the cursor and only snap into place on release.
        dividerX += (REAL)(g_dividerOffset.load() * scale);
        // Keep it inside the widget even if a stale offset is larger than the
        // current width (e.g. the widget was made narrower after a drag).
        REAL minX = padding;
        REAL maxX = (REAL)width - padding;
        if (dividerX < minX) {
            dividerX = minX;
        }
        if (dividerX > maxX) {
            dividerX = maxX;
        }
    }
    g_dividerHitX = (float)dividerX;

    if (s.dividerOpacity > 0 || (s.dividerDraggable && g_hoverState.load() == 3)) {
        // While being dragged (or hovered) the line brightens, so it is obvious
        // that it is the thing being grabbed even at opacity 0.
        BYTE alpha = (BYTE)s.dividerOpacity;
        REAL thickness = (REAL)(1.0 * scale);
        if (g_dividerDragging.load()) {
            alpha = 220;
            thickness = (REAL)(2.0 * scale);
        } else if (g_hoverState.load() == 3) {
            alpha = (BYTE)(alpha < 140 ? 140 : alpha);
            thickness = (REAL)(2.0 * scale);
        }
        Pen divider(BlendColor(textColor, alpha), thickness);
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
// All vertical rhythm lives in one struct so the rows cannot drift apart and the
// panel's natural height is known before it is created.
struct PanelLayout {
    REAL base = 12.0f;
    REAL margin = 16.0f;
    REAL top = 16.0f;
    REAL captionRow = 0;
    REAL headlineRow = 0;
    REAL labelRow = 0;
    REAL valueRow = 0;
    REAL totalRow = 0;
    REAL rowGap = 0;
    REAL sectionGap = 0;
    REAL buttonGap = 0;
    REAL buttonHeight = 0;
    REAL arrow = 0;
    REAL totalArrow = 0;
    REAL arrowGap = 0;
    REAL columnGap = 0;
    REAL naturalHeight = 0;  // height needed to show everything uncramped
};

static PanelLayout ComputePanelLayout(double scale) {
    PanelLayout l;
    l.base = (REAL)(12.0 * scale);
    const REAL b = l.base;

    l.margin = b * 1.34f;
    l.top = l.margin;
    // Row heights are >= the font line height (Segoe UI is ~1.33 em) so nothing
    // is ever clipped and descenders never touch the row below.
    l.captionRow = b * 1.40f;    // caption font 0.82 em
    l.headlineRow = b * 1.80f;   // headline font 1.25 em
    l.labelRow = b * 1.30f;      // label font 0.92 em
    l.valueRow = b * 2.00f;      // value font 1.45 em
    l.totalRow = b * 1.55f;      // total font 1.08 em
    l.rowGap = b * 0.60f;
    l.sectionGap = b * 0.50f;
    l.buttonGap = b * 1.30f;
    l.buttonHeight = b * 2.70f;
    l.arrow = b * 1.50f;
    // The totals use the same arrow size as the speeds so every glyph in the
    // panel sits on one optical rail; only the text gets smaller.
    l.totalArrow = l.arrow;
    l.arrowGap = b * 0.60f;
    // Wide enough for "1023 MB" / "4.82 GB" plus the arrow gutter, so the second
    // column starts at the panel's midpoint rather than wherever the first value
    // happened to end.
    l.columnGap = b * 0.50f;

    l.naturalHeight = l.top + l.captionRow + l.headlineRow + l.labelRow +
                      l.sectionGap * 2.0f +
                      (l.labelRow + l.valueRow) * 2.0f + l.rowGap * 1.6f +
                      l.sectionGap * 2.0f + l.captionRow + l.totalRow +
                      l.buttonGap + l.buttonHeight + l.margin;
    return l;
}

struct PanelMetrics {
    int width = 240;
    int height = 250;
    RECT resetButton{};
};

static PanelMetrics ComputePanelMetrics(double scale) {
    ModSettings s = GetSettings();
    PanelLayout layout = ComputePanelLayout(scale);

    PanelMetrics metrics;
    metrics.width = (int)(s.detailsWidth * scale);
    // The setting is a minimum: the panel never shrinks below the height its own
    // content needs, so no row can end up cramped or clipped.
    int wanted = (int)(s.detailsHeight * scale);
    int needed = (int)(layout.naturalHeight + 0.5f);
    metrics.height = wanted > needed ? wanted : needed;

    int margin = (int)layout.margin;
    int buttonHeight = (int)layout.buttonHeight;
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
    // Captions keep a fixed bold weight so the section hierarchy survives a
    // lighter weight choice for the values.
    std::unique_ptr<FontFamily> captionFamilyPtr;
    INT captionWeight = FontStyleBold;
    MakeFontFamily(ResolveWeight(TextWeight::Bold), captionFamilyPtr,
                   captionWeight);

    const PanelLayout layout = ComputePanelLayout(scale);
    const REAL base = layout.base;

    Font fontCaption(captionFamilyPtr.get(), base * 0.82f, captionWeight, UnitPixel);
    Font fontHeadline(&fontFamily, base * 1.25f, weight, UnitPixel);
    Font fontLabel(&fontFamily, base * 0.92f, weight, UnitPixel);
    Font fontValue(&fontFamily, base * 1.45f, weight, UnitPixel);
    Font fontValueUnit(&fontFamily, base * 1.05f, weight, UnitPixel);
    Font fontTotal(&fontFamily, base * 1.08f, weight, UnitPixel);
    Font fontTotalUnit(&fontFamily, base * 0.92f, weight, UnitPixel);

    SolidBrush textBrush(textColor);
    SolidBrush labelBrush(BlendColor(textColor, 215));
    SolidBrush captionBrush(BlendColor(textColor, 200));
    SolidBrush unitBrush(BlendColor(textColor, 190));

    // One shared format: near/near so every row is positioned by its own rect
    // rather than by vertical centring inside a guessed box.
    StringFormat left(StringFormat::GenericTypographic());
    left.SetAlignment(StringAlignmentNear);
    left.SetLineAlignment(StringAlignmentNear);
    left.SetFormatFlags(left.GetFormatFlags() | StringFormatFlagsNoWrap |
                        StringFormatFlagsNoClip);

    const REAL margin = layout.margin;
    const REAL contentWidth = (REAL)width - margin * 2.0f;
    REAL y = layout.top;

    auto separator = [&](REAL atY) {
        Pen pen(BlendColor(textColor, 38), (REAL)(1.0 * scale));
        graphics.DrawLine(&pen, margin, atY, margin + contentWidth, atY);
    };

    // --- Header ------------------------------------------------------------
    DrawSharpString(graphics, L"NETWORK", fontCaption,
                    RectF(margin, y, contentWidth, layout.captionRow), left,
                    captionBrush, light, shadowOffset);
    y += layout.captionRow;

    std::wstring headline = snapshot.ifName;
    if (!snapshot.ifKind.empty() && snapshot.ifKind != snapshot.ifName) {
        headline += L"  (" + snapshot.ifKind + L")";
    }
    DrawSharpString(graphics, headline.c_str(), fontHeadline,
                    RectF(margin, y, contentWidth, layout.headlineRow), left,
                    textBrush, light, shadowOffset);
    y += layout.headlineRow;

    std::wstring status = snapshot.connected ? L"Connected" : L"Disconnected";
    if (!snapshot.ipv4.empty()) {
        status += L"  \u00B7  " + snapshot.ipv4;  // middle dot, single spaces
    }
    DrawSharpString(graphics, status.c_str(), fontLabel,
                    RectF(margin, y, contentWidth, layout.labelRow), left,
                    labelBrush, light, shadowOffset);
    y += layout.labelRow;

    y += layout.sectionGap;
    separator(y);
    y += layout.sectionGap;

    // --- Live speeds -------------------------------------------------------
    // The arrow sits in its own gutter and is centred on the label+value pair;
    // label and value then share ONE left edge, so the block has a single
    // text column instead of two competing ones.
    const REAL arrow = layout.arrow;
    const REAL textX = margin + arrow + layout.arrowGap;
    const REAL textWidth = contentWidth - arrow - layout.arrowGap;
    const REAL valueNumberColumn = MeasureNumberColumn(graphics, fontValue, left);

    auto drawMetric = [&](const wchar_t* label, const std::wstring& value,
                          bool down) {
        REAL blockTop = y;
        REAL blockHeight = layout.labelRow + layout.valueRow;
        DrawArrow(graphics, down ? downColor : upColor, margin + arrow / 2.0f,
                  blockTop + blockHeight / 2.0f, arrow, down, s.arrowStyle);
        DrawSharpString(graphics, label, fontLabel,
                        RectF(textX, y, textWidth, layout.labelRow), left,
                        labelBrush, light, shadowOffset);
        y += layout.labelRow;
        DrawValueWithFixedUnit(graphics, value, fontValue, fontValueUnit, textX, y,
                               textWidth, layout.valueRow, left, textBrush,
                               unitBrush, light, shadowOffset, valueNumberColumn);
        y += layout.valueRow;
    };

    drawMetric(L"Download", FormatSpeed(snapshot.downBytesPerSec, s), true);
    y += layout.rowGap * 1.6f;  // clear separation between the two pairs
    drawMetric(L"Upload", FormatSpeed(snapshot.upBytesPerSec, s), false);

    y += layout.sectionGap;
    separator(y);
    y += layout.sectionGap;

    // --- Totals ------------------------------------------------------------
    DrawSharpString(graphics,
                    s.counterMode == CounterMode::Persistent
                        ? L"TRAFFIC \u00B7 PERSISTENT"
                        : L"TRAFFIC \u00B7 SESSION",
                    fontCaption, RectF(margin, y, contentWidth, layout.captionRow),
                    left, captionBrush, light, shadowOffset);
    y += layout.captionRow;

    // Totals row: the download pair sits on the same left rail as everything
    // else; the upload pair is right-aligned as a group, so the row has equal
    // padding at both ends instead of a pocket of dead space on the right.
    const REAL totalArrow = layout.totalArrow;
    const REAL halfWidth = (contentWidth - layout.columnGap) / 2.0f;
    const REAL totalNumberColumn = MeasureNumberColumn(graphics, fontTotal, left);

    DrawArrow(graphics, downColor, margin + arrow / 2.0f,
              y + layout.totalRow / 2.0f, totalArrow, true, s.arrowStyle);
    DrawValueWithFixedUnit(graphics, FormatBytes(snapshot.totalDown, s), fontTotal,
                           fontTotalUnit, margin + arrow + layout.arrowGap, y,
                           halfWidth - arrow - layout.arrowGap, layout.totalRow,
                           left, textBrush, unitBrush, light, shadowOffset,
                           totalNumberColumn);

    // The upload pair is placed as a group whose right edge lands on the content
    // edge, so the row has equal padding at both ends. Its own digits still use
    // the fixed number column, so the unit does not move as the value grows.
    std::wstring upTotalText = FormatBytes(snapshot.totalUp, s);
    size_t upSpace = upTotalText.rfind(L' ');
    std::wstring upUnit = upSpace == std::wstring::npos
                              ? std::wstring()
                              : upTotalText.substr(upSpace + 1);
    RectF unitBounds;
    graphics.MeasureString(upUnit.empty() ? L"GB" : upUnit.c_str(), -1,
                           &fontTotalUnit,
                           RectF(0, 0, contentWidth, layout.totalRow), &left,
                           &unitBounds);
    REAL upGroupWidth =
        arrow + layout.arrowGap + totalNumberColumn + unitBounds.Width;
    REAL upGroupX = margin + contentWidth - upGroupWidth;
    REAL upGroupFloor = margin + halfWidth + layout.columnGap;
    if (upGroupX < upGroupFloor) {
        upGroupX = upGroupFloor;
    }
    DrawArrow(graphics, upColor, upGroupX + arrow / 2.0f,
              y + layout.totalRow / 2.0f, totalArrow, false, s.arrowStyle);
    DrawValueWithFixedUnit(graphics, upTotalText, fontTotal, fontTotalUnit,
                           upGroupX + arrow + layout.arrowGap, y,
                           upGroupWidth - arrow - layout.arrowGap, layout.totalRow,
                           left, textBrush, unitBrush, light, shadowOffset,
                           totalNumberColumn);
    y += layout.totalRow;

    // --- Reset button ------------------------------------------------------
    // Positioned from the panel's real height, so it stays pinned to the bottom
    // margin whatever the content above it needed.
    PanelMetrics metrics = ComputePanelMetrics(scale);
    RECT button = metrics.resetButton;
    // The panel may be taller than the requested size (natural-height floor), so
    // anchor to the DC we were actually given rather than to metrics.height.
    int buttonHeight = button.bottom - button.top;
    button.bottom = height - (int)margin;
    button.top = button.bottom - buttonHeight;
    button.left = (int)margin;
    button.right = width - (int)margin;

    GraphicsPath buttonPath;
    AddRoundedRect(buttonPath, (REAL)button.left, (REAL)button.top,
                   (REAL)(button.right - button.left),
                   (REAL)(button.bottom - button.top), (REAL)(6.0 * scale));
    bool hoverReset = g_hoverState.load() == 2;
    SolidBrush buttonBrush(BlendColor(textColor, hoverReset ? 56 : 30));
    graphics.FillPath(&buttonBrush, &buttonPath);
    Pen buttonPen(BlendColor(textColor, hoverReset ? 110 : 70), (REAL)(1.0 * scale));
    graphics.DrawPath(&buttonPen, &buttonPath);

    StringFormat centered;
    centered.SetAlignment(StringAlignmentCenter);
    centered.SetLineAlignment(StringAlignmentCenter);
    DrawSharpString(graphics, L"Reset traffic counters", fontLabel,
                    RectF((REAL)button.left, (REAL)button.top,
                          (REAL)(button.right - button.left),
                          (REAL)(button.bottom - button.top)),
                    centered, textBrush, light, shadowOffset);
}
