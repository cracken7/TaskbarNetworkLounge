// Vector arrow styles, all drawn as paths so they stay crisp at any DPI and need
// no font glyph. `size` is the total arrow height; `cx`/`cy` is its centre.
// `style` None draws nothing (the "hide arrows" choice).

// Filled stem + head polygon shared by the solid-family styles; every dimension
// is a fraction of `size`, so the shape scales without drifting.
static void FillStemHeadArrow(Graphics& graphics,
                              const Brush& brush,
                              REAL cx,
                              REAL cy,
                              REAL size,
                              REAL dir,
                              REAL headHalf,
                              REAL headLen,
                              REAL stemHalf) {
    REAL half = size / 2.0f;
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
}

// Chevron strokes meeting at the tip, rounded caps - the Fluent look.
static void StrokeChevronArrow(Graphics& graphics,
                               const Color& color,
                               REAL cx,
                               REAL cy,
                               REAL size,
                               REAL dir,
                               REAL width,
                               REAL stroke) {
    Pen pen(color, stroke);
    pen.SetStartCap(LineCapRound);
    pen.SetEndCap(LineCapRound);
    pen.SetLineJoin(LineJoinRound);
    PointF points[3] = {PointF(cx - width, cy - dir * size * 0.28f),
                        PointF(cx, cy + dir * size * 0.28f),
                        PointF(cx + width, cy - dir * size * 0.28f)};
    graphics.DrawLines(&pen, points, 3);
}

