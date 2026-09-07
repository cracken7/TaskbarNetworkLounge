// ==WindhawkMod==
// @id              taskbar-network-lounge
// @name            Taskbar Network Lounge
// @description     Live network speed and traffic totals on the taskbar, in a native acrylic widget
// @version         1.5.0
// @author          cracken7
// @github          https://github.com/cracken7
// @homepage        https://github.com/cracken7/TaskbarNetworkLounge
// @include         explorer.exe
// @license         MIT
// @compilerOptions -lole32 -ldwmapi -lgdi32 -luser32 -lgdiplus -lshell32 -lcomctl32 -liphlpapi -lws2_32
// @name:ar         مؤشر الشبكة لشريط المهام
// @description:ar  سرعة الشبكة الحيّة وإجمالي الترافيك على شريط المهام في ودجت أصلية بمظهر زجاجي
// ==/WindhawkMod==

// The Arabic metadata above uses the short "ar" tag: Windhawk best-matches the UI
// language, so it covers ar-EG, ar-SA and the rest.

// ==WindhawkModReadme==
/*
# Taskbar Network Lounge

A live network meter on your Windows 11 taskbar: your download and upload speed
right now, plus how much data you have used — in a small acrylic widget that
looks like part of Windows.

## What you see

One rectangle split into two halves by a thin line:

- **SPEED** (left) — how fast you are downloading and uploading **now**.
- **TOTAL** (right) — how much you have downloaded and uploaded **in total**.

In each half the top row is download (blue arrow, pointing down) and the bottom
row is upload (green arrow, pointing up). Every number shows its own unit, so
`MB/s` and `Mbps` are never confused.

## How to use it

- **Hover** — a tooltip with the same numbers and the adapter being measured.
- **Left click** — a details panel: adapter, connection, IP, speeds, totals, and
  a button to reset the totals.
- **Middle click** — cycle what the widget shows: both, then speed only, then
  traffic only.
- **Right click** — a menu to switch what is shown, toggle the SPEED / TOTAL
  captions and the arrows, reset the counters, open Windows or Windhawk settings,
  or hide the widget. Every choice is remembered.

## Why it is different

**It counts your VPN correctly.** A VPN adds a second, virtual adapter, so your
traffic passes through two of them and most meters count a 1 GB download as 2 GB.
Worse, some VPN clients (NekoBox among them) inflate their own adapter's counters
— measured at 2.66× the real payload — so a meter that follows the tunnel shows
speeds your line cannot deliver. This mod measures the physical adapter that
actually carries your traffic out, the same one Task Manager and speedtests read,
so nothing is double-counted and the numbers match your real connection. The
totals restart when your internet source changes, because a total from one
connection says nothing about another.

**The numbers are honest.** Speed is measured from Windows' own byte counters,
sampled twice and divided by the exact elapsed time — never estimated, never
parsed from `ipconfig` or PowerShell. It matched Windows' own counters to within
0.06% over 22 seconds.

**You shape it.** Twenty-two arrow designs (or none at all), any installed font,
five weights, a separate colour for download and for upload, a draggable divider,
a resizable widget and panel, and layouts from two columns down to a single line.

## It stays out of the way

One reading per second on a background thread, and it only repaints when a number
on screen actually changes: about 0.3-0.5% of one CPU core and 30 MB of memory,
with no leaks. The look is native — real acrylic, rounded corners, and it follows
your light/dark theme.

## Arabic

Every setting, description and option is translated. Windhawk shows Arabic
automatically when your Windows display language is Arabic.

## Requirements

Windows 11 for the glass and rounded corners (it works on Windows 10 with square
corners), and Windhawk installed. If the Widgets button covers the meter, turn
Widgets off or move the meter with the **X offset** setting.

---

# مؤشر الشبكة لشريط المهام

مؤشر شبكة حيّ على شريط مهام ويندوز 11: سرعة التحميل والرفع في اللحظة الحالية،
وإجمالي ما استهلكته من بيانات — في ودجت زجاجي صغير يبدو جزءًا من ويندوز.

## ما تراه

مستطيل واحد مقسوم إلى نصفين بخط رفيع:

- **SPEED** (يسار) — سرعة التحميل والرفع **الآن**.
- **TOTAL** (يمين) — إجمالي ما نزّلته وما رفعته **كليًّا**.

وفي كل نصف السطر الأعلى للتحميل (سهم أزرق لأسفل) والأسفل للرفع (سهم أخضر لأعلى).
وكل رقم يعرض وحدته بجانبه، فلا يخلط أحد بين `MB/s` و`Mbps`.

## كيف تستخدمه

- **المرور بالماوس** — تلميح بنفس الأرقام مع اسم الكرت المقاس.
- **الزر الأيسر** — لوحة تفاصيل: الكرت، والاتصال، وعنوان IP، والسرعات، والإجماليات،
  وزر لتصفير الإجماليات.
- **الزر الأوسط** — التنقّل بين ما يعرضه الودجت: الاثنان، ثم السرعة فقط، ثم
  الترافيك فقط.
- **الزر الأيمن** — قائمة لتبديل المعروض، وإظهار/إخفاء عنواني SPEED وTOTAL
  والأسهم، وتصفير العدادات، وفتح إعدادات ويندوز أو Windhawk، أو إخفاء الودجت.
  وكل اختيار يُحفَظ.

## ما يميّزه

**يحسب الـVPN بشكل صحيح.** يضيف الـVPN كرتًا ثانيًا وهميًّا، فتمرّ بياناتك عبر
كرتين، ومعظم المؤشرات تحسّب تحميل 1 جيجا على أنه 2 جيجا. والأسوأ أن بعض عملاء
VPN (منهم NekoBox) ينفخون عدادات كرتهم الوهمي — قيس في اختبار مضبوط فبلغ 2.66
ضعف الحجم الحقيقي — فالمؤشر الذي يتبع النفق يعرض سرعات لا تتحملها خطتك. يقيس هذا
المود الكرت الحقيقي الذي يحمل بياناتك فعلًا إلى الخارج، نفس الكرت الذي يقرأه مدير
المهام واختبارات السرعة، فلا يُحسَب شيء مرتين وتطابق الأرقام اتصالك الحقيقي.
وتبدأ الإجماليات من الصفر عند تغيّر مصدر الإنترنت، لأن إجمالي اتصال لا يصف آخر.

**الأرقام صادقة.** تُقاس السرعة من عدّادات بايتات ويندوز نفسها، بقراءتين والقسمة
على الزمن الفاصل بدقّة — بلا تقدير ولا قراءة لمخرجات `ipconfig` أو PowerShell.
وطابقت عدّادات ويندوز بفرق لا يزيد عن 0.06% على مدى 22 ثانية.

**أنت الذي تشكّله.** اثنان وعشرون شكل سهم (أو بلا أسهم)، وأي خط مثبت، وخمسة أوزان،
ولون مستقل للتحميل وآخر للرفع، وخط فاصل قابل للسحب، وودجت ولوحة قابلان لتغيير
الحجم، وتخطيطات من عمودين إلى سطر واحد.

## لا يثقل على جهازك

قراءة واحدة كل ثانية على خيط خلفي، ولا يعيد الرسم إلا عند تغيّر رقم ظاهر فعلًا:
حوالي 0.3-0.5% من نواة معالج واحدة و30 ميجابايت من الذاكرة، وبلا تسريب. والمظهر
أصلي — أكريليك حقيقي وحواف دائرية، ويتبع ثيمك الفاتح/الغامق.

## العربية

كل إعداد ووصف وخيار مترجم. ويعرض Windhawk العربية تلقائيًّا حين تكون لغة عرض
ويندوز عربية.

## المتطلبات

ويندوز 11 للزجاج والحواف الدائرية (ويعمل على ويندوز 10 بحواف قائمة)، مع تثبيت
Windhawk. ولو غطّى زر Widgets المؤشر، فأوقِف Widgets أو حرّكه بإعداد **الإزاحة
الأفقية**.

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- Appearance:
  - PanelWidth: 220
    $name: Panel width
    $description: Width of the widget in pixels (at 100% scaling).
    $name:ar: عرض الودجت
    $description:ar: عرض الودجت بالبكسل (عند تحجيم 100%).
  - PanelHeight: 52
    $name: Panel height
    $description: Height of the widget in pixels (at 100% scaling).
    $name:ar: ارتفاع الودجت
    $description:ar: ارتفاع الودجت بالبكسل (عند تحجيم 100%). زوّده لو عايز أسهم أكبر.
  - FontSize: 13
    $name: Font size
    $description: Size of the numbers. 13 is the clearest; use 11 or 12 for a smaller widget.
    $name:ar: حجم الخط
    $description:ar: حجم الأرقام. القيمة 13 هي الأوضح، واستخدم 11 أو 12 لودجت أصغر.
  - FontFamily: ""
    $name: Font family
    $description: The font used for all text. Leave empty for Segoe UI (recommended). Any installed font works, e.g. Consolas, Arial, Tahoma, Segoe UI Variable Display, Bahnschrift. If the name is wrong or the font is missing, Segoe UI is used automatically.
    $name:ar: نوع الخط
    $description:ar: الخط المستخدم في كل النص. اتركه فارغًا لاستخدام Segoe UI (المُستحسن). أي خط مثبت يعمل، مثل Consolas وArial وTahoma وSegoe UI Variable Display وBahnschrift. ولو كان الاسم خطأ أو الخط غير مثبت، يُستخدم Segoe UI تلقائيًّا.
  - TextWeight: bold
    $name: Text weight
    $options:
    - bold: Bold (sharpest, recommended)
    - black: Black (heaviest - Segoe UI Black)
    - semibold: Semibold
    - regular: Regular
    - italic: Italic
    $name:ar: وزن الخط
    $options:ar:
    - bold: عريض (الأوضح - مُستحسن)
    - black: أسود (الأثقل - Segoe UI Black)
    - semibold: نصف عريض
    - regular: عادي
    - italic: مائل
  - ArrowStyle: rounded
    $name: Arrow style
    $description: The shape of the download / upload arrows - 22 shapes. Pick "No arrows" to hide them completely and keep only the numbers; you can also toggle them from the right click menu without losing your shape choice.
    $options:
    - solid: Solid arrow (stem + head)
    - rounded: Rounded arrow (soft, Fluent-like)
    - chevron: Chevron (thin, modern)
    - triangle: Triangle (compact, no stem)
    - circle: Arrow in a circle (badge)
    - double: Double chevron (stacked)
    - triple: Triple chevron (speed lines)
    - outline: Outline arrow (hollow)
    - thin: Hairline arrow (minimal)
    - fat: Block arrow (wide, bold)
    - head: Arrowhead only (no stem)
    - ring: Chevron in a ring
    - square: Arrow in a rounded square
    - fade: Fading arrow (gradient tail)
    - dashed: Dashed shaft arrow
    - barb: Barbed arrow (broadhead)
    - needle: Needle dart (sleek)
    - dot: Arrow with tail dot
    - feather: Fletched arrow
    - layered: Two-tone layered arrow
    - tray: Arrow into a tray (download/upload icon)
    - plus: Arrow with a plus bar (+)
    - none: No arrows (hide them)
    $name:ar: شكل السهم
    $description:ar: شكل أسهم التحميل والرفع - 22 شكلًا. اختر "بدون أسهم" لإخفائها تمامًا والإبقاء على الأرقام فقط، ويمكنك أيضًا تبديل إظهارها من قائمة الزر الأيمن دون أن يفقدك اختيارك للشكل.
    $options:ar:
    - solid: سهم صلب (ساق + رأس)
    - rounded: سهم دائري الأطراف (ناعم، شبيه بـFluent)
    - chevron: شيفرون (رفيع وعصري)
    - triangle: مثلث (مضغوط، بدون ساق)
    - circle: سهم داخل دائرة (شعار)
    - double: شيفرون مزدوج (مكدّس)
    - triple: شيفرون ثلاثي (خطوط سرعة)
    - outline: سهم مفرّغ (مخطط فقط)
    - thin: سهم خيطي (أدنى حد)
    - fat: سهم كتلي (عريض وبارز)
    - head: رأس سهم فقط (بدون ساق)
    - ring: شيفرون داخل حلقة
    - square: سهم داخل مربع دائري
    - fade: سهم متلاشٍ (تدرّج من الذيل)
    - dashed: سهم بساق متقطّعة
    - barb: سهم بخطّافات (رأس صيد)
    - needle: سهم إبرة (رمحة انسيابية)
    - dot: سهم بنقطة عند الذيل
    - feather: سهم بريش
    - layered: سهم بطبقتين (ظلّ ولون)
    - tray: سهم في درج (أيقونة التحميل/الرفع)
    - plus: سهم بعلامة زائد (+)
    - none: بدون أسهم (إخفاؤها)
  - ArrowScale: 120
    $name: Arrow size (% of font size)
    $description: Arrow size relative to the text. 120 means the arrow is 1.2x as tall as the numbers. It never grows past what fits, so for really big arrows increase Panel height too. Range 60-400.
    $name:ar: حجم السهم (% من حجم الخط)
    $description:ar: حجم السهم بالنسبة للنص. القيمة 120 تعني أن السهم أطول من الأرقام بمقدار 1.2 ضعف. ولن يكبر أبدًا أكثر من المساحة المتاحة، فلو أردت أسهمًا كبيرة فعلًا زوّد ارتفاع الودجت أيضًا. المدى 60-400.
  - ShowColumnLabels: both
    $name: SPEED / TOTAL captions
    $description: The small labels above the two halves. Choose which ones stay visible, or hide both. You can also tick them on and off from the right click menu.
    $options:
    - both: Show both SPEED and TOTAL
    - speed: Show only SPEED
    - total: Show only TOTAL
    - none: Hide both captions
    $name:ar: عنوانا SPEED / TOTAL
    $description:ar: العنوانان الصغيران فوق النصفين. اختر أيهما يبقى ظاهرًا أو أخفِ الاثنين، ويمكنك أيضًا تحديدهما من قائمة الزر الأيمن.
    $options:ar:
    - both: إظهار SPEED وTOTAL معًا
    - speed: إظهار SPEED فقط
    - total: إظهار TOTAL فقط
    - none: إخفاء الاثنين
  - DividerPos: 50
    $name: Divider position (% of width)
    $description: Where the dividing line sits, which also decides how much room each half gets. 50 splits it evenly; a lower number gives the speeds more room. Range 20-80.
    $name:ar: موضع الخط الفاصل (% من العرض)
    $description:ar: مكان الخط الفاصل، وهو أيضًا ما يحدّد المساحة التي يأخذها كل نصف. القيمة 50 تقسّمها بالتساوي، والأقل منها يعطي السرعات مساحة أكبر. المدى 20-80.
  - DividerOpacity: 46
    $name: Divider opacity (0-255)
    $description: How visible the dividing line is. 0 hides it completely, 255 makes it solid.
    $name:ar: شفافية الخط الفاصل (0-255)
    $description:ar: مدى ظهور الخط الفاصل. القيمة 0 تخفيه تمامًا، و255 تجعله صريحًا.
  - DividerDraggable: true
    $name: Drag the divider with the mouse
    $description: Lets you grab the dividing line and drag it left or right. Only the line moves; the numbers stay where they are. To centre it again, right click the widget and pick "Reset divider position".
    $name:ar: تحريك الخط الفاصل بالماوس
    $description:ar: يسمح لك بسحب الخط الفاصل يمينًا أو شمالًا. الخط وحده هو الذي يتحرك، والأرقام تبقى في مكانها. ولإرجاعه للمنتصف اضغط بالزر الأيمن على الودجت واختر "Reset divider position".
  - DetailsWidth: 240
    $name: Details panel width
    $description: Size of the pop-up panel opened by clicking the widget.
    $name:ar: عرض لوحة التفاصيل
    $description:ar: حجم اللوحة التي تفتح عند الضغط على الودجت.
  - DetailsHeight: 276
    $name: Details panel height
    $description: A minimum height, not a fixed one. The panel grows by itself if its contents need more room, so nothing is ever cut off.
    $name:ar: ارتفاع لوحة التفاصيل
    $description:ar: هذا حدّ أدنى للارتفاع لا مقاس ثابت. فاللوحة تكبر من نفسها لو احتاج محتواها مساحة أكبر، فلا يُقطَع أي شيء.
  - LayoutMode: full
    $name: Layout
    $options:
    - full: Speeds + totals (two columns)
    - speeds: Speeds only (two rows)
    - oneline: Speeds only (single line)
    $name:ar: التخطيط
    $options:ar:
    - full: السرعات + الإجماليات (عمودان)
    - speeds: السرعات فقط (سطران)
    - oneline: السرعات فقط (سطر واحد)
  - DisplayMode: both
    $name: Show traffic or speed
    $description: Which half the widget shows. "Both" keeps the two-column layout. You can also switch live from the right click menu (Show both / Show speed only / Show traffic only) or by middle clicking the widget, which cycles through the three. The widget remembers that choice across restarts; changing this setting applies its value again.
    $options:
    - both: Show both speed and traffic
    - speed: Show speed only
    - traffic: Show traffic (totals) only
    $name:ar: عرض الترافيك أو السبيد
    $description:ar: أي جانب يعرضه الودجت. "الاثنان" يبقي تخطيط العمودين. ويمكنك التبديل فورًا من قائمة الزر الأيمن (عرض الاثنين / السرعة فقط / الترافيك فقط) أو بالضغط الأوسط على الودجت للتنقّل بين الأشكال الثلاثة، والودجت يتذكر اختيارك بعد إعادة التشغيل. وتغيير هذا الإعداد يعيد تطبيق قيمته.
    $options:ar:
    - both: عرض السرعة والترافيك معًا
    - speed: عرض السرعة فقط
    - traffic: عرض الترافيك (الإجماليات) فقط
  - CycleOnMiddleClick: true
    $name: Middle click switches speed / traffic
    $description: Press the mouse wheel (middle click) on the widget to cycle through both, speed only, and traffic only.
    $name:ar: التبديل بالضغط الأوسط
    $description:ar: اضغط بعجلة الماوس (الزر الأوسط) على الودجت للتنقّل بين عرض الاثنين ثم السرعة فقط ثم الترافيك فقط.
  - OffsetX: 12
    $name: X offset
    $description: How far along the taskbar the widget sits, measured from the left edge. Increase it to move the widget right, e.g. to get out from under the Widgets button. (Measured from the top edge if your taskbar is vertical.)
    $name:ar: الإزاحة الأفقية
    $description:ar: مكان الودجت على شريط المهام، مقيسًا من الحد الأيسر. زوّده لتحريك الودجت يمينًا، مثلًا للخروج من تحت زر Widgets. (ويُقاس من الحد الأعلى لو كان شريط المهام رأسيًّا.)
  - OffsetY: 0
    $name: Y offset
    $name:ar: الإزاحة الرأسية
  - DpiScaling: true
    $name: Scale with DPI
    $description: Keep the widget the same apparent size when Windows display scaling is not 100%. Leave this on unless you want exact pixel sizes.
    $name:ar: التحجيم مع تكبير الشاشة
    $description:ar: يبقي الودجت بنفس الحجم المرئي حين لا يكون تكبير شاشة ويندوز 100%. اتركه مُفعّلًا إلا لو أردت مقاسات بالبكسل بالضبط.
  - AutoTheme: true
    $name: Auto theme
    $description: Pick the text colour automatically from your Windows light/dark theme. Turn this off to choose the colour yourself below.
    $name:ar: الثيم التلقائي
    $description:ar: يختار لون النص تلقائيًّا من ثيم ويندوز الفاتح/الغامق. أوقِفه لتختار اللون بنفسك بالأسفل.
  - TextColor: "0xFFFFFF"
    $name: Manual text color (hex)
    $description: Only used when Auto theme is off. Write it as 0xRRGGBB, e.g. 0xFFFFFF for white or 0xFF6060 for red.
    $name:ar: لون النص اليدوي (hex)
    $description:ar: يُستخدم فقط عند إيقاف الثيم التلقائي. اكتبه بالصيغة 0xRRGGBB، مثل 0xFFFFFF للأبيض أو 0xFF6060 للأحمر.
  - DownloadColor: ""
    $name: Download color (hex)
    $description: A custom colour for the download half - its arrow AND its numbers. Leave empty (or write auto) to keep the default blue accent. Write it as 0xRRGGBB, e.g. 0xFF4D4D for red.
    $name:ar: لون التحميل (hex)
    $description:ar: لون مخصص لجانب التحميل — سهمه وأرقامه معًا. اتركه فارغًا (أو اكتب auto) للإبقاء على الأزرق الافتراضي. اكتبه بالصيغة 0xRRGGBB، مثل 0xFF4D4D للأحمر.
  - UploadColor: ""
    $name: Upload color (hex)
    $description: A custom colour for the upload half - its arrow AND its numbers. Leave empty (or write auto) to keep the default green accent. Write it as 0xRRGGBB, e.g. 0x66CCFF for sky blue.
    $name:ar: لون الرفع (hex)
    $description:ar: لون مخصص لجانب الرفع — سهمه وأرقامه معًا. اتركه فارغًا (أو اكتب auto) للإبقاء على الأخضر الافتراضي. اكتبه بالصيغة 0xRRGGBB، مثل 0x66CCFF للأزرق السماوي.
  - ColorArrows: true
    $name: Colored arrows
    $description: Blue arrow for download, green for upload. Turn this off to draw both in the text colour. The Download / Upload color settings below override this for one direction at a time.
    $name:ar: أسهم ملوّنة
    $description:ar: سهم أزرق للتحميل وأخضر للرفع. أوقِفه لرسم الاثنين بلون النص. وإعدادا لون التحميل ولون الرفع بالأسفل يستبدلان هذا اللون لاتجاه واحد.
  - BgOpacity: 0
    $name: Acrylic tint opacity (0-255)
    $description: Adds a solid tint behind the glass. Keep it at 0 for clear glass; raise it if your wallpaper makes the numbers hard to read.
    $name:ar: تعتيم الخلفية الزجاجية (0-255)
    $description:ar: يضيف طبقة لون خلف الزجاج. اتركه 0 لزجاج صافٍ، وزوّده لو كانت خلفية سطح المكتب تُصعّب قراءة الأرقام.
  $name: Appearance
  $name:ar: المظهر
- Network:
  - InterfaceMode: auto
    $name: Interface mode
    $options:
    - auto: Auto (adapter used for the internet)
    - ethernet: Ethernet only
    - wifi: Wi-Fi only
    - all: All active interfaces
    - specific: Specific interface (below)
    $name:ar: اختيار كرت الشبكة
    $options:ar:
    - auto: تلقائي (الكرت المستخدم للإنترنت فعلًا)
    - ethernet: إيثرنت فقط
    - wifi: واي فاي فقط
    - all: كل الكروت النشطة
    - specific: كرت محدد (بالأسفل)
  - SelectedInterface: ""
    $name: Specific interface
    $description: Only used when Interface mode is set to "Specific". Type the adapter's name or any part of it, e.g. "Ethernet" or "Realtek".
    $name:ar: الكرت المحدد
    $description:ar: يُستخدم فقط عند اختيار "كرت محدد" في اختيار كرت الشبكة. اكتب اسم الكرت أو أي جزء منه، مثل "Ethernet" أو "Realtek".
  - ExcludeVirtual: true
    $name: Ignore virtual adapters
    $description: Ignores adapters that are not real hardware - VPN tunnels, VMware, Hyper-V, Docker, TAP, loopback. In Auto mode the meter always reads the physical adapter carrying your traffic, never a virtual tunnel, so this mainly matters for the other modes.
    $name:ar: تجاهل الكروت الوهمية
    $description:ar: يتجاهل الكروت التي ليست عتادًا حقيقيًّا - أنفاق VPN وVMware وHyper-V وDocker وTAP وloopback. وفي الوضع التلقائي يقيس المؤشر دائمًا الكرت الحقيقي الحامل للبيانات وليس أي نفق وهمي، لذا يهمّ هذا الإعداد أساسًا في الأوضاع الأخرى.
  - ResetOnSourceChange: true
    $name: Reset counters when the internet source changes
    $description: On - the totals start from zero every time your internet source changes (VPN on or off, Wi-Fi to Ethernet), so they always describe the connection you are on right now. Off - one running total is kept across every connection.
    $name:ar: تصفير العدادات عند تغيّر مصدر الإنترنت
    $description:ar: مُفعّل - تبدأ الإجماليات من الصفر كل مرة يتغيّر مصدر الإنترنت (تشغيل VPN أو إيقافه، أو الانتقال من واي فاي إلى إيثرنت)، فتصف دائمًا الاتصال الذي تستخدمه الآن. مُعطّل - يُحفَظ إجماليّ واحد متراكم عبر كل الاتصالات.
  - UpdateInterval: 1000
    $name: Update interval (ms)
    $description: How often the numbers refresh. 1000 means once a second, which is the usual choice. Lower is more responsive but slightly busier. Range 250-5000.
    $name:ar: زمن التحديث (ملي ثانية)
    $description:ar: كل كم يتم تحديث الأرقام. القيمة 1000 تعني مرة كل ثانية وهي الاختيار المعتاد. والأقل أسرع استجابة لكنه أكثر انشغالًا قليلًا. المدى من 250 إلى 5000.
  - SpeedUnit: auto
    $name: Speed unit
    $options:
    - auto: Auto (bytes per second, e.g. 12.4 MB/s)
    - bytes: Bytes per second (MB/s)
    - bits: Bits per second (Mbps)
    $name:ar: وحدة السرعة
    $options:ar:
    - auto: تلقائي (بايت/ثانية، مثل 12.4 MB/s)
    - bytes: بايت في الثانية (MB/s)
    - bits: بِت في الثانية (Mbps)
  - BinaryUnits: true
    $name: Use 1024-based byte units
    $description: On - 1 MB counts as 1048576 bytes, the same way File Explorer shows file sizes. Off - 1 MB counts as 1000000 bytes, the way ISPs advertise. Bit units (Mbps) always use 1000.
    $name:ar: وحدات بايت بأساس 1024
    $description:ar: مُفعّل - الميجابايت = 1048576 بايت، بنفس طريقة عرض مستكشف الملفات لأحجام الملفات. مُعطّل - الميجابايت = 1000000 بايت، بالطريقة التي تعلن بها شركات الإنترنت. ووحدات البِت (Mbps) تستخدم 1000 دائمًا.
  $name: Network
  $name:ar: الشبكة
- Traffic:
  - CounterMode: session
    $name: Traffic counter mode
    $options:
    - session: This session only (starts from zero each time)
    - persistent: Keep counting (survives restarts)
    $name:ar: طريقة عدّ الترافيك
    $options:ar:
    - session: هذه الجلسة فقط (يبدأ من الصفر كل مرة)
    - persistent: عدّ متواصل (يصمد بعد إعادة التشغيل)
  - ResetCounters: none
    $name: Reset traffic counters
    $description: Pick what to zero and press Save. It happens once, so you can leave the choice here afterwards. You can also reset from the button in the details panel or from the right click menu.
    $options:
    - none: Do not reset
    - download: Reset download
    - upload: Reset upload
    - both: Reset both
    $name:ar: تصفير عدادات الترافيك
    $description:ar: اختر ما تريد تصفيره واضغط Save. يحدث هذا مرة واحدة، فيمكنك ترك الاختيار كما هو بعدها. ويمكنك التصفير أيضًا من الزر في لوحة التفاصيل أو من قائمة الزر الأيمن.
    $options:ar:
    - none: لا تصفير
    - download: تصفير التحميل
    - upload: تصفير الرفع
    - both: تصفير الاثنين
  $name: Traffic
  $name:ar: الترافيك
- Behavior:
  - ShowTooltip: true
    $name: Show tooltip on hover
    $name:ar: إظهار تلميح عند المرور بالماوس
  - ShowDetailsOnClick: true
    $name: Show details panel on click
    $name:ar: إظهار لوحة التفاصيل عند الضغط
  - HideFullscreen: false
    $name: Hide when fullscreen
    $name:ar: الإخفاء في وضع ملء الشاشة
  - StartEnabled: true
    $name: Start enabled
    $description: Turn this off to hide the widget without removing the mod. Also how you bring it back after using "Hide widget" in the right click menu.
    $name:ar: يبدأ مُفعّلًا
    $description:ar: أوقِفه لإخفاء الودجت دون إزالة المود. وهو أيضًا الطريقة لإرجاعه بعد استخدام "Hide widget" من قائمة الزر الأيمن.
  $name: Behavior
  $name:ar: السلوك
*/
// ==/WindhawkModSettings==
