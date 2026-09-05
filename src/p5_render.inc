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

// Down/up chevron glyphs drawn as vectors so they stay crisp at any DPI and do
// not depend on a font containing the arrow characters.
static void DrawArrow(Graphics& graphics,
                      const Color& color,
                      REAL cx,
                      REAL cy,
                      REAL size,
                      bool down) {
    REAL half = size / 2.0f;
    REAL stem = size * 0.22f;
    Pen pen(color, size * 0.16f);
    pen.SetStartCap(LineCapRound);
    pen.SetEndCap(LineCapRound);

    if (down) {
        graphics.DrawLine(&pen, cx, cy - half, cx, cy + half * 0.35f);
        PointF points[3] = {PointF(cx - stem, cy + half * 0.05f),
                            PointF(cx, cy + half),
                            PointF(cx + stem, cy + half * 0.05f)};
        SolidBrush brush(color);
        graphics.FillPolygon(&brush, points, 3);
    } else {
        graphics.DrawLine(&pen, cx, cy + half, cx, cy - half * 0.35f);
        PointF points[3] = {PointF(cx - stem, cy - half * 0.05f),
                            PointF(cx, cy - half),
                            PointF(cx + stem, cy - half * 0.05f)};
        SolidBrush brush(color);
        graphics.FillPolygon(&brush, points, 3);
    }
}