static void DrawArrowCore(Graphics& graphics,
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
            StrokeChevronArrow(graphics, color, cx, cy, size, dir,
                               size * 0.40f, size * 0.20f);
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

        case ArrowStyle::Double: {
            // Two stacked chevrons: reads as "fast" without a stem.
            StrokeChevronArrow(graphics, color, cx, cy - dir * size * 0.17f,
                               size, dir, size * 0.34f, size * 0.15f);
            StrokeChevronArrow(graphics, color, cx, cy + dir * size * 0.17f,
                               size, dir, size * 0.34f, size * 0.15f);
            return;
        }

        case ArrowStyle::Triple: {
            // Three stacked chevrons - the classic speed-lines mark.
            StrokeChevronArrow(graphics, color, cx, cy - dir * size * 0.28f,
                               size, dir, size * 0.32f, size * 0.13f);
            StrokeChevronArrow(graphics, color, cx, cy, size, dir,
                               size * 0.32f, size * 0.13f);
            StrokeChevronArrow(graphics, color, cx, cy + dir * size * 0.28f,
                               size, dir, size * 0.32f, size * 0.13f);
            return;
        }

        case ArrowStyle::Outline: {
            // The solid silhouette stroked only - hollow, light on the eye.
            GraphicsPath path;
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
            path.AddClosedCurve(points, 7, 0.0f);
            Pen pen(color, size * 0.11f);
            pen.SetLineJoin(LineJoinRound);
            graphics.DrawPath(&pen, &path);
            return;
        }

        case ArrowStyle::Thin: {
            // Hairline stem + narrow chevron head: the quietest arrow here.
            Pen pen(color, size * 0.09f);
            pen.SetStartCap(LineCapRound);
            pen.SetEndCap(LineCapRound);
            graphics.DrawLine(&pen, cx, cy - dir * half, cx,
                              cy + dir * half * 0.75f);
            PointF head[3] = {PointF(cx - size * 0.26f, cy + dir * half * 0.24f),
                              PointF(cx, cy + dir * half),
                              PointF(cx + size * 0.26f, cy + dir * half * 0.24f)};
            graphics.DrawLines(&pen, head, 3);
            return;
        }

        case ArrowStyle::Fat: {
            // Wide head, thick stem: the loudest arrow here, good on bright
            // wallpapers where thin strokes wash out.
            FillStemHeadArrow(graphics, brush, cx, cy, size, dir,
                              size * 0.50f, size * 0.52f, size * 0.22f);
            return;
        }

        case ArrowStyle::Head: {
            // Just the head, floating: minimal ink, still unmistakable.
            REAL w = size * 0.42f;
            Pen pen(color, size * 0.16f);
            pen.SetStartCap(LineCapRound);
            pen.SetEndCap(LineCapRound);
            pen.SetLineJoin(LineJoinRound);
            PointF head[3] = {PointF(cx - w, cy - dir * size * 0.22f),
                              PointF(cx, cy + dir * size * 0.22f),
                              PointF(cx + w, cy - dir * size * 0.22f)};
            graphics.DrawLines(&pen, head, 3);
            return;
        }

        case ArrowStyle::Ring: {
            // Chevron inside a thin ring - badge-like without the filled disc.
            REAL r = size * 0.5f;
            Pen ring(color, size * 0.08f);
            graphics.DrawEllipse(&ring, cx - r, cy - r, r * 2, r * 2);
            StrokeChevronArrow(graphics, color, cx, cy + dir * size * 0.02f,
                               size, dir, size * 0.24f, size * 0.11f);
            return;
        }

        case ArrowStyle::Square: {
            // Circle badge's sibling: rounded square, tinted fill.
            REAL r = size * 0.5f;
            REAL sq = r * 1.7f;
            REAL sqX = cx - sq / 2.0f;
            REAL sqY = cy - sq / 2.0f;
            GraphicsPath box;
            AddRoundedRect(box, sqX, sqY, sq, sq, size * 0.22f);
            SolidBrush fill(BlendColor(color, 62));
            graphics.FillPath(&fill, &box);
            Pen edge(BlendColor(color, 150), size * 0.07f);
            graphics.DrawPath(&edge, &box);
            StrokeChevronArrow(graphics, color, cx, cy + dir * size * 0.02f,
                               size, dir, size * 0.22f, size * 0.11f);
            return;
        }

        case ArrowStyle::Fade: {
            // Solid head, stem drawn in segments that thin out toward the tail.
            FillStemHeadArrow(graphics, brush, cx, cy, size, dir,
                              size * 0.40f, size * 0.48f, size * 0.145f);
            REAL stemY0 = cy - dir * half;
            REAL stemY1 = cy - dir * half + dir * size * 0.50f;
            const int kSegs = 6;
            for (int i = 0; i < kSegs; i++) {
                REAL t0 = (REAL)i / kSegs;
                REAL t1 = (REAL)(i + 1) / kSegs;
                BYTE a = (BYTE)(255.0f * (0.15f + 0.85f * t1));
                Pen seg(BlendColor(color, a), size * (0.05f + 0.11f * t1));
                seg.SetStartCap(LineCapRound);
                seg.SetEndCap(LineCapRound);
                graphics.DrawLine(&seg, cx, stemY0 + (stemY1 - stemY0) * t0, cx,
                                  stemY0 + (stemY1 - stemY0) * t1);
            }
            return;
        }

        case ArrowStyle::Dashed: {
            // Solid head, dashed shaft. The shaft runs from the tail toward the
            // head; lo/hi are normalised so the loop always moves downward in
            // screen space whatever direction the arrow points.
            FillStemHeadArrow(graphics, brush, cx, cy, size, dir,
                              size * 0.40f, size * 0.48f, size * 0.145f);
            REAL dash = size * 0.13f;
            REAL shaftA = cy - dir * half;
            REAL shaftB = shaftA + dir * size * 0.52f;
            REAL lo = shaftA < shaftB ? shaftA : shaftB;
            REAL hi = shaftA < shaftB ? shaftB : shaftA;
            for (REAL t = lo; t < hi; t += dash * 2.0f) {
                REAL segEnd = t + dash;
                if (segEnd > hi) {
                    segEnd = hi;
                }
                SolidBrush dashBrush(color);
                graphics.FillRectangle(&dashBrush, cx - size * 0.10f, t,
                                       size * 0.20f, segEnd - t);
            }
            return;
        }

        case ArrowStyle::Barb: {
            // Broadhead: the head base sweeps into backward barbs.
            REAL headHalf = size * 0.46f;
            REAL headLen = size * 0.55f;
            REAL stemHalf = size * 0.11f;
            REAL tipY = cy + dir * half;
            REAL headY = tipY - dir * headLen;
            REAL barbY = headY + dir * headLen * 0.45f;
            REAL stemY = cy - dir * half;
            PointF points[9] = {
                PointF(cx - stemHalf, stemY), PointF(cx + stemHalf, stemY),
                PointF(cx + stemHalf, headY), PointF(cx + headHalf, headY),
                PointF(cx + stemHalf * 1.6f, barbY),
                PointF(cx, tipY),
                PointF(cx - stemHalf * 1.6f, barbY),
                PointF(cx - headHalf, headY),
                PointF(cx - stemHalf, headY),
            };
            graphics.FillPolygon(&brush, points, 9);
            return;
        }

        case ArrowStyle::Needle: {
            // A slim spearhead: all tip, no stem.
            REAL w = size * 0.20f;
            REAL tipY = cy + dir * half;
            REAL tailY = cy - dir * half;
            REAL shoulderY = tipY - dir * size * 0.55f;
            PointF points[4] = {
                PointF(cx, tipY), PointF(cx + w, shoulderY),
                PointF(cx, tailY), PointF(cx - w, shoulderY),
            };
            graphics.FillPolygon(&brush, points, 4);
            return;
        }

        case ArrowStyle::Dot: {
            // Rounded stem + head with a dot at the tail - reads as a pin.
            REAL stem = size * 0.13f;
            REAL inset = stem / 2.0f;
            REAL top = cy - dir * (half - size * 0.16f);
            REAL tip = cy + dir * (half - inset);
            Pen stemPen(color, stem);
            stemPen.SetStartCap(LineCapRound);
            stemPen.SetEndCap(LineCapRound);
            graphics.DrawLine(&stemPen, cx, top, cx, cy + dir * half * 0.35f);
            REAL w = size * 0.28f - inset;
            Pen headPen(color, stem);
            headPen.SetStartCap(LineCapRound);
            headPen.SetEndCap(LineCapRound);
            headPen.SetLineJoin(LineJoinRound);
            PointF head[3] = {PointF(cx - w, tip - dir * (w + inset)),
                              PointF(cx, tip),
                              PointF(cx + w, tip - dir * (w + inset))};
            graphics.DrawLines(&headPen, head, 3);
            REAL dotR = size * 0.13f;
            graphics.FillEllipse(&brush, cx - dotR, top - dotR, dotR * 2,
                                 dotR * 2);
            return;
        }

        case ArrowStyle::Feather: {
            // Solid silhouette plus two fletch strokes at the tail.
            FillStemHeadArrow(graphics, brush, cx, cy, size, dir,
                              size * 0.38f, size * 0.46f, size * 0.13f);
            Pen pen(color, size * 0.09f);
            pen.SetStartCap(LineCapRound);
            pen.SetEndCap(LineCapRound);
            REAL stemY = cy - dir * half;
            graphics.DrawLine(&pen, cx, stemY + dir * size * 0.06f,
                              cx - size * 0.22f, stemY + dir * size * 0.24f);
            graphics.DrawLine(&pen, cx, stemY + dir * size * 0.06f,
                              cx + size * 0.22f, stemY + dir * size * 0.24f);
            return;
        }

        case ArrowStyle::Layered: {
            // A dim copy offset behind the solid one - a soft drop shadow that
            // gives the arrow depth on flat backgrounds.
            REAL off = size * 0.10f;
            FillStemHeadArrow(graphics, SolidBrush(BlendColor(color, 80)),
                              cx - off, cy - off * 0.6f, size, dir,
                              size * 0.40f, size * 0.48f, size * 0.145f);
            FillStemHeadArrow(graphics, brush, cx, cy, size, dir,
                              size * 0.40f, size * 0.48f, size * 0.145f);
            return;
        }

        case ArrowStyle::Tray: {
            // The Windows "download/upload" mark: a stem + head arrow feeding a
            // tray - a horizontal base line with short upturned ends. For the
            // download arrow the tip points INTO the tray; for upload the arrow
            // rises OUT of it, so the pair reads as in / out at a glance.
            REAL stem = size * 0.15f;
            REAL inset = stem / 2.0f;
            REAL trayY = cy + dir * half;
            REAL trayW = size * 0.42f;
            REAL trayLip = size * 0.16f;
            Pen tray(color, stem * 0.85f);
            tray.SetStartCap(LineCapRound);
            tray.SetEndCap(LineCapRound);
            tray.SetLineJoin(LineJoinRound);
            PointF trayPts[4] = {
                PointF(cx - trayW, trayY - dir * trayLip),
                PointF(cx - trayW, trayY),
                PointF(cx + trayW, trayY),
                PointF(cx + trayW, trayY - dir * trayLip),
            };
            graphics.DrawLines(&tray, trayPts, 4);
            // The arrow itself sits above the tray, shortened so it never
            // touches the base line.
            REAL arrowSpan = size * 0.66f;
            REAL aHalf = arrowSpan / 2.0f;
            REAL aCy = cy - dir * (half - aHalf - size * 0.06f);
            REAL tip = aCy + dir * (aHalf - inset);
            REAL top = aCy - dir * (aHalf - inset);
            Pen stemPen(color, stem);
            stemPen.SetStartCap(LineCapRound);
            stemPen.SetEndCap(LineCapRound);
            graphics.DrawLine(&stemPen, cx, top, cx, aCy + dir * aHalf * 0.3f);
            REAL w = size * 0.26f - inset;
            Pen headPen(color, stem);
            headPen.SetStartCap(LineCapRound);
            headPen.SetEndCap(LineCapRound);
            headPen.SetLineJoin(LineJoinRound);
            PointF head[3] = {PointF(cx - w, tip - dir * (w + inset)),
                              PointF(cx, tip),
                              PointF(cx + w, tip - dir * (w + inset))};
            graphics.DrawLines(&headPen, head, 3);
            return;
        }

        case ArrowStyle::Plus: {
            // A plus sign carrying the arrow: the vertical stroke IS the shaft
            // (head at its tip), and a horizontal bar crosses it - so download
            // reads as a "+" pointing down and upload as a "+" pointing up.
            REAL bar = size * 0.16f;
            REAL inset = bar / 2.0f;
            REAL tip = cy + dir * (half - inset);
            REAL tail = cy - dir * (half - inset);
            Pen shaft(color, bar);
            shaft.SetStartCap(LineCapRound);
            shaft.SetEndCap(LineCapRound);
            graphics.DrawLine(&shaft, cx, tail, cx, cy + dir * half * 0.30f);
            // The cross-bar sits toward the tail so the head stays dominant.
            REAL barY = cy - dir * half * 0.30f;
            REAL barW = size * 0.34f;
            graphics.DrawLine(&shaft, cx - barW, barY, cx + barW, barY);
            // Chevron head on the tip.
            REAL w = size * 0.28f - inset;
            Pen headPen(color, bar);
            headPen.SetStartCap(LineCapRound);
            headPen.SetEndCap(LineCapRound);
            headPen.SetLineJoin(LineJoinRound);
            PointF head[3] = {PointF(cx - w, tip - dir * (w + inset)),
                              PointF(cx, tip),
                              PointF(cx + w, tip - dir * (w + inset))};
            graphics.DrawLines(&headPen, head, 3);
            return;
        }

        case ArrowStyle::None:
            // "Hide arrows": nothing is drawn and the caller collapses the
            // gutter so the numbers move left into the freed space.
            return;

        case ArrowStyle::Solid:
        default: {
            // Stem + wide head as one filled polygon: the sturdiest shape, the
            // easiest to read at small sizes.
            FillStemHeadArrow(graphics, brush, cx, cy, size, dir,
                              size * 0.40f, size * 0.48f, size * 0.145f);
            return;
        }
    }
}

static void DrawArrow(Graphics& graphics,
                      const Color& color,
                      REAL cx,
                      REAL cy,
                      REAL size,
                      bool down,
                      ArrowStyle style) {
    DrawArrowCore(graphics, color, cx, cy, size, down, style);
}

// Horizontal space an arrow reserves in front of its number. None collapses it
// to zero so the text slides left into the freed space instead of leaving a
// gap where no glyph is drawn.
static REAL ArrowGutter(ArrowStyle style, REAL arrowSize, REAL arrowGap) {
    return style == ArrowStyle::None ? 0.0f : arrowSize + arrowGap;
}