static Color BlendColor(const Color& base, BYTE alpha) {
    return Color(alpha, base.GetRed(), base.GetGreen(), base.GetBlue());
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
    graphics.SetTextRenderingHint(TextRenderingHintClearTypeGridFit);
    graphics.Clear(Color(0, 0, 0, 0));

    const double scale = GetScaleForWindow(hwnd);
    Color textColor{GetCurrentTextColor(s)};

    Color downColor = textColor;
    Color upColor = textColor;
    if (s.colorArrows) {
        bool light = s.autoTheme ? IsSystemLightMode() : false;
        downColor = light ? Color(255, 0, 95, 184) : Color(255, 96, 205, 255);
        upColor = light ? Color(255, 16, 124, 16) : Color(255, 108, 219, 132);
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

    FontFamily fontFamily(kFontName, nullptr);
    REAL fontSize = (REAL)(s.fontSize * scale);
    Font fontValue(&fontFamily, fontSize, FontStyleBold, UnitPixel);
    Font fontTotal(&fontFamily, fontSize * 0.92f, FontStyleRegular, UnitPixel);
    SolidBrush textBrush(textColor);
    SolidBrush dimBrush(BlendColor(textColor, 200));

    StringFormat format;
    format.SetAlignment(StringAlignmentNear);
    format.SetLineAlignment(StringAlignmentCenter);
    format.SetFormatFlags(StringFormatFlagsNoWrap);

    const REAL padding = (REAL)(10.0 * scale);
    const REAL arrowSize = fontSize * 1.05f;
    const REAL arrowGap = (REAL)(5.0 * scale);

    if (!snapshot.haveInterface) {
        // Disconnected / no adapter: single centered line, no fake numbers.
        StringFormat centered;
        centered.SetAlignment(StringAlignmentCenter);
        centered.SetLineAlignment(StringAlignmentCenter);
        RectF area(0, 0, (REAL)width, (REAL)height);
        const wchar_t* message =
            snapshot.ifName.empty() ? L"No Network" : snapshot.ifName.c_str();
        graphics.DrawString(message, -1, &fontValue, area, &centered, &textBrush);
        return;
    }

    std::wstring downSpeed = FormatSpeed(snapshot.downBytesPerSec, s);
    std::wstring upSpeed = FormatSpeed(snapshot.upBytesPerSec, s);
    std::wstring downTotal = FormatBytes(snapshot.totalDown, s);
    std::wstring upTotal = FormatBytes(snapshot.totalUp, s);

    auto drawRow = [&](REAL x, REAL centerY, bool down, const std::wstring& text,
                       Font& font, SolidBrush& brush, REAL maxWidth) {
        DrawArrow(graphics, down ? downColor : upColor, x + arrowSize / 2.0f,
                  centerY, arrowSize, down);
        RectF textRect(x + arrowSize + arrowGap, centerY - fontSize,
                       maxWidth - arrowSize - arrowGap, fontSize * 2.0f);
        graphics.DrawString(text.c_str(), -1, &font, textRect, &format, &brush);
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

    // Full layout: speeds on the left, totals on the right.
    REAL columnGap = (REAL)(8.0 * scale);
    REAL columnWidth = ((REAL)width - padding * 2.0f - columnGap) / 2.0f;
    REAL rightX = padding + columnWidth + columnGap;

    drawRow(padding, topY, true, downSpeed, fontValue, textBrush, columnWidth);
    drawRow(padding, bottomY, false, upSpeed, fontValue, textBrush, columnWidth);
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
    PanelMetrics metrics;
    metrics.width = (int)(240 * scale);
    metrics.height = (int)(252 * scale);
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
    graphics.SetTextRenderingHint(TextRenderingHintClearTypeGridFit);
    graphics.Clear(Color(0, 0, 0, 0));

    double scale = GetScaleForWindow(hwnd);
    Color textColor{GetCurrentTextColor(s)};
    bool light = s.autoTheme ? IsSystemLightMode() : false;

    Color downColor = s.colorArrows
                          ? (light ? Color(255, 0, 95, 184) : Color(255, 96, 205, 255))
                          : textColor;
    Color upColor = s.colorArrows
                        ? (light ? Color(255, 16, 124, 16) : Color(255, 108, 219, 132))
                        : textColor;

    FontFamily fontFamily(kFontName, nullptr);
    REAL base = (REAL)(12.0 * scale);
    Font fontTitle(&fontFamily, base * 0.85f, FontStyleBold, UnitPixel);
    Font fontLabel(&fontFamily, base * 0.85f, FontStyleRegular, UnitPixel);
    Font fontValue(&fontFamily, base * 1.35f, FontStyleBold, UnitPixel);
    Font fontSmall(&fontFamily, base * 0.95f, FontStyleRegular, UnitPixel);

    SolidBrush textBrush(textColor);
    SolidBrush labelBrush(BlendColor(textColor, 165));

    StringFormat left;
    left.SetAlignment(StringAlignmentNear);
    left.SetLineAlignment(StringAlignmentNear);
    left.SetFormatFlags(StringFormatFlagsNoWrap);

    REAL margin = (REAL)(16.0 * scale);
    REAL y = margin;

    graphics.DrawString(L"NETWORK", -1, &fontTitle,
                        RectF(margin, y, (REAL)width - margin * 2, base * 1.5f),
                        &left, &labelBrush);
    y += base * 1.7f;

    std::wstring headline = snapshot.ifName;
    if (!snapshot.ifKind.empty() && snapshot.ifKind != snapshot.ifName) {
        headline += L"  (" + snapshot.ifKind + L")";
    }
    graphics.DrawString(headline.c_str(), -1, &fontSmall,
                        RectF(margin, y, (REAL)width - margin * 2, base * 1.6f),
                        &left, &textBrush);
    y += base * 1.4f;

    std::wstring status = snapshot.connected ? L"Connected" : L"Disconnected";
    if (!snapshot.ipv4.empty()) {
        status += L"  -  " + snapshot.ipv4;
    }
    graphics.DrawString(status.c_str(), -1, &fontLabel,
                        RectF(margin, y, (REAL)width - margin * 2, base * 1.6f),
                        &left, &labelBrush);
    y += base * 2.0f;

    auto drawMetric = [&](const wchar_t* label, const std::wstring& value,
                          bool down) {
        REAL arrow = base * 1.1f;
        DrawArrow(graphics, down ? downColor : upColor, margin + arrow / 2.0f,
                  y + base * 0.55f, arrow, down);
        graphics.DrawString(label, -1, &fontLabel,
                            RectF(margin + arrow + (REAL)(6.0 * scale), y,
                                  (REAL)width - margin * 2, base * 1.4f),
                            &left, &labelBrush);
        y += base * 1.35f;
        graphics.DrawString(value.c_str(), -1, &fontValue,
                            RectF(margin, y, (REAL)width - margin * 2,
                                  base * 1.8f),
                            &left, &textBrush);
        y += base * 1.9f;
    };

    drawMetric(L"Download", FormatSpeed(snapshot.downBytesPerSec, s), true);
    drawMetric(L"Upload", FormatSpeed(snapshot.upBytesPerSec, s), false);

    graphics.DrawString(
        s.counterMode == CounterMode::Persistent ? L"Traffic (persistent)"
                                                 : L"Traffic (session)",
        -1, &fontLabel, RectF(margin, y, (REAL)width - margin * 2, base * 1.4f),
        &left, &labelBrush);
    y += base * 1.5f;

    REAL arrow = base;
    DrawArrow(graphics, downColor, margin + arrow / 2.0f, y + base * 0.55f, arrow,
              true);
    graphics.DrawString(FormatBytes(snapshot.totalDown, s).c_str(), -1, &fontSmall,
                        RectF(margin + arrow + (REAL)(6.0 * scale), y,
                              (REAL)width / 2, base * 1.5f),
                        &left, &textBrush);
    REAL midX = (REAL)width / 2.0f + (REAL)(4.0 * scale);
    DrawArrow(graphics, upColor, midX + arrow / 2.0f, y + base * 0.55f, arrow,
              false);
    graphics.DrawString(FormatBytes(snapshot.totalUp, s).c_str(), -1, &fontSmall,
                        RectF(midX + arrow + (REAL)(6.0 * scale), y,
                              (REAL)width / 2, base * 1.5f),
                        &left, &textBrush);

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
    graphics.DrawString(L"Reset", -1, &fontSmall,
                        RectF((REAL)metrics.resetButton.left,
                              (REAL)metrics.resetButton.top,
                              (REAL)(metrics.resetButton.right -
                                     metrics.resetButton.left),
                              (REAL)(metrics.resetButton.bottom -
                                     metrics.resetButton.top)),
                        &centered, &textBrush);
}
