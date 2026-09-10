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
// ---------------------------------------------------------------------------
// Taskbar Network Lounge - implementation
//
// Design notes:
//  * This is a Windhawk *tool mod* that owns a native layered popup window docked
//    over Shell_TrayWnd, drawn with GDI+, acrylic via
//    SetWindowCompositionAttribute, rounded via DWM. That window/z-band/acrylic
//    approach follows what taskbar-music-lounge and net-speed-taskbar do; both
//    were read as references while building this, no code was taken from either.
//  * The network measurement, adapter selection, traffic totals and details panel
//    are specific to this mod.
//  * winsock2.h MUST be included before iphlpapi.h/netioapi.h, otherwise
//    mingw-w64 hides MIB_IF_ROW2 / GetIfTable2 (they are only declared when
//    _WS2IPDEF_ is defined).
// ---------------------------------------------------------------------------

// Windhawk force-includes windhawk_api.h (which pulls in windows.h) before this
// file, so winsock2.h emits its "include me before windows.h" warning. The
// declarations we need are unaffected, so the warning is silenced locally.
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-W#warnings"
#include <winsock2.h>
#pragma clang diagnostic pop
#include <ws2ipdef.h>
#include <iphlpapi.h>
#include <netioapi.h>

#include <windows.h>
#include <commctrl.h>
#include <dwmapi.h>
#include <gdiplus.h>
#include <shellapi.h>
#include <shlobj.h>
#include <windowsx.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <climits>
#include <cstdio>
#include <cwctype>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

using namespace Gdiplus;

// --- Constants -------------------------------------------------------------
static const WCHAR* kFontName = L"Segoe UI";
static const WCHAR* kWidgetClass = L"WindhawkNetworkLoungeWidget";
static const WCHAR* kPanelClass = L"WindhawkNetworkLoungePanel";

// Timers / messages
#define IDT_WATCHDOG 1001
#define IDT_ANIM 1002
// Polls for a click outside the details panel. SetForegroundWindow is routinely
// refused for a process that isn't already in the foreground (the widget is
// WS_EX_NOACTIVATE, so clicking it grants no foreground rights), and without
// activation the panel never gets WM_ACTIVATE/WA_INACTIVE - so "click elsewhere to
// dismiss" cannot rely on activation alone.
#define IDT_PANEL_DISMISS 1003
#define APP_WM_CLOSE (WM_APP + 0)
#define APP_WM_REPOSITION (WM_APP + 10)
#define APP_WM_DATA_UPDATED (WM_APP + 11)
#define APP_WM_TOGGLE_PANEL (WM_APP + 12)
// Own message for "settings changed", rather than synthesizing a system
// WM_SETTINGCHANGE broadcast whose wParam/lParam would then have to be ignored.
#define APP_WM_SETTINGS_CHANGED (WM_APP + 13)

// Context menu ids
#define IDM_REFRESH 100
#define IDM_RESET_DL 101
#define IDM_RESET_UL 102
#define IDM_RESET_BOTH 103
#define IDM_NET_SETTINGS 104
#define IDM_WH_SETTINGS 105
#define IDM_HIDE 106
#define IDM_RESET_DIVIDER 107
#define IDM_SHOW_BOTH 108
#define IDM_SHOW_SPEED 109
#define IDM_SHOW_TRAFFIC 110
#define IDM_TOGGLE_CAPTION_SPEED 111
#define IDM_TOGGLE_CAPTION_TOTAL 112
#define IDM_TOGGLE_ARROWS 113

// mingw-w64's winuser.h predates the newer menu check flags; define the ones
// used by the display toggles (values from the Windows SDK).
#ifndef MF_RADIOCHECK
#define MF_RADIOCHECK 0x00000400U
#endif
#ifndef MF_GROUPCHECK
#define MF_GROUPCHECK 0x00000200U
#endif

// --- Undocumented composition / z-band APIs (same as the reference mod) ----
typedef enum _WINDOWCOMPOSITIONATTRIB { WCA_ACCENT_POLICY = 19 } WINDOWCOMPOSITIONATTRIB;
typedef enum _ACCENT_STATE {
    ACCENT_DISABLED = 0,
    ACCENT_ENABLE_BLURBEHIND = 3,
    ACCENT_ENABLE_ACRYLICBLURBEHIND = 4,
    ACCENT_INVALID_STATE = 5
} ACCENT_STATE;
typedef struct _ACCENT_POLICY {
    ACCENT_STATE AccentState;
    DWORD AccentFlags;
    DWORD GradientColor;
    DWORD AnimationId;
} ACCENT_POLICY;
typedef struct _WINDOWCOMPOSITIONATTRIBDATA {
    WINDOWCOMPOSITIONATTRIB Attribute;
    PVOID Data;
    SIZE_T SizeOfData;
} WINDOWCOMPOSITIONATTRIBDATA;
typedef BOOL(WINAPI* pSetWindowCompositionAttribute)(HWND, WINDOWCOMPOSITIONATTRIBDATA*);

enum ZBID {
    ZBID_DEFAULT = 0,
    ZBID_DESKTOP = 1,
    ZBID_UIACCESS = 2,
    ZBID_IMMERSIVE_IHM = 3,
    ZBID_IMMERSIVE_NOTIFICATION = 4,
    ZBID_IMMERSIVE_APPCHROME = 5,
    ZBID_IMMERSIVE_MOGO = 6,
    ZBID_IMMERSIVE_EDGY = 7,
    ZBID_IMMERSIVE_INACTIVEMOBODY = 8,
    ZBID_IMMERSIVE_INACTIVEDOCK = 9,
    ZBID_IMMERSIVE_ACTIVEMOBODY = 10,
    ZBID_IMMERSIVE_ACTIVEDOCK = 11,
    ZBID_IMMERSIVE_BACKGROUND = 12,
    ZBID_IMMERSIVE_SEARCH = 13,
    ZBID_GENUINE_WINDOWS = 14,
    ZBID_IMMERSIVE_RESTRICTED = 15,
    ZBID_SYSTEM_TOOLS = 16,
    ZBID_LOCK = 17,
    ZBID_ABOVELOCK_UX = 18,
};

typedef HWND(WINAPI* pCreateWindowInBand)(DWORD dwExStyle,
                                          LPCWSTR lpClassName,
                                          LPCWSTR lpWindowName,
                                          DWORD dwStyle,
                                          int x,
                                          int y,
                                          int nWidth,
                                          int nHeight,
                                          HWND hWndParent,
                                          HMENU hMenu,
                                          HINSTANCE hInstance,
                                          LPVOID lpParam,
                                          DWORD dwBand);

// --- Settings --------------------------------------------------------------
enum class InterfaceMode { Auto, Ethernet, Wifi, All, Specific };
enum class SpeedUnit { Auto, Bytes, Bits };
enum class CounterMode { Session, Persistent };
enum class LayoutMode { Full, Speeds, OneLine };
enum class ResetRequest { None, Download, Upload, Both };

enum class ArrowStyle {
    Solid, Chevron, Triangle, Rounded, Circle,
    Double, Triple, Outline, Thin, Fat, Head, Ring, Square, Fade, Dashed,
    Barb, Needle, Dot, Feather, Layered, Tray, Plus, None };

// Which of the SPEED / TOTAL captions are painted above the two columns.
enum class CaptionMode { Both, SpeedOnly, TotalOnly, None };

enum class TextWeight { Regular, Italic, Semibold, Bold, Black };

struct ModSettings {
    // Appearance
    int width = 220;
    int height = 52;
    int fontSize = 13;
    std::wstring fontFamily = L"Segoe UI";
    TextWeight textWeight = TextWeight::Bold;
    ArrowStyle arrowStyle = ArrowStyle::Rounded;
    int arrowScale = 120;       // percent of font size
    CaptionMode captions = CaptionMode::Both;
    int dividerPos = 50;        // percent of the usable width
    int dividerOffset = 0;      // px nudge on top of dividerPos, set by dragging
    bool dividerDraggable = true;
    int dividerOpacity = 46;    // 0 = hidden
    int detailsWidth = 240;
    int detailsHeight = 276;
    LayoutMode layout = LayoutMode::Full;
    // Which half the widget shows: both, speeds only, or totals only. The
    // right-click menu cycles through these live; the setting is the default.
    LayoutMode display = LayoutMode::Full;
    bool cycleOnMiddleClick = true;
    int offsetX = 12;
    int offsetY = 0;
    bool dpiScaling = true;
    bool autoTheme = true;
    DWORD manualTextColor = 0xFFFFFFFF;
    // Per-direction colours (0 = not set, use the theme accent). Applied to the
    // arrow AND the numbers of that half, so "download is blue" stays true even
    // after the user recolours it.
    DWORD downloadColor = 0;
    DWORD uploadColor = 0;
    bool colorArrows = true;
    int bgOpacity = 0;
    // Network
    InterfaceMode ifMode = InterfaceMode::Auto;
    std::wstring selectedInterface;
    bool excludeVirtual = true;
    bool resetOnSourceChange = true;
    int updateInterval = 1000;
    SpeedUnit speedUnit = SpeedUnit::Auto;
    bool binaryUnits = true;
    // Traffic
    CounterMode counterMode = CounterMode::Session;
    ResetRequest resetRequest = ResetRequest::None;
    // Behavior
    bool showTooltip = true;
    bool showDetails = true;
    bool hideFullscreen = false;
    bool startEnabled = true;
};

static ModSettings g_settings;
static std::mutex g_settingsMutex;  // guards g_settings against UI/worker races

// --- Shared network state --------------------------------------------------
struct NetSnapshot {
    double downBytesPerSec = 0.0;
    double upBytesPerSec = 0.0;
    unsigned long long totalDown = 0;
    unsigned long long totalUp = 0;
    std::wstring ifName = L"Detecting...";
    std::wstring ifKind = L"";  // Ethernet / Wi-Fi / ...
    std::wstring ipv4 = L"";
    bool connected = false;
    bool haveInterface = false;
    int ifCount = 0;
};

static NetSnapshot g_snapshot;
static std::mutex g_snapshotMutex;

// Windows / threads
static HWND g_hWidget = nullptr;
static HWND g_hPanel = nullptr;
static HWND g_hTooltip = nullptr;
static std::thread* g_uiThread = nullptr;
static std::thread* g_workerThread = nullptr;
// The UI thread's own id, published before it creates any window. Shutdown posts
// WM_QUIT to the THREAD, not to a window: if uninit runs before the window exists
// (mod disabled or reloaded right after start), a window-targeted message would go
// nowhere and join() would block forever.
static std::atomic<DWORD> g_uiThreadId{0};
static HANDLE g_stopEvent = nullptr;   // manual reset: shut worker down
static HANDLE g_wakeEvent = nullptr;   // auto reset: sample immediately
static std::atomic<bool> g_panelVisible{false};
static std::atomic<unsigned long long> g_panelHiddenAt{0};  // GetTickCount64
// Hover state is split per window so moving the mouse over one cannot clear the
// other's highlight.
// Widget: 0 = none, 1 = body, 2 = divider grab zone.
static std::atomic<int> g_widgetHover{0};
// Details panel: 0 = none, 1 = Reset button.
static std::atomic<int> g_panelHover{0};
static std::atomic<bool> g_hiddenByUser{false};
static std::atomic<unsigned long long> g_pendingReset{0};  // bit0 dl, bit1 ul
// Divider drag state. g_dividerHitX is written by the painter every frame so the
// hit test always matches the line that is actually on screen, whatever the
// layout/DPI. g_dividerOffset is the user's dragged offset in *logical* pixels
// (it is scaled at paint time), kept live here and persisted on mouse-up.
static std::atomic<float> g_dividerHitX{-1.0f};
static std::atomic<int> g_dividerOffset{0};
static std::atomic<bool> g_dividerDragging{false};
// Live UI state the settings dialog cannot express as a quick toggle: which half
// is shown, which captions are visible, and whether the arrows are hidden. Each
// is toggled from the right-click menu (and the display mode also by middle
// click), persisted in the mod's own storage, and seeded at init. The matching
// settings act as defaults that are (re)applied only when the user actually
// changes them in the settings UI - see the LastApplied checks in LoadSettings -
// so a menu toggle is never stomped by an unrelated settings reload.
//   g_displayMode:  0 = both, 1 = speeds only, 2 = totals only
//   g_captionBits:  bit0 = SPEED visible, bit1 = TOTAL visible
//   g_arrowStyle:   the arrow *shape* (never None)
//   g_arrowsHidden: 1 = draw no arrows regardless of shape
static std::atomic<int> g_displayMode{0};
static std::atomic<int> g_captionBits{3};
static std::atomic<int> g_arrowStyle{(int)ArrowStyle::Rounded};
static std::atomic<int> g_arrowsHidden{0};
static UINT g_taskbarCreatedMsg = 0;
static HWINEVENTHOOK g_taskbarHook = nullptr;
static HWND g_hookedTaskbar = nullptr;
static ULONG_PTR g_gdiplusToken = 0;
// --- Small helpers ---------------------------------------------------------
static std::wstring ToLower(std::wstring s) {
    for (auto& c : s) {
        c = (wchar_t)towlower(c);
    }
    return s;
}

static bool Contains(const std::wstring& haystackLower, const wchar_t* needleLower) {
    return haystackLower.find(needleLower) != std::wstring::npos;
}

static int ClampInt(int v, int lo, int hi) {
    return v < lo ? lo : (v > hi ? hi : v);
}

// --- Settings --------------------------------------------------------------
static std::wstring ReadStringSetting(PCWSTR name) {
    std::wstring result;
    PCWSTR value = Wh_GetStringSetting(name);
    if (value) {
        result = value;
        Wh_FreeStringSetting(value);
    }
    return result;
}

static bool StringSettingIs(const std::wstring& value, const wchar_t* expected) {
    return _wcsicmp(value.c_str(), expected) == 0;
}

// Parse "0xRRGGBB", "#RRGGBB" or "RRGGBB" into an RGB DWORD. Returns fallback on
// empty/invalid input.
static DWORD ParseHexColor(const std::wstring& text, DWORD fallback) {
    if (text.empty()) {
        return fallback;
    }
    const wchar_t* p = text.c_str();
    while (*p == L' ' || *p == L'\t') {
        p++;
    }
    if (*p == L'#') {
        p++;
    } else if (p[0] == L'0' && (p[1] == L'x' || p[1] == L'X')) {
        p += 2;
    }
    wchar_t* end = nullptr;
    unsigned long parsed = wcstoul(p, &end, 16);
    if (end == p) {
        return fallback;
    }
    return (DWORD)(parsed & 0xFFFFFF);
}

// Like ParseHexColor but treats empty / "auto" / "default" as "not set" (0), so
// the caller can tell "user picked a colour" from "leave it to the theme".
static DWORD ParseHexColorOrAuto(const std::wstring& text) {
    if (text.empty() || StringSettingIs(text, L"auto") ||
        StringSettingIs(text, L"default")) {
        return 0;
    }
    return 0xFF000000 | ParseHexColor(text, 0);
}

// The right-click menu toggles a few things the settings YAML also exposes
// (display mode, captions, arrows). Those live toggles are the source of truth
// while the mod runs; the settings act as defaults that are applied only when
// their value actually changes (or the first time they are ever seen). Without
// this, any unrelated LoadSettings() - a divider drag ending, saving a different
// setting - would stomp a mode the user picked from the menu. The last applied
// value is remembered in the mod's own storage, same trick as the reset
// dropdown. Returns true when the caller should apply the new setting value.
static bool SettingChangedSinceLastApply(PCWSTR settingName,
                                         PCWSTR lastAppliedKey,
                                         std::wstring* currentOut) {
    std::wstring current = ReadStringSetting(settingName);
    WCHAR previous[64] = {};
    Wh_GetStringValue(lastAppliedKey, previous, ARRAYSIZE(previous));
    const bool firstTime = previous[0] == L'\0';
    const bool changed =
        !firstTime && _wcsicmp(previous, current.c_str()) != 0;
    if (firstTime || changed) {
        Wh_SetStringValue(lastAppliedKey, current.c_str());
    }
    *currentOut = std::move(current);
    return firstTime || changed;
}

static void LoadSettings() {
    ModSettings s;

    s.width = ClampInt(Wh_GetIntSetting(L"Appearance.PanelWidth"), 80, 1200);
    s.height = ClampInt(Wh_GetIntSetting(L"Appearance.PanelHeight"), 20, 200);
    s.fontSize = ClampInt(Wh_GetIntSetting(L"Appearance.FontSize"), 6, 40);

    s.fontFamily = ReadStringSetting(L"Appearance.FontFamily");
    // Trim: a stray space makes GDI+ fail to find the family.
    while (!s.fontFamily.empty() && (s.fontFamily.back() == L' ' || s.fontFamily.front() == L' ')) {
        if (s.fontFamily.back() == L' ') {
            s.fontFamily.pop_back();
        } else {
            s.fontFamily.erase(0, 1);
        }
    }

    std::wstring weight = ReadStringSetting(L"Appearance.TextWeight");
    if (StringSettingIs(weight, L"black")) {
        s.textWeight = TextWeight::Black;
    } else if (StringSettingIs(weight, L"semibold")) {
        s.textWeight = TextWeight::Semibold;
    } else if (StringSettingIs(weight, L"regular")) {
        s.textWeight = TextWeight::Regular;
    } else if (StringSettingIs(weight, L"italic")) {
        s.textWeight = TextWeight::Italic;
    } else {
        s.textWeight = TextWeight::Bold;
    }

    // Arrow style: table-driven so the YAML option strings and the enum stay in
    // one place. Unknown values fall back to Solid, as before. "none" is the
    // hide-arrows choice: it sets the hidden flag but keeps the last shape, so
    // showing them again restores what the user had.
    static const struct { const wchar_t* name; ArrowStyle style; } kArrowStyles[] = {
        {L"solid", ArrowStyle::Solid},       {L"chevron", ArrowStyle::Chevron},
        {L"triangle", ArrowStyle::Triangle}, {L"rounded", ArrowStyle::Rounded},
        {L"circle", ArrowStyle::Circle},     {L"double", ArrowStyle::Double},
        {L"triple", ArrowStyle::Triple},     {L"outline", ArrowStyle::Outline},
        {L"thin", ArrowStyle::Thin},         {L"fat", ArrowStyle::Fat},
        {L"head", ArrowStyle::Head},         {L"ring", ArrowStyle::Ring},
        {L"square", ArrowStyle::Square},     {L"fade", ArrowStyle::Fade},
        {L"dashed", ArrowStyle::Dashed},     {L"barb", ArrowStyle::Barb},
        {L"needle", ArrowStyle::Needle},     {L"dot", ArrowStyle::Dot},
        {L"feather", ArrowStyle::Feather},   {L"layered", ArrowStyle::Layered},   {L"tray", ArrowStyle::Tray},
        {L"plus", ArrowStyle::Plus},
        {L"none", ArrowStyle::None},
    };
    std::wstring arrow;
    if (SettingChangedSinceLastApply(L"Appearance.ArrowStyle",
                                     L"LastArrowStyleApplied", &arrow)) {
        for (const auto& entry : kArrowStyles) {
            if (StringSettingIs(arrow, entry.name)) {
                if (entry.style == ArrowStyle::None) {
                    g_arrowsHidden = 1;
                } else {
                    g_arrowStyle = (int)entry.style;
                    g_arrowsHidden = 0;
                }
                Wh_SetIntValue(L"ArrowStyle", g_arrowStyle.load());
                Wh_SetIntValue(L"ArrowsHidden", g_arrowsHidden.load());
                break;
            }
        }
    }
    s.arrowStyle = g_arrowsHidden.load()
                       ? ArrowStyle::None
                       : (ArrowStyle)ClampInt(g_arrowStyle.load(), 0,
                                              (int)ArrowStyle::Plus);

    s.arrowScale = ClampInt(Wh_GetIntSetting(L"Appearance.ArrowScale"), 60, 400);
    std::wstring captions;
    if (SettingChangedSinceLastApply(L"Appearance.ShowColumnLabels",
                                     L"LastCaptionsApplied", &captions)) {
        int bits = 3;
        if (StringSettingIs(captions, L"speed")) {
            bits = 1;
        } else if (StringSettingIs(captions, L"total")) {
            bits = 2;
        } else if (StringSettingIs(captions, L"none")) {
            bits = 0;
        }
        g_captionBits = bits;
        Wh_SetIntValue(L"CaptionBits", bits);
    }
    const int captionBits = g_captionBits.load();
    s.captions = captionBits == 3   ? CaptionMode::Both
                 : captionBits == 1 ? CaptionMode::SpeedOnly
                 : captionBits == 2 ? CaptionMode::TotalOnly
                                    : CaptionMode::None;
    s.dividerPos = ClampInt(Wh_GetIntSetting(L"Appearance.DividerPos"), 20, 80);
    // The dragged offset lives in the mod's own storage, not in the settings YAML:
    // Windhawk's settings UI has no draggable control, and writing a setting from
    // the mod would fight the UI. The live value is in g_dividerOffset; this read
    // only seeds it once so a drag survives a restart.
    s.dividerDraggable =
        Wh_GetIntSetting(L"Appearance.DividerDraggable") != 0;
    if (!s.dividerDraggable) {
        g_dividerOffset = 0;  // going back to a fixed divider discards the drag
        Wh_SetIntValue(L"DividerOffset", 0);
    }
    s.dividerOffset = g_dividerOffset.load();
    s.dividerOpacity =
        ClampInt(Wh_GetIntSetting(L"Appearance.DividerOpacity"), 0, 255);
    s.detailsWidth = ClampInt(Wh_GetIntSetting(L"Appearance.DetailsWidth"), 140, 900);
    s.detailsHeight =
        ClampInt(Wh_GetIntSetting(L"Appearance.DetailsHeight"), 140, 900);

    std::wstring layout = ReadStringSetting(L"Appearance.LayoutMode");
    if (StringSettingIs(layout, L"speeds")) {
        s.layout = LayoutMode::Speeds;
    } else if (StringSettingIs(layout, L"oneline")) {
        s.layout = LayoutMode::OneLine;
    } else {
        s.layout = LayoutMode::Full;
    }

    // Display mode: same live-toggle model as arrows and captions - the setting
    // is applied only when it actually changes (or on first run), so a mode
    // picked from the right-click menu survives unrelated LoadSettings() calls
    // (e.g. a divider drag ending).
    std::wstring display;
    if (SettingChangedSinceLastApply(L"Appearance.DisplayMode",
                                     L"LastDisplayModeApplied", &display)) {
        int modeDefault = StringSettingIs(display, L"speed")
                              ? 1
                              : (StringSettingIs(display, L"traffic") ? 2 : 0);
        g_displayMode = modeDefault;
        Wh_SetIntValue(L"DisplayMode", modeDefault);
    }
    const int displayMode = ClampInt(g_displayMode.load(), 0, 2);
    s.display = displayMode == 1 ? LayoutMode::Speeds
                                 : (displayMode == 2 ? LayoutMode::OneLine
                                                     : LayoutMode::Full);
    s.cycleOnMiddleClick =
        Wh_GetIntSetting(L"Appearance.CycleOnMiddleClick") != 0;

    s.offsetX = ClampInt(Wh_GetIntSetting(L"Appearance.OffsetX"), -4000, 4000);
    s.offsetY = ClampInt(Wh_GetIntSetting(L"Appearance.OffsetY"), -4000, 4000);
    s.dpiScaling = Wh_GetIntSetting(L"Appearance.DpiScaling") != 0;
    s.autoTheme = Wh_GetIntSetting(L"Appearance.AutoTheme") != 0;

    s.manualTextColor = 0xFF000000 | ParseHexColor(
                            ReadStringSetting(L"Appearance.TextColor"), 0xFFFFFF);

    // Per-direction colours: empty / "auto" means "not set" (0) so the painter
    // keeps the theme accent; any hex recolours that half (arrow + numbers).
    s.downloadColor = ParseHexColorOrAuto(
        ReadStringSetting(L"Appearance.DownloadColor"));
    s.uploadColor =
        ParseHexColorOrAuto(ReadStringSetting(L"Appearance.UploadColor"));

    s.colorArrows = Wh_GetIntSetting(L"Appearance.ColorArrows") != 0;
    s.bgOpacity = ClampInt(Wh_GetIntSetting(L"Appearance.BgOpacity"), 0, 255);

    std::wstring mode = ReadStringSetting(L"Network.InterfaceMode");
    if (StringSettingIs(mode, L"ethernet")) {
        s.ifMode = InterfaceMode::Ethernet;
    } else if (StringSettingIs(mode, L"wifi")) {
        s.ifMode = InterfaceMode::Wifi;
    } else if (StringSettingIs(mode, L"all")) {
        s.ifMode = InterfaceMode::All;
    } else if (StringSettingIs(mode, L"specific")) {
        s.ifMode = InterfaceMode::Specific;
    } else {
        s.ifMode = InterfaceMode::Auto;
    }

    s.selectedInterface = ReadStringSetting(L"Network.SelectedInterface");
    s.excludeVirtual = Wh_GetIntSetting(L"Network.ExcludeVirtual") != 0;
    s.resetOnSourceChange =
        Wh_GetIntSetting(L"Network.ResetOnSourceChange") != 0;
    s.updateInterval = ClampInt(Wh_GetIntSetting(L"Network.UpdateInterval"), 250, 5000);

    std::wstring unit = ReadStringSetting(L"Network.SpeedUnit");
    if (StringSettingIs(unit, L"bits")) {
        s.speedUnit = SpeedUnit::Bits;
    } else if (StringSettingIs(unit, L"bytes")) {
        s.speedUnit = SpeedUnit::Bytes;
    } else {
        s.speedUnit = SpeedUnit::Auto;  // Auto == bytes per second
    }
    s.binaryUnits = Wh_GetIntSetting(L"Network.BinaryUnits") != 0;

    std::wstring counter = ReadStringSetting(L"Traffic.CounterMode");
    s.counterMode = StringSettingIs(counter, L"persistent") ? CounterMode::Persistent
                                                            : CounterMode::Session;

    std::wstring reset = ReadStringSetting(L"Traffic.ResetCounters");
    if (StringSettingIs(reset, L"download")) {
        s.resetRequest = ResetRequest::Download;
    } else if (StringSettingIs(reset, L"upload")) {
        s.resetRequest = ResetRequest::Upload;
    } else if (StringSettingIs(reset, L"both")) {
        s.resetRequest = ResetRequest::Both;
    } else {
        s.resetRequest = ResetRequest::None;
    }

    s.showTooltip = Wh_GetIntSetting(L"Behavior.ShowTooltip") != 0;
    s.showDetails = Wh_GetIntSetting(L"Behavior.ShowDetailsOnClick") != 0;
    s.hideFullscreen = Wh_GetIntSetting(L"Behavior.HideFullscreen") != 0;
    s.startEnabled = Wh_GetIntSetting(L"Behavior.StartEnabled") != 0;

    {
        std::lock_guard<std::mutex> guard(g_settingsMutex);
        g_settings = std::move(s);
    }
}

static ModSettings GetSettings() {
    std::lock_guard<std::mutex> guard(g_settingsMutex);
    return g_settings;
}

// The Windhawk settings UI has no push buttons, so "Reset traffic counters" is a
// dropdown. Applying it on every settings read would keep the counters pinned at
// zero, so the last applied value is remembered in the mod's own storage and the
// reset only fires when the selection actually changes to a non-none value.
static void ApplyResetSettingIfChanged() {
    std::wstring current = ReadStringSetting(L"Traffic.ResetCounters");
    if (current.empty()) {
        current = L"none";
    }

    WCHAR previous[32] = {};
    Wh_GetStringValue(L"LastResetApplied", previous, ARRAYSIZE(previous));

    if (_wcsicmp(previous, current.c_str()) == 0) {
        return;  // already handled
    }

    Wh_SetStringValue(L"LastResetApplied", current.c_str());

    unsigned long long flags = 0;
    if (StringSettingIs(current, L"download")) {
        flags = 1;
    } else if (StringSettingIs(current, L"upload")) {
        flags = 2;
    } else if (StringSettingIs(current, L"both")) {
        flags = 3;
    }

    if (flags) {
        g_pendingReset.fetch_or(flags);
        Wh_Log(L"Reset requested from settings: %s", current.c_str());
        if (g_wakeEvent) {
            SetEvent(g_wakeEvent);
        }
    }
}

// --- Number formatting -----------------------------------------------------
// MB/s  = MegaBYTES per second (1 byte = 8 bits)
// Mbps  = MegaBITS  per second
static std::wstring FormatScaled(double value, const wchar_t* unit) {
    wchar_t buffer[64];
    int decimals = (value >= 100.0) ? 0 : (value >= 10.0 ? 1 : 2);
    swprintf_s(buffer, L"%.*f", decimals, value);

    std::wstring text = buffer;
    if (decimals > 0 && text.find(L'.') != std::wstring::npos) {
        // 2.40 -> 2.4, 3.00 -> 3, but keep 1.02 intact.
        while (!text.empty() && text.back() == L'0') {
            text.pop_back();
        }
        if (!text.empty() && text.back() == L'.') {
            text.pop_back();
        }
    }

    text += L' ';
    text += unit;
    return text;
}

// Instantaneous rate. bytesPerSec is always measured in bytes; the unit setting
// only changes how it is presented.
static std::wstring FormatSpeed(double bytesPerSec, const ModSettings& s) {
    if (bytesPerSec < 0.0 || !std::isfinite(bytesPerSec)) {
        bytesPerSec = 0.0;
    }

    if (s.speedUnit == SpeedUnit::Bits) {
        static const wchar_t* kBitUnits[] = {L"bps", L"Kbps", L"Mbps", L"Gbps",
                                             L"Tbps"};
        double value = bytesPerSec * 8.0;  // 1 byte = 8 bits
        int index = 0;
        while (value >= 1000.0 && index < 4) {
            value /= 1000.0;
            index++;
        }
        if (index == 0) {
            wchar_t buffer[64];
            swprintf_s(buffer, L"%.0f bps", value);
            return buffer;
        }
        return FormatScaled(value, kBitUnits[index]);
    }

    static const wchar_t* kByteUnits[] = {L"B/s", L"KB/s", L"MB/s", L"GB/s",
                                          L"TB/s"};
    const double base = s.binaryUnits ? 1024.0 : 1000.0;
    double value = bytesPerSec;
    int index = 0;
    while (value >= base && index < 4) {
        value /= base;
        index++;
    }
    if (index == 0) {
        wchar_t buffer[64];
        swprintf_s(buffer, L"%.0f B/s", value);
        return buffer;
    }
    return FormatScaled(value, kByteUnits[index]);
}

// Cumulative volume: B / KB / MB / GB / TB / PB.
static std::wstring FormatBytes(unsigned long long bytes, const ModSettings& s) {
    static const wchar_t* kUnits[] = {L"B", L"KB", L"MB", L"GB", L"TB", L"PB"};
    const double base = s.binaryUnits ? 1024.0 : 1000.0;
    double value = (double)bytes;
    int index = 0;
    while (value >= base && index < 5) {
        value /= base;
        index++;
    }
    if (index == 0) {
        wchar_t buffer[64];
        swprintf_s(buffer, L"%.0f B", value);
        return buffer;
    }
    return FormatScaled(value, kUnits[index]);
}

// --- Persistent traffic counters ------------------------------------------
// Stored in Windhawk's per-mod storage so the totals survive Explorer / Windhawk
// / Windows restarts and disappear with the mod - a mod's own state does not
// belong in a directory that outlives its uninstall. Writes are batched (every 30
// seconds and on unload).
struct PersistentTotals {
    unsigned long long down = 0;
    unsigned long long up = 0;
};

static bool LoadPersistentTotals(PersistentTotals* out) {
    PersistentTotals stored;
    size_t read = Wh_GetBinaryValue(L"TrafficTotals", &stored, sizeof(stored));
    if (read != sizeof(stored)) {
        return false;
    }
    *out = stored;
    return true;
}

static bool SavePersistentTotals(const PersistentTotals& totals) {
    return Wh_SetBinaryValue(L"TrafficTotals", &totals, sizeof(totals)) != FALSE;
}
// --- Network monitoring ----------------------------------------------------
// One GetIfTable2 call per interval on a worker thread. Speed comes from the
// delta of the interface octet counters divided by the *measured* elapsed time
// (QueryPerformanceCounter), never from an assumed timer period.

struct TrackedInterface {
    NET_LUID luid{};
    NET_IFINDEX index = 0;
    unsigned long long lastIn = 0;
    unsigned long long lastOut = 0;
    bool seen = false;
};

struct InterfaceCandidate {
    NET_LUID luid{};
    NET_IFINDEX index = 0;
    std::wstring alias;
    std::wstring description;
    IFTYPE type = 0;
    unsigned long long inOctets = 0;
    unsigned long long outOctets = 0;
    bool up = false;
    bool physical = false;
};

class NetworkMonitor {
   public:
    void Initialize() {
        QueryPerformanceFrequency(&m_frequency);
        m_lastCounter.QuadPart = 0;
        m_haveBaseline = false;

        ModSettings s = GetSettings();
        if (s.counterMode == CounterMode::Persistent) {
            PersistentTotals stored;
            if (LoadPersistentTotals(&stored)) {
                m_totalDown = stored.down;
                m_totalUp = stored.up;
                Wh_Log(L"Traffic counter loaded: down=%llu up=%llu", stored.down,
                       stored.up);
            } else {
                Wh_Log(L"No stored traffic counter, starting from zero");
            }
        }
        m_lastSave = GetTickCount64();
    }

    void Shutdown() {
        FlushTotals(true);
    }

    // Returns true when the visible values changed enough to justify a repaint.
    bool Sample(NetSnapshot* out) {
        ModSettings s = GetSettings();

        unsigned long long resetFlags = g_pendingReset.exchange(0);
        if (resetFlags & 1) {
            m_totalDown = 0;
        }
        if (resetFlags & 2) {
            m_totalUp = 0;
        }
        if (resetFlags) {
            Wh_Log(L"Traffic counters reset (flags=%llu)", resetFlags);
            FlushTotals(true);
        }

        if (s.counterMode != m_lastCounterMode) {
            // Switching Session <-> Persistent: reload or restart the totals.
            if (s.counterMode == CounterMode::Persistent) {
                PersistentTotals stored;
                if (LoadPersistentTotals(&stored)) {
                    m_totalDown = stored.down;
                    m_totalUp = stored.up;
                } 
            } else {
                m_totalDown = 0;
                m_totalUp = 0;
            }
            m_lastCounterMode = s.counterMode;
        }

        MIB_IF_TABLE2* table = nullptr;
        NETIO_STATUS status = GetIfTable2(&table);
        if (status != NO_ERROR || !table) {
            if (table) {
                FreeMibTable(table);
            }
            if (!m_loggedTableFailure) {
                Wh_Log(L"GetIfTable2 failed (status=%lu)", (unsigned long)status);
                m_loggedTableFailure = true;
            }
            NetSnapshot snapshot = MakeDisconnectedSnapshot();
            bool changed = SnapshotDiffers(snapshot);
            *out = snapshot;
            m_lastPublished = snapshot;
            return changed;
        }
        m_loggedTableFailure = false;

        std::vector<InterfaceCandidate> candidates;
        std::vector<InterfaceCandidate> allCandidates;
        candidates.reserve(table->NumEntries);
        allCandidates.reserve(table->NumEntries);

        for (ULONG i = 0; i < table->NumEntries; i++) {
            const MIB_IF_ROW2& row = table->Table[i];

            InterfaceCandidate candidate;
            candidate.luid = row.InterfaceLuid;
            candidate.index = row.InterfaceIndex;
            candidate.alias = row.Alias;
            candidate.description = row.Description;
            candidate.type = row.Type;
            candidate.inOctets = row.InOctets;
            candidate.outOctets = row.OutOctets;
            candidate.up = row.OperStatus == IfOperStatusUp &&
                           row.MediaConnectState == MediaConnectStateConnected;
            candidate.physical =
                row.InterfaceAndOperStatusFlags.HardwareInterface &&
                !row.InterfaceAndOperStatusFlags.FilterInterface;

            // Loopback and NDIS filter pseudo-interfaces are never useful, not
            // even as a default-route target.
            if (row.Type != IF_TYPE_SOFTWARE_LOOPBACK &&
                !row.InterfaceAndOperStatusFlags.FilterInterface) {
                allCandidates.push_back(candidate);
            }

            if (!IsUsableInterface(row, s)) {
                continue;
            }
            candidates.push_back(std::move(candidate));
        }

        FreeMibTable(table);

        std::vector<InterfaceCandidate> selected =
            SelectInterfaces(candidates, allCandidates, s);

        // --- source change --------------------------------------------------
        // When the adapter carrying the internet changes (Ethernet -> VPN,
        // Wi-Fi -> Ethernet, ...) the old adapter's counters are meaningless for
        // the new connection, so start over rather than adding the two together.
        if (DidSourceChange(selected)) {
            std::wstring from = DescribeSelection(m_currentSource);
            std::wstring to = DescribeSelection(selected);
            if (s.resetOnSourceChange) {
                m_totalDown = 0;
                m_totalUp = 0;
                m_downRate = 0.0;
                m_upRate = 0.0;
                m_haveBaseline = false;  // no delta across the switch
                FlushTotals(true);
                Wh_Log(L"Network source changed (%s -> %s): traffic counters reset",
                       from.c_str(), to.c_str());
            } else {
                Wh_Log(L"Network source changed (%s -> %s): counters kept "
                       L"(ResetOnSourceChange is off)",
                       from.c_str(), to.c_str());
            }
            RememberSource(selected);
        }

        // --- elapsed time ---------------------------------------------------
        LARGE_INTEGER now;
        QueryPerformanceCounter(&now);
        double elapsedSeconds = 0.0;
        if (m_lastCounter.QuadPart != 0 && m_frequency.QuadPart != 0) {
            elapsedSeconds = (double)(now.QuadPart - m_lastCounter.QuadPart) /
                             (double)m_frequency.QuadPart;
        }
        m_lastCounter = now;

        // --- deltas ---------------------------------------------------------
        unsigned long long deltaIn = 0;
        unsigned long long deltaOut = 0;

        for (auto& tracked : m_tracked) {
            tracked.seen = false;
        }

        for (const auto& candidate : selected) {
            TrackedInterface* tracked = FindTracked(candidate.luid);
            if (!tracked) {
                TrackedInterface fresh;
                fresh.luid = candidate.luid;
                fresh.index = candidate.index;
                fresh.lastIn = candidate.inOctets;
                fresh.lastOut = candidate.outOctets;
                fresh.seen = true;
                m_tracked.push_back(fresh);
                continue;  // no delta on the first sighting
            }

            tracked->seen = true;
            tracked->index = candidate.index;

            // Counters can go backwards after a driver reset / adapter reset.
            if (candidate.inOctets >= tracked->lastIn) {
                deltaIn += candidate.inOctets - tracked->lastIn;
            }
            if (candidate.outOctets >= tracked->lastOut) {
                deltaOut += candidate.outOctets - tracked->lastOut;
            }
            tracked->lastIn = candidate.inOctets;
            tracked->lastOut = candidate.outOctets;
        }

        m_tracked.erase(std::remove_if(m_tracked.begin(), m_tracked.end(),
                                       [](const TrackedInterface& t) {
                                           return !t.seen;
                                       }),
                        m_tracked.end());

        // Ignore absurd deltas (sleep/wake, counter wrap) instead of showing a
        // fake multi-gigabyte spike.
        const bool plausible = m_haveBaseline && elapsedSeconds > 0.0005 &&
                               elapsedSeconds < 30.0;

        double downRate = 0.0;
        double upRate = 0.0;
        if (plausible) {
            downRate = (double)deltaIn / elapsedSeconds;
            upRate = (double)deltaOut / elapsedSeconds;
            m_totalDown += deltaIn;
            m_totalUp += deltaOut;
        }
        m_haveBaseline = true;

        // Light smoothing so the text does not jitter between two samples.
        m_downRate = m_downRate * 0.35 + downRate * 0.65;
        m_upRate = m_upRate * 0.35 + upRate * 0.65;
        if (m_downRate < 1.0) {
            m_downRate = downRate;
        }
        if (m_upRate < 1.0) {
            m_upRate = upRate;
        }

        NetSnapshot snapshot;
        snapshot.downBytesPerSec = m_downRate;
        snapshot.upBytesPerSec = m_upRate;
        snapshot.totalDown = m_totalDown;
        snapshot.totalUp = m_totalUp;
        snapshot.ifCount = (int)selected.size();
        snapshot.haveInterface = !selected.empty();
        snapshot.connected = !selected.empty();

        if (selected.empty()) {
            snapshot.ifName = candidates.empty() ? L"No Network" : L"Disconnected";
            snapshot.ifKind = L"";
            snapshot.ipv4 = L"";
        } else if (selected.size() == 1) {
            snapshot.ifName = selected[0].alias;
            snapshot.ifKind = DescribeType(selected[0].type);
            snapshot.ipv4 = GetIPv4(selected[0].index);
        } else {
            wchar_t buffer[64];
            swprintf_s(buffer, L"%d interfaces", (int)selected.size());
            snapshot.ifName = buffer;
            snapshot.ifKind = L"Multiple";
            snapshot.ipv4 = GetIPv4(selected[0].index);
        }

        LogInterfaceChange(snapshot, selected);

        if (s.counterMode == CounterMode::Persistent) {
            FlushTotals(false);
        }

        bool changed = SnapshotDiffers(snapshot);
        m_lastPublished = snapshot;
        *out = snapshot;
        return changed;
    }

    void ForceSaveNow() {
        FlushTotals(true);
    }

   private:
    static bool IsUsableInterface(const MIB_IF_ROW2& row, const ModSettings& s) {
        if (row.Type == IF_TYPE_SOFTWARE_LOOPBACK) {
            return false;
        }
        if (row.InterfaceAndOperStatusFlags.FilterInterface) {
            return false;  // NDIS filter / capture pseudo-interfaces
        }

        if (!s.excludeVirtual) {
            return true;
        }

        if (row.Type == IF_TYPE_TUNNEL || row.TunnelType != TUNNEL_TYPE_NONE) {
            return false;
        }
        if (!row.InterfaceAndOperStatusFlags.HardwareInterface) {
            return false;
        }

        std::wstring haystack = ToLower(std::wstring(row.Description) + L" " +
                                       std::wstring(row.Alias));
        static const wchar_t* kVirtualMarkers[] = {
            L"vmware",      L"virtualbox",  L"vbox",       L"hyper-v",
            L"vethernet",   L"docker",      L"loopback",   L"tap-windows",
            L"tap adapter", L"openvpn",     L"wintun",     L"wireguard",
            L"tunnel",      L"vpn",         L"zerotier",   L"tailscale",
            L"nordlynx",    L"proton",      L"expressvpn", L"hamachi",
            L"radmin",      L"pseudo",      L"teredo",     L"isatap",
            L"6to4",        L"npcap",       L"wan miniport", L"bluetooth",
            L"virtual",
        };
        for (const wchar_t* marker : kVirtualMarkers) {
            if (Contains(haystack, marker)) {
                return false;
            }
        }
        return true;
    }

    static std::wstring DescribeType(IFTYPE type) {
        switch (type) {
            case IF_TYPE_ETHERNET_CSMACD:
                return L"Ethernet";
            case IF_TYPE_IEEE80211:
                return L"Wi-Fi";
            case IF_TYPE_PPP:
                return L"PPP";
            case IF_TYPE_TUNNEL:
                return L"Tunnel";
            case IF_TYPE_IEEE1394:
                return L"FireWire";
            case IF_TYPE_SOFTWARE_LOOPBACK:
                return L"Loopback";
            default:
                return L"Network";
        }
    }

    // Auto mode: prefer the adapter carrying the default route, but only when it
    // is a physical one. When a VPN/tun client owns the route, measure its
    // physical carrier instead - some clients inflate their virtual adapter's
    // counters, and the physical one is what Task Manager and speedtests agree
    // with. Falls back to the busiest active physical adapter, and logs the
    // decision.
    //
    // `candidates` is the filtered list (virtual adapters removed when the
    // setting says so); `allCandidates` is everything real, used to resolve the
    // default-route index to a physical adapter even when the filter hides the
    // tunnel that owns the route.
    std::vector<InterfaceCandidate> SelectInterfaces(
        const std::vector<InterfaceCandidate>& candidates,
        const std::vector<InterfaceCandidate>& allCandidates,
        const ModSettings& s) {
        std::vector<InterfaceCandidate> result;

        auto pushActive = [&result](const InterfaceCandidate& c) {
            if (c.up) {
                result.push_back(c);
            }
        };

        switch (s.ifMode) {
            case InterfaceMode::All:
                for (const auto& c : candidates) {
                    pushActive(c);
                }
                return result;

            case InterfaceMode::Ethernet:
                for (const auto& c : candidates) {
                    if (c.type == IF_TYPE_ETHERNET_CSMACD) {
                        pushActive(c);
                    }
                }
                return result;

            case InterfaceMode::Wifi:
                for (const auto& c : candidates) {
                    if (c.type == IF_TYPE_IEEE80211) {
                        pushActive(c);
                    }
                }
                return result;

            case InterfaceMode::Specific: {
                if (s.selectedInterface.empty()) {
                    break;  // behave like Auto
                }
                std::wstring needle = ToLower(s.selectedInterface);
                for (const auto& c : candidates) {
                    std::wstring haystack =
                        ToLower(c.alias + L" " + c.description);
                    if (haystack.find(needle) != std::wstring::npos) {
                        pushActive(c);
                    }
                }
                if (!result.empty()) {
                    return result;
                }
                if (!m_loggedSpecificMiss) {
                    Wh_Log(L"Specific interface '%s' not found or not active, using Auto",
                           s.selectedInterface.c_str());
                    m_loggedSpecificMiss = true;
                }
                break;
            }

            case InterfaceMode::Auto:
            default:
                break;
        }

        // --- Auto ------------------------------------------------------------
        // The default route wins only when it is a *physical* adapter. When a
        // VPN/tun client owns the route, its virtual adapter's counters are not
        // trustworthy: some clients (e.g. NekoBox's sing-tun) count the same
        // bytes several times - measured at 2.66x the real payload in a
        // controlled test - so following the tunnel shows speeds the line
        // cannot deliver. Instead measure the physical adapter that actually
        // carries the traffic out, which is what Task Manager, speedtest and
        // the ISP all agree on.
        NET_IFINDEX bestIndex = GetInternetInterfaceIndex();
        if (bestIndex != 0) {
            for (const auto& c : allCandidates) {
                if (c.index == bestIndex && c.up) {
                    if (c.physical) {
                        result.push_back(c);
                        return result;
                    }
                    break;  // virtual default route; use its physical carrier
                }
            }
        }

        // Fall back to the active *physical* interface with the most traffic
        // so far; only if there is no physical one at all, any active one.
        const InterfaceCandidate* best = nullptr;
        unsigned long long bestTraffic = 0;
        for (bool preferPhysical : {true, false}) {
            for (const auto& c : candidates) {
                if (!c.up) {
                    continue;
                }
                if (preferPhysical && !c.physical) {
                    continue;
                }
                unsigned long long traffic = c.inOctets + c.outOctets;
                if (!best || traffic > bestTraffic) {
                    best = &c;
                    bestTraffic = traffic;
                }
            }
            if (best) {
                break;
            }
        }
        if (best) {
            if (!m_loggedAutoFallback) {
                Wh_Log(L"Auto mode: default route is virtual or unknown, measuring physical carrier '%s'",
                       best->alias.c_str());
                m_loggedAutoFallback = true;
            }
            result.push_back(*best);
        }
        return result;
    }

    static NET_IFINDEX GetInternetInterfaceIndex() {
        SOCKADDR_IN dest{};
        dest.sin_family = AF_INET;
        dest.sin_addr.S_un.S_addr = htonl(0x08080808);  // 8.8.8.8, no traffic sent
        DWORD index = 0;
        if (GetBestInterfaceEx((sockaddr*)&dest, &index) == NO_ERROR) {
            return (NET_IFINDEX)index;
        }
        return 0;
    }

    static std::wstring GetIPv4(NET_IFINDEX index) {
        ULONG size = 0;
        const ULONG flags = GAA_FLAG_SKIP_ANYCAST | GAA_FLAG_SKIP_MULTICAST |
                            GAA_FLAG_SKIP_DNS_SERVER | GAA_FLAG_SKIP_FRIENDLY_NAME;
        if (GetAdaptersAddresses(AF_INET, flags, nullptr, nullptr, &size) !=
                ERROR_BUFFER_OVERFLOW ||
            size == 0) {
            return L"";
        }

        std::vector<BYTE> buffer(size);
        IP_ADAPTER_ADDRESSES* adapters = (IP_ADAPTER_ADDRESSES*)buffer.data();
        if (GetAdaptersAddresses(AF_INET, flags, nullptr, adapters, &size) !=
            NO_ERROR) {
            return L"";
        }

        for (IP_ADAPTER_ADDRESSES* adapter = adapters; adapter;
             adapter = adapter->Next) {
            if (adapter->IfIndex != index) {
                continue;
            }
            for (IP_ADAPTER_UNICAST_ADDRESS* unicast =
                     adapter->FirstUnicastAddress;
                 unicast; unicast = unicast->Next) {
                if (!unicast->Address.lpSockaddr ||
                    unicast->Address.lpSockaddr->sa_family != AF_INET) {
                    continue;
                }
                sockaddr_in* addr = (sockaddr_in*)unicast->Address.lpSockaddr;
                BYTE* octets = (BYTE*)&addr->sin_addr;
                if (octets[0] == 169 && octets[1] == 254) {
                    continue;  // link-local, not useful
                }
                wchar_t text[32];
                swprintf_s(text, L"%u.%u.%u.%u", octets[0], octets[1], octets[2],
                           octets[3]);
                return text;
            }
        }
        return L"";
    }

    TrackedInterface* FindTracked(const NET_LUID& luid) {
        for (auto& tracked : m_tracked) {
            if (tracked.luid.Value == luid.Value) {
                return &tracked;
            }
        }
        return nullptr;
    }

    // --- source-change detection -------------------------------------------
    // The "source" is the set of LUIDs currently being counted. When it changes,
    // the traffic being measured is a different connection.
    static std::vector<unsigned long long> SelectionLuids(
        const std::vector<InterfaceCandidate>& selected) {
        std::vector<unsigned long long> luids;
        luids.reserve(selected.size());
        for (const auto& c : selected) {
            luids.push_back(c.luid.Value);
        }
        std::sort(luids.begin(), luids.end());
        return luids;
    }

    bool DidSourceChange(const std::vector<InterfaceCandidate>& selected) {
        std::vector<unsigned long long> luids = SelectionLuids(selected);

        // An empty selection means "disconnected", which is a transient state on
        // the way between two adapters (unplug Ethernet, VPN comes up). Treating
        // it as a change would reset twice; wait for the next real adapter.
        if (luids.empty()) {
            return false;
        }
        if (m_sourceLuids.empty()) {
            m_sourceLuids = luids;
            m_currentSource = selected;
            return false;  // first selection is not a change
        }
        return luids != m_sourceLuids;
    }

    void RememberSource(const std::vector<InterfaceCandidate>& selected) {
        m_sourceLuids = SelectionLuids(selected);
        m_currentSource = selected;
    }

    static std::wstring DescribeSelection(
        const std::vector<InterfaceCandidate>& selected) {
        if (selected.empty()) {
            return L"none";
        }
        if (selected.size() == 1) {
            return selected[0].alias;
        }
        wchar_t buffer[64];
        swprintf_s(buffer, L"%d interfaces", (int)selected.size());
        return buffer;
    }

    NetSnapshot MakeDisconnectedSnapshot() const {
        NetSnapshot snapshot;
        snapshot.downBytesPerSec = 0.0;
        snapshot.upBytesPerSec = 0.0;
        snapshot.totalDown = m_totalDown;
        snapshot.totalUp = m_totalUp;
        snapshot.ifName = L"No Network";
        snapshot.connected = false;
        snapshot.haveInterface = false;
        return snapshot;
    }

    bool SnapshotDiffers(const NetSnapshot& snapshot) const {
        // Compare what is actually drawn: the formatted strings carry 2-3
        // significant digits, so a sub-digit wobble in the raw rate no longer
        // forces a repaint. A numeric threshold could not do this - any absolute
        // or relative epsilon still fires on changes the user cannot see.
        ModSettings s = GetSettings();
        if (FormatSpeed(snapshot.downBytesPerSec, s) !=
                FormatSpeed(m_lastPublished.downBytesPerSec, s) ||
            FormatSpeed(snapshot.upBytesPerSec, s) !=
                FormatSpeed(m_lastPublished.upBytesPerSec, s) ||
            FormatBytes(snapshot.totalDown, s) !=
                FormatBytes(m_lastPublished.totalDown, s) ||
            FormatBytes(snapshot.totalUp, s) !=
                FormatBytes(m_lastPublished.totalUp, s)) {
            return true;
        }
        return snapshot.connected != m_lastPublished.connected ||
               snapshot.ifName != m_lastPublished.ifName ||
               snapshot.ipv4 != m_lastPublished.ipv4;
    }

    void LogInterfaceChange(const NetSnapshot& snapshot,
                            const std::vector<InterfaceCandidate>& selected) {
        std::wstring signature;
        for (const auto& c : selected) {
            signature += c.alias;
            signature += L'|';
        }
        if (signature == m_lastSignature) {
            return;
        }
        m_lastSignature = signature;
        m_loggedAutoFallback = false;
        m_loggedSpecificMiss = false;

        if (selected.empty()) {
            Wh_Log(L"Network interface changed: none active (%s)",
                   snapshot.ifName.c_str());
        } else {
            Wh_Log(L"Selected interface: %s (%s, %d total) ipv4=%s",
                   snapshot.ifName.c_str(), snapshot.ifKind.c_str(),
                   snapshot.ifCount,
                   snapshot.ipv4.empty() ? L"n/a" : snapshot.ipv4.c_str());
        }
    }

    // Batched, atomic persistence: at most one write every 30 seconds, plus one
    // on unload / reset.
    void FlushTotals(bool force) {
        ModSettings s = GetSettings();
        if (s.counterMode != CounterMode::Persistent) {
            return;  // session mode never touches storage
        }

        ULONGLONG now = GetTickCount64();
        if (!force && now - m_lastSave < 30000) {
            return;
        }
        if (!force && m_totalDown == m_savedDown && m_totalUp == m_savedUp) {
            return;
        }

        PersistentTotals totals;
        totals.down = m_totalDown;
        totals.up = m_totalUp;
        if (SavePersistentTotals(totals)) {
            m_savedDown = m_totalDown;
            m_savedUp = m_totalUp;
            m_lastSave = now;
            Wh_Log(L"Traffic counter saved: down=%llu up=%llu", totals.down,
                   totals.up);
        }
    }

    LARGE_INTEGER m_frequency{};
    LARGE_INTEGER m_lastCounter{};
    bool m_haveBaseline = false;
    bool m_loggedTableFailure = false;
    bool m_loggedAutoFallback = false;
    bool m_loggedSpecificMiss = false;

    std::vector<TrackedInterface> m_tracked;
    // The set of interface LUIDs currently being counted, so a change of
    // internet source (Ethernet -> VPN, ...) can be detected.
    std::vector<unsigned long long> m_sourceLuids;
    std::vector<InterfaceCandidate> m_currentSource;
    double m_downRate = 0.0;
    double m_upRate = 0.0;
    unsigned long long m_totalDown = 0;
    unsigned long long m_totalUp = 0;
    unsigned long long m_savedDown = ULLONG_MAX;
    unsigned long long m_savedUp = ULLONG_MAX;
    ULONGLONG m_lastSave = 0;
    CounterMode m_lastCounterMode = CounterMode::Session;
    NetSnapshot m_lastPublished;
    std::wstring m_lastSignature;
};

static NetworkMonitor g_monitor;

// Worker thread: samples the counters and only wakes the UI when something
// visible changed. Waits on events, so it never busy-waits.
static void NetworkWorkerThread() {
    g_monitor.Initialize();

    HANDLE waitHandles[2] = {g_stopEvent, g_wakeEvent};

    for (;;) {
        NetSnapshot snapshot;
        bool changed = false;
        try {
            changed = g_monitor.Sample(&snapshot);
        } catch (...) {
            Wh_Log(L"Exception while sampling network counters");
            snapshot = NetSnapshot();
            snapshot.ifName = L"No Network";
            changed = true;
        }

        {
            std::lock_guard<std::mutex> guard(g_snapshotMutex);
            g_snapshot = snapshot;
        }

        if (changed && g_hWidget) {
            PostMessage(g_hWidget, APP_WM_DATA_UPDATED, 0, 0);
        }

        int interval = GetSettings().updateInterval;
        DWORD wait = WaitForMultipleObjects(2, waitHandles, FALSE, (DWORD)interval);
        if (wait == WAIT_OBJECT_0) {
            break;  // stop event
        }
        // WAIT_OBJECT_0 + 1 (wake) or WAIT_TIMEOUT: sample again immediately.
    }

    g_monitor.Shutdown();
}
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
//
// The DPI entry points are resolved once into function-local statics: this runs on
// every paint and on every WM_MOUSEMOVE (twice, via the divider hit test), so a
// GetProcAddress per call would be pure overhead.
static double GetScaleForWindow(HWND hwnd) {
    ModSettings s = GetSettings();
    if (!s.dpiScaling) {
        return 1.0;
    }

    using GetDpiForWindow_t = UINT(WINAPI*)(HWND);
    using GetDpiForSystem_t = UINT(WINAPI*)();
    static GetDpiForWindow_t getDpiForWindow = nullptr;
    static GetDpiForSystem_t getDpiForSystem = nullptr;
    static bool resolved = false;
    if (!resolved) {
        resolved = true;
        if (HMODULE user32 = GetModuleHandleW(L"user32.dll")) {
            getDpiForWindow =
                (GetDpiForWindow_t)GetProcAddress(user32, "GetDpiForWindow");
            getDpiForSystem =
                (GetDpiForSystem_t)GetProcAddress(user32, "GetDpiForSystem");
        }
    }

    UINT dpi = 0;
    if (getDpiForWindow && hwnd) {
        dpi = getDpiForWindow(hwnd);
    }
    if (!dpi && getDpiForSystem) {
        dpi = getDpiForSystem();
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

// The arrow style as it is RIGHT NOW: the shape from the setting/menu, or None
// when the "Show arrows" toggle is off. Painters use this instead of the
// settings snapshot so a menu toggle takes effect on the next repaint without a
// settings reload.
static ArrowStyle LiveArrowStyle() {
    if (g_arrowsHidden.load()) {
        return ArrowStyle::None;
    }
    return (ArrowStyle)ClampInt(g_arrowStyle.load(), 0, (int)ArrowStyle::Plus);
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
// "Segoe UI" + FontStyleBold if a family is missing. When the user sets a custom
// font family, the weight maps onto that family's style bits instead (Semibold
// and Black only exist as separate Segoe families).
struct ResolvedFont {
    std::wstring family;
    INT style;
};

static ResolvedFont ResolveWeight(TextWeight weight,
                                  const std::wstring& customFamily) {
    if (!customFamily.empty() && customFamily != kFontName) {
        switch (weight) {
            case TextWeight::Regular:
                return {customFamily, FontStyleRegular};
            case TextWeight::Italic:
                return {customFamily, FontStyleItalic};
            case TextWeight::Bold:
            default:
                return {customFamily, FontStyleBold};
        }
    }
    switch (weight) {
        case TextWeight::Black:
            return {L"Segoe UI Black", FontStyleRegular};
        case TextWeight::Semibold:
            return {L"Segoe UI Semibold", FontStyleRegular};
        case TextWeight::Regular:
            return {kFontName, FontStyleRegular};
        case TextWeight::Italic:
            return {kFontName, FontStyleItalic};
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
    // A manual per-direction colour wins over both the text colour and the
    // accent, and recolours the numbers of that half too - not just the arrow.
    const bool customDown = s.downloadColor != 0;
    const bool customUp = s.uploadColor != 0;
    if (customDown) {
        downColor = Color(s.downloadColor);
    }
    if (customUp) {
        upColor = Color(s.uploadColor);
    }

    // Hover background for the widget body only - not for the divider grab zone,
    // which gets its own highlight on the line itself.
    if (g_widgetHover.load() == 1) {
        GraphicsPath hoverPath;
        AddRoundedRect(hoverPath, 1.0f, 1.0f, (REAL)width - 2.0f,
                       (REAL)height - 2.0f, (REAL)(8.0 * scale));
        SolidBrush hoverBrush(BlendColor(textColor, 26));
        graphics.FillPath(&hoverBrush, &hoverPath);
    }

    std::unique_ptr<FontFamily> family;
    INT weight = FontStyleBold;
    MakeFontFamily(ResolveWeight(s.textWeight, s.fontFamily), family, weight);
    FontFamily& fontFamily = *family;
    // Captions keep a fixed weight so the SPEED/TOTAL hierarchy does not collapse
    // when the user picks a lighter weight for the values.
    std::unique_ptr<FontFamily> captionFamily;
    INT captionWeight = FontStyleBold;
    MakeFontFamily(ResolveWeight(TextWeight::Bold, s.fontFamily), captionFamily, captionWeight);
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
    // Live display state, read from the atomics (not the settings snapshot) so a
    // right-click toggle repaints correctly without a settings reload:
    //   displayMode: 0 = both halves, 1 = speeds only, 2 = totals only
    //   captionBits: bit0 = SPEED visible, bit1 = TOTAL visible
    //   arrowsHidden / arrowStyle: the arrow shape, or none at all
    const int displayMode = ClampInt(g_displayMode.load(), 0, 2);
    const int captionBits = ClampInt(g_captionBits.load(), 0, 3);
    const ArrowStyle arrowStyle = LiveArrowStyle();
    // Captions follow their own bits in every display mode: in speeds-only the
    // SPEED label can still show, in totals-only the TOTAL label can still show.
    const bool showLabels = captionBits != 0;
    // Row centres are a fixed fraction of the height apart; the arrow is capped to
    // that distance minus a small gap, so a large ArrowScale can never make the
    // down/up glyphs touch or spill outside the widget. Bigger arrows therefore
    // need a taller widget (Panel height), which the setting description says.
    REAL rowPitch = (REAL)height * (showLabels ? 0.36f : 0.44f);
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

    const REAL gutter = ArrowGutter(arrowStyle, arrowSize, arrowGap);

    // When a direction has a manual colour, its numbers take that colour too
    // (slightly dimmed for the totals so the hierarchy survives); otherwise the
    // shared text brushes are used as before.
    SolidBrush downTextBrush(downColor);
    SolidBrush upTextBrush(upColor);
    SolidBrush downTotalBrush(BlendColor(Color(downColor.GetRed(), downColor.GetGreen(), downColor.GetBlue()), 250));
    SolidBrush upTotalBrush(BlendColor(Color(upColor.GetRed(), upColor.GetGreen(), upColor.GetBlue()), 250));

    auto drawRow = [&](REAL x, REAL centerY, bool down, const std::wstring& text,
                       Font& font, SolidBrush& brush, REAL maxWidth,
                       bool isTotal = false) {
        DrawArrow(graphics, down ? downColor : upColor, x + arrowSize / 2.0f,
                  centerY, arrowSize, down, arrowStyle);
        RectF textRect(x + gutter, centerY - fontSize, maxWidth - gutter,
                       fontSize * 2.0f);
        SolidBrush* use = &brush;
        if (customDown && down) {
            use = isTotal ? &downTotalBrush : &downTextBrush;
        } else if (customUp && !down) {
            use = isTotal ? &upTotalBrush : &upTextBrush;
        }
        DrawSharpString(graphics, text.c_str(), font, textRect, format, *use,
                        light, shadowOffset);
    };

    // Speeds-only views (the LayoutMode setting, or display mode 1).
    if (displayMode == 1 || s.layout == LayoutMode::OneLine) {
        if (s.layout == LayoutMode::OneLine && displayMode != 2) {
            REAL centerY = height / 2.0f;
            REAL columnWidth = ((REAL)width - padding * 2.0f) / 2.0f;
            drawRow(padding, centerY, true, downSpeed, fontValue, textBrush,
                    columnWidth);
            drawRow(padding + columnWidth, centerY, false, upSpeed, fontValue,
                    textBrush, columnWidth);
            return;
        }
        if (displayMode != 2) {
            REAL speedTopY = height * 0.30f;
            REAL speedBottomY = height * 0.72f;
            if (showLabels && (captionBits & 1)) {
                REAL captionY = height * 0.16f;
                speedTopY = height * 0.46f;
                speedBottomY = height * 0.79f;
                DrawSharpString(graphics, L"SPEED", fontCaption,
                                RectF(padding, captionY - fontSize * 0.6f,
                                      (REAL)width - padding * 2.0f,
                                      fontSize * 1.2f),
                                format, captionBrush, light, shadowOffset);
            }
            REAL columnWidth = (REAL)width - padding * 2.0f;
            drawRow(padding, speedTopY, true, downSpeed, fontValue, textBrush,
                    columnWidth);
            drawRow(padding, speedBottomY, false, upSpeed, fontValue, textBrush,
                    columnWidth);
            return;
        }
        // fall through: display mode 2 (totals) uses the full-layout geometry
        // below with the speed column hidden.
    }

    REAL topY = height * 0.30f;
    REAL bottomY = height * 0.72f;

    if (s.layout == LayoutMode::Speeds && displayMode == 0) {
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
    //
    // Display mode 1 hides the totals column (speeds fill the width), mode 2
    // hides the speeds column (totals fill the width).
    const bool showSpeedCol = displayMode != 2;
    const bool showTotalCol = displayMode != 1;
    REAL columnGap = (REAL)(10.0 * scale);
    REAL usable = (REAL)width - padding * 2.0f -
                  (showSpeedCol && showTotalCol ? columnGap : 0.0f);
    REAL leftWidth = usable * (s.dividerPos / 100.0f);
    REAL columnWidth = usable - leftWidth;
    REAL rightX = padding + leftWidth + columnGap;
    if (!showSpeedCol) {
        // Totals-only: the totals column takes the whole width and the TOTAL
        // caption sits at the left padding, not at the (now meaningless) split.
        leftWidth = usable;
        columnWidth = usable;
        rightX = padding;
    } else if (!showTotalCol) {
        leftWidth = usable;
        columnWidth = 0;
    }

    const bool showSpeedLabel = (captionBits & 1) != 0 && showLabels && showSpeedCol;
    const bool showTotalLabel = (captionBits & 2) != 0 && showLabels && showTotalCol;
    if (showSpeedLabel || showTotalLabel) {
        REAL captionY = height * 0.16f;
        topY = height * 0.46f;
        bottomY = height * 0.79f;

        if (showSpeedLabel) {
            DrawSharpString(graphics, L"SPEED", fontCaption,
                            RectF(padding, captionY - fontSize * 0.6f, leftWidth,
                                  fontSize * 1.2f),
                            format, captionBrush, light, shadowOffset);
        }
        if (showTotalLabel) {
            DrawSharpString(graphics, L"TOTAL", fontCaption,
                            RectF(rightX, captionY - fontSize * 0.6f, columnWidth,
                                  fontSize * 1.2f),
                            format, captionBrush, light, shadowOffset);
        }
    }

    // Divider. Its x is the split point plus the user's dragged offset - and the
    // offset is applied HERE ONLY, never to leftWidth/rightX, so dragging the line
    // moves nothing but the line. The painted x is published for the hit test so
    // the grab zone always matches what is on screen.
    // Only meaningful while both columns are on screen.
    REAL dividerX = padding + leftWidth + columnGap / 2.0f;
    if (showSpeedCol && showTotalCol && s.dividerDraggable) {
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
    g_dividerHitX = (showSpeedCol && showTotalCol) ? (float)dividerX : -1.0f;

    if (showSpeedCol && showTotalCol &&
        (s.dividerOpacity > 0 ||
         (s.dividerDraggable && g_widgetHover.load() == 2))) {
        // While being dragged (or hovered) the line brightens, so it is obvious
        // that it is the thing being grabbed even at opacity 0.
        BYTE alpha = (BYTE)s.dividerOpacity;
        REAL thickness = (REAL)(1.0 * scale);
        if (g_dividerDragging.load()) {
            alpha = 220;
            thickness = (REAL)(2.0 * scale);
        } else if (g_widgetHover.load() == 2) {
            alpha = (BYTE)(alpha < 140 ? 140 : alpha);
            thickness = (REAL)(2.0 * scale);
        }
        Pen divider(BlendColor(textColor, alpha), thickness);
        REAL top = showLabels ? height * 0.10f : height * 0.20f;
        REAL bottom = showLabels ? height * 0.90f : height * 0.80f;
        graphics.DrawLine(&divider, dividerX, top, dividerX, bottom);
    }

    if (showSpeedCol) {
        drawRow(padding, topY, true, downSpeed, fontValue, textBrush, leftWidth);
        drawRow(padding, bottomY, false, upSpeed, fontValue, textBrush,
                leftWidth);
    }
    if (showTotalCol) {
        drawRow(showSpeedCol ? rightX : padding, topY, true, downTotal, fontTotal,
                dimBrush, showSpeedCol ? columnWidth : leftWidth, true);
        drawRow(showSpeedCol ? rightX : padding, bottomY, false, upTotal,
                fontTotal, dimBrush, showSpeedCol ? columnWidth : leftWidth,
                true);
    }
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
    // Manual per-direction colours win, exactly as in the widget.
    const bool customDown = s.downloadColor != 0;
    const bool customUp = s.uploadColor != 0;
    if (customDown) {
        downColor = Color(s.downloadColor);
    }
    if (customUp) {
        upColor = Color(s.uploadColor);
    }

    std::unique_ptr<FontFamily> familyPtr;
    INT weight = FontStyleBold;
    MakeFontFamily(ResolveWeight(s.textWeight, s.fontFamily), familyPtr, weight);
    FontFamily& fontFamily = *familyPtr;
    // Captions keep a fixed bold weight so the section hierarchy survives a
    // lighter weight choice for the values.
    std::unique_ptr<FontFamily> captionFamilyPtr;
    INT captionWeight = FontStyleBold;
    MakeFontFamily(ResolveWeight(TextWeight::Bold, s.fontFamily), captionFamilyPtr,
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
    SolidBrush downValueBrush(downColor);
    SolidBrush upValueBrush(upColor);

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
    // text column instead of two competing ones. With arrows hidden the gutter
    // collapses and the text moves left into the freed space.
    const REAL arrow = layout.arrow;
    const ArrowStyle arrowStyle = LiveArrowStyle();
    const REAL arrowGutter = ArrowGutter(arrowStyle, arrow, layout.arrowGap);
    const REAL textX = margin + arrowGutter;
    const REAL textWidth = contentWidth - arrowGutter;
    const REAL valueNumberColumn = MeasureNumberColumn(graphics, fontValue, left);

    auto drawMetric = [&](const wchar_t* label, const std::wstring& value,
                          bool down) {
        REAL blockTop = y;
        REAL blockHeight = layout.labelRow + layout.valueRow;
        DrawArrow(graphics, down ? downColor : upColor, margin + arrow / 2.0f,
                  blockTop + blockHeight / 2.0f, arrow, down, arrowStyle);
        DrawSharpString(graphics, label, fontLabel,
                        RectF(textX, y, textWidth, layout.labelRow), left,
                        labelBrush, light, shadowOffset);
        y += layout.labelRow;
        // A manual direction colour also tints that metric's value.
        const bool tint = down ? customDown : customUp;
        DrawValueWithFixedUnit(graphics, value, fontValue, fontValueUnit, textX, y,
                               textWidth, layout.valueRow, left,
                               tint ? (down ? downValueBrush : upValueBrush)
                                    : textBrush,
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
              y + layout.totalRow / 2.0f, totalArrow, true, arrowStyle);
    DrawValueWithFixedUnit(graphics, FormatBytes(snapshot.totalDown, s), fontTotal,
                           fontTotalUnit, margin + arrowGutter, y,
                           halfWidth - arrowGutter, layout.totalRow,
                           left, customDown ? downValueBrush : textBrush,
                           unitBrush, light, shadowOffset,
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
        arrowGutter + totalNumberColumn + unitBounds.Width;
    REAL upGroupX = margin + contentWidth - upGroupWidth;
    REAL upGroupFloor = margin + halfWidth + layout.columnGap;
    if (upGroupX < upGroupFloor) {
        upGroupX = upGroupFloor;
    }
    DrawArrow(graphics, upColor, upGroupX + arrow / 2.0f,
              y + layout.totalRow / 2.0f, totalArrow, false, arrowStyle);
    DrawValueWithFixedUnit(graphics, upTotalText, fontTotal, fontTotalUnit,
                           upGroupX + arrowGutter, y,
                           upGroupWidth - arrowGutter, layout.totalRow,
                           left, customUp ? upValueBrush : textBrush,
                           unitBrush, light, shadowOffset,
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
    bool hoverReset = g_panelHover.load() == 1;
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
// --- Taskbar geometry ------------------------------------------------------
// The widget is positioned relative to the taskbar rectangle, never with fixed
// screen coordinates. Works for bottom/top/left/right taskbars, any DPI, and
// multiple monitors (primary taskbar preferred, secondary as fallback).
enum class TaskbarEdge { Bottom, Top, Left, Right };

// Pure function so it can be unit tested with synthetic rectangles: which screen
// edge is this taskbar docked to?
static TaskbarEdge ClassifyTaskbarEdge(const RECT& taskbar, const RECT& screen) {
    int width = taskbar.right - taskbar.left;
    int height = taskbar.bottom - taskbar.top;
    if (width >= height) {
        return (taskbar.top - screen.top) <= (screen.bottom - taskbar.bottom)
                   ? TaskbarEdge::Top
                   : TaskbarEdge::Bottom;
    }
    return (taskbar.left - screen.left) <= (screen.right - taskbar.right)
               ? TaskbarEdge::Left
               : TaskbarEdge::Right;
}

struct TaskbarInfo {
    HWND hwnd = nullptr;
    RECT rect{};
    bool visible = false;
    TaskbarEdge edge = TaskbarEdge::Bottom;
};

static bool GetTaskbarInfo(TaskbarInfo* out) {
    HWND taskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (!taskbar) {
        // Explorer may be restarting, or this monitor only has a secondary bar.
        taskbar = FindWindowW(L"Shell_SecondaryTrayWnd", nullptr);
    }
    if (!taskbar || !IsWindow(taskbar)) {
        return false;
    }

    RECT rect{};
    if (!GetWindowRect(taskbar, &rect)) {
        return false;
    }

    out->hwnd = taskbar;
    out->rect = rect;
    out->visible = IsWindowVisible(taskbar) != FALSE;

    HMONITOR monitor = MonitorFromWindow(taskbar, MONITOR_DEFAULTTONEAREST);
    MONITORINFO monitorInfo{sizeof(MONITORINFO)};
    if (GetMonitorInfoW(monitor, &monitorInfo)) {
        out->edge = ClassifyTaskbarEdge(rect, monitorInfo.rcMonitor);
    }
    return true;
}

// Where should the widget sit, given a taskbar rect and edge? Also pure, so the
// bottom/top/left/right placement can be verified without a real taskbar.
static void ComputeWidgetOrigin(const RECT& taskbar,
                                TaskbarEdge edge,
                                int width,
                                int height,
                                int offsetX,
                                int offsetY,
                                int* x,
                                int* y) {
    int taskbarWidth = taskbar.right - taskbar.left;
    int taskbarHeight = taskbar.bottom - taskbar.top;

    switch (edge) {
        case TaskbarEdge::Left:
        case TaskbarEdge::Right:
            // Vertical taskbar: stack the widget from the top, centered.
            *x = taskbar.left + (taskbarWidth - width) / 2 + offsetX;
            *y = taskbar.top + offsetY;
            break;
        case TaskbarEdge::Top:
        case TaskbarEdge::Bottom:
        default:
            *x = taskbar.left + offsetX;
            *y = taskbar.top + (taskbarHeight - height) / 2 + offsetY;
            break;
    }
}

// Widget size in physical pixels for the taskbar's monitor.
static void GetWidgetSize(HWND hwnd, int* width, int* height) {
    ModSettings s = GetSettings();
    double scale = GetScaleForWindow(hwnd);
    *width = (int)(s.width * scale);
    *height = (int)(s.height * scale);
}

static bool ComputeWidgetPosition(HWND hwnd, int* x, int* y, int* width,
                                  int* height) {
    TaskbarInfo taskbar;
    if (!GetTaskbarInfo(&taskbar)) {
        return false;
    }

    GetWidgetSize(hwnd, width, height);

    ModSettings s = GetSettings();
    double scale = GetScaleForWindow(hwnd);
    int offsetX = (int)(s.offsetX * scale);
    int offsetY = (int)(s.offsetY * scale);

    ComputeWidgetOrigin(taskbar.rect, taskbar.edge, *width, *height, offsetX,
                        offsetY, x, y);

    // Keep the widget on the taskbar's monitor.
    HMONITOR monitor = MonitorFromWindow(taskbar.hwnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFO monitorInfo{sizeof(MONITORINFO)};
    if (GetMonitorInfoW(monitor, &monitorInfo)) {
        const RECT& screen = monitorInfo.rcMonitor;
        if (*x + *width > screen.right) {
            *x = screen.right - *width;
        }
        if (*x < screen.left) {
            *x = screen.left;
        }
        if (*y + *height > screen.bottom) {
            *y = screen.bottom - *height;
        }
        if (*y < screen.top) {
            *y = screen.top;
        }
    }
    return true;
}

// A window that covers its whole monitor without being a real D3D fullscreen
// (browser kiosk/F11, video players, some games in "borderless" mode) does not
// raise the notification states above, so check the foreground window's rect
// against its monitor as well.
static bool IsForegroundWindowFullscreen() {
    HWND fg = GetForegroundWindow();
    if (!fg || fg == g_hWidget || fg == g_hPanel || fg == g_hTooltip) {
        return false;
    }
    // Never treat the desktop or the taskbar itself as "fullscreen".
    wchar_t className[64] = {};
    GetClassNameW(fg, className, ARRAYSIZE(className));
    if (!wcscmp(className, L"WorkerW") || !wcscmp(className, L"Progman") ||
        !wcscmp(className, L"Shell_TrayWnd") ||
        !wcscmp(className, L"Windows.UI.Core.CoreWindow") ||
        !wcscmp(className, L"XamlExplorerHostIslandWindow")) {
        return false;
    }
    RECT window{};
    if (!GetWindowRect(fg, &window)) {
        return false;
    }
    HMONITOR monitor = MonitorFromWindow(fg, MONITOR_DEFAULTTONEAREST);
    if (!monitor) {
        return false;
    }
    MONITORINFO info{sizeof(info)};
    if (!GetMonitorInfoW(monitor, &info)) {
        return false;
    }
    // A window is "fullscreen" when it covers the monitor completely.
    return window.left <= info.rcMonitor.left &&
           window.top <= info.rcMonitor.top &&
           window.right >= info.rcMonitor.right &&
           window.bottom >= info.rcMonitor.bottom;
}

static bool ShouldHideForFullscreen() {
    if (!GetSettings().hideFullscreen) {
        return false;
    }
    QUERY_USER_NOTIFICATION_STATE state;
    if (SUCCEEDED(SHQueryUserNotificationState(&state)) &&
        (state == QUNS_BUSY || state == QUNS_RUNNING_D3D_FULL_SCREEN ||
         state == QUNS_PRESENTATION_MODE)) {
        return true;
    }
    return IsForegroundWindowFullscreen();
}

static void LogFullscreenTransition(bool hidden) {
    HWND fg = GetForegroundWindow();
    wchar_t className[64] = {};
    wchar_t title[80] = {};
    if (fg) {
        GetClassNameW(fg, className, ARRAYSIZE(className));
        GetWindowTextW(fg, title, ARRAYSIZE(title));
    }
    Wh_Log(L"Fullscreen hide %s (foreground: %s '%s')",
           hidden ? L"engaged" : L"released", className, title);
}

// --- fullscreen state machine ----------------------------------------------
// The foreground window crosses the fullscreen boundary in moments: a
// notification stealing focus for an instant, an in-game overlay popup, the
// task switcher passing through. Flipping visibility on every check made the
// widget blink in and out, so a hide is confirmed only after two consecutive
// sightings and a restore the same way; single blips change nothing.
static int g_fsStrikes = 0;
static bool g_fullscreenHidden = false;

static void UpdateFullscreenHideState(HWND hwnd) {
    if (!GetSettings().hideFullscreen) {
        g_fsStrikes = 0;
        if (g_fullscreenHidden) {
            g_fullscreenHidden = false;
            LogFullscreenTransition(false);
            PostMessage(hwnd, APP_WM_REPOSITION, 0, 0);
        }
        return;
    }
    if (ShouldHideForFullscreen()) {
        if (g_fullscreenHidden) {
            g_fsStrikes = 2;
        } else if (++g_fsStrikes >= 2) {
            g_fullscreenHidden = true;
            LogFullscreenTransition(true);
            PostMessage(hwnd, APP_WM_REPOSITION, 0, 0);
        }
    } else if (g_fullscreenHidden && --g_fsStrikes <= 0) {
        g_fullscreenHidden = false;
        LogFullscreenTransition(false);
        PostMessage(hwnd, APP_WM_REPOSITION, 0, 0);
    } else if (!g_fullscreenHidden) {
        g_fsStrikes = 0;
    }
}

static void RepositionWidget(HWND hwnd) {
    ModSettings s = GetSettings();

    TaskbarInfo taskbar;
    bool haveTaskbar = GetTaskbarInfo(&taskbar);

    bool shouldHide = g_hiddenByUser.load() || !s.startEnabled ||
                      !haveTaskbar || !taskbar.visible || g_fullscreenHidden;

    if (shouldHide) {
        if (IsWindowVisible(hwnd)) {
            ShowWindow(hwnd, SW_HIDE);
            if (g_hPanel && g_panelVisible.load()) {
                ShowWindow(g_hPanel, SW_HIDE);
                g_panelVisible = false;
            }
        }
        return;
    }

    int x = 0, y = 0, width = 0, height = 0;
    if (!ComputeWidgetPosition(hwnd, &x, &y, &width, &height)) {
        return;
    }

    RECT current{};
    GetWindowRect(hwnd, &current);
    if (current.left != x || current.top != y ||
        (current.right - current.left) != width ||
        (current.bottom - current.top) != height) {
        SetWindowPos(hwnd, HWND_TOPMOST, x, y, width, height,
                     SWP_NOACTIVATE | SWP_NOOWNERZORDER);
    }

    if (!IsWindowVisible(hwnd)) {
        ShowWindow(hwnd, SW_SHOWNOACTIVATE);
    }
}

// --- Taskbar event hook ----------------------------------------------------
static bool IsTaskbarWindow(HWND hwnd) {
    if (!hwnd) {
        return false;
    }
    WCHAR className[64] = {};
    GetClassNameW(hwnd, className, ARRAYSIZE(className));
    return wcscmp(className, L"Shell_TrayWnd") == 0 ||
           wcscmp(className, L"Shell_SecondaryTrayWnd") == 0;
}

// EVENT_OBJECT_LOCATIONCHANGE fires very often on the taskbar thread, so the cheap
// checks come first: the hooked window itself and the window object. Only then is
// the cross-process GetClassNameW inside IsTaskbarWindow worth paying for.
static void CALLBACK TaskbarEventProc(HWINEVENTHOOK,
                                      DWORD,
                                      HWND hwnd,
                                      LONG idObject,
                                      LONG,
                                      DWORD,
                                      DWORD) {
    if (!g_hWidget || idObject != OBJID_WINDOW) {
        return;
    }
    if (hwnd != g_hookedTaskbar && !IsTaskbarWindow(hwnd)) {
        return;
    }
    PostMessage(g_hWidget, APP_WM_REPOSITION, 0, 0);
}

static void RegisterTaskbarHook(HWND hwnd) {
    if (g_taskbarHook) {
        UnhookWinEvent(g_taskbarHook);
        g_taskbarHook = nullptr;
    }

    HWND taskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (taskbar) {
        DWORD processId = 0;
        DWORD threadId = GetWindowThreadProcessId(taskbar, &processId);
        if (threadId) {
            g_taskbarHook = SetWinEventHook(
                EVENT_OBJECT_LOCATIONCHANGE, EVENT_OBJECT_LOCATIONCHANGE, nullptr,
                TaskbarEventProc, processId, threadId,
                WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
        }
        g_hookedTaskbar = taskbar;
    }
    PostMessage(hwnd, APP_WM_REPOSITION, 0, 0);
}

// --- Tooltip ---------------------------------------------------------------
static std::wstring BuildTooltipText() {
    ModSettings s = GetSettings();
    NetSnapshot snapshot;
    {
        std::lock_guard<std::mutex> guard(g_snapshotMutex);
        snapshot = g_snapshot;
    }

    std::wstring text = L"Network\n\n";
    text += L"Download: " + FormatSpeed(snapshot.downBytesPerSec, s) + L"\n";
    text += L"Upload: " + FormatSpeed(snapshot.upBytesPerSec, s) + L"\n\n";
    text += (s.counterMode == CounterMode::Persistent ? L"Total Download: "
                                                      : L"Session Download: ");
    text += FormatBytes(snapshot.totalDown, s) + L"\n";
    text += (s.counterMode == CounterMode::Persistent ? L"Total Upload: "
                                                      : L"Session Upload: ");
    text += FormatBytes(snapshot.totalUp, s) + L"\n\n";
    text += L"Interface: " + snapshot.ifName;
    if (!snapshot.ifKind.empty() && snapshot.ifKind != snapshot.ifName) {
        text += L" (" + snapshot.ifKind + L")";
    }
    text += L"\n";
    text += L"Status: ";
    text += snapshot.connected ? L"Connected" : L"Disconnected";
    if (!snapshot.ipv4.empty()) {
        text += L"\nIPv4: " + snapshot.ipv4;
    }
    return text;
}

static void CreateTooltipFor(HWND owner) {
    if (g_hTooltip) {
        return;
    }

    g_hTooltip = CreateWindowExW(WS_EX_TOPMOST, TOOLTIPS_CLASSW, nullptr,
                                 WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP, 0, 0, 0,
                                 0, owner, nullptr, GetModuleHandleW(nullptr),
                                 nullptr);
    if (!g_hTooltip) {
        return;
    }

    static std::wstring tooltipText;
    tooltipText = BuildTooltipText();

    TTTOOLINFOW info{};
    info.cbSize = sizeof(info);
    info.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
    info.hwnd = owner;
    info.uId = (UINT_PTR)owner;
    info.lpszText = (LPWSTR)tooltipText.c_str();
    SendMessageW(g_hTooltip, TTM_ADDTOOLW, 0, (LPARAM)&info);
    SendMessageW(g_hTooltip, TTM_SETMAXTIPWIDTH, 0, 320);
}

static void UpdateTooltipText(HWND owner) {
    if (!g_hTooltip) {
        return;
    }
    static std::wstring tooltipText;
    tooltipText = BuildTooltipText();

    TTTOOLINFOW info{};
    info.cbSize = sizeof(info);
    info.hwnd = owner;
    info.uId = (UINT_PTR)owner;
    info.lpszText = (LPWSTR)tooltipText.c_str();
    SendMessageW(g_hTooltip, TTM_UPDATETIPTEXTW, 0, (LPARAM)&info);
}

static void DestroyTooltip() {
    if (g_hTooltip) {
        DestroyWindow(g_hTooltip);
        g_hTooltip = nullptr;
    }
}

// --- Reset helpers ---------------------------------------------------------
static void RequestReset(bool download, bool upload) {
    unsigned long long flags = (download ? 1ull : 0ull) | (upload ? 2ull : 0ull);
    if (!flags) {
        return;
    }
    g_pendingReset.fetch_or(flags);
    if (g_wakeEvent) {
        SetEvent(g_wakeEvent);
    }
}

// --- Details panel window --------------------------------------------------
static LRESULT CALLBACK PanelWndProc(HWND hwnd,
                                     UINT msg,
                                     WPARAM wParam,
                                     LPARAM lParam) {
    switch (msg) {
        case WM_CREATE:
            UpdateAppearance(hwnd);
            return 0;

        case WM_ERASEBKGND:
            return 1;

        case WM_SETTINGCHANGE:
        case WM_THEMECHANGED:
        case WM_DPICHANGED:
            UpdateAppearance(hwnd);
            InvalidateRect(hwnd, nullptr, TRUE);
            return 0;

        case WM_ACTIVATE:
            if (LOWORD(wParam) == WA_INACTIVE) {
                KillTimer(hwnd, IDT_PANEL_DISMISS);
                ShowWindow(hwnd, SW_HIDE);
                g_panelVisible = false;
                // Clicking the widget while the panel is open deactivates the
                // panel first, so remember when that happened; TogglePanel uses
                // it to avoid immediately re-opening the panel.
                g_panelHiddenAt = GetTickCount64();
            }
            return 0;

        case WM_TIMER:
            if (wParam == IDT_PANEL_DISMISS) {
                // Dismiss on a mouse-down anywhere outside the panel and outside
                // the widget (the widget's own click toggles the panel itself).
                // This is the path that actually runs, because a background
                // process is normally refused foreground activation.
                if (!g_panelVisible.load()) {
                    KillTimer(hwnd, IDT_PANEL_DISMISS);
                    return 0;
                }
                if ((GetAsyncKeyState(VK_LBUTTON) & 0x8000) == 0 &&
                    (GetAsyncKeyState(VK_RBUTTON) & 0x8000) == 0) {
                    return 0;
                }
                POINT cursor{};
                GetCursorPos(&cursor);
                RECT panelRect{}, widgetRect{};
                GetWindowRect(hwnd, &panelRect);
                if (PtInRect(&panelRect, cursor)) {
                    return 0;
                }
                if (g_hWidget && GetWindowRect(g_hWidget, &widgetRect) &&
                    PtInRect(&widgetRect, cursor)) {
                    return 0;
                }
                KillTimer(hwnd, IDT_PANEL_DISMISS);
                ShowWindow(hwnd, SW_HIDE);
                g_panelVisible = false;
                g_panelHiddenAt = GetTickCount64();
            }
            return 0;

        case WM_MOUSEMOVE: {
            double scale = GetScaleForWindow(hwnd);
            PanelMetrics metrics = ComputePanelMetrics(scale);
            POINT point{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            int newState = PtInRect(&metrics.resetButton, point) ? 1 : 0;
            if (newState != g_panelHover.load()) {
                g_panelHover = newState;
                InvalidateRect(hwnd, nullptr, FALSE);
            }
            TRACKMOUSEEVENT track{sizeof(TRACKMOUSEEVENT), TME_LEAVE, hwnd, 0};
            TrackMouseEvent(&track);
            return 0;
        }

        case WM_MOUSELEAVE:
            if (g_panelHover.load() != 0) {
                g_panelHover = 0;
                InvalidateRect(hwnd, nullptr, FALSE);
            }
            return 0;

        case WM_LBUTTONUP: {
            double scale = GetScaleForWindow(hwnd);
            PanelMetrics metrics = ComputePanelMetrics(scale);
            POINT point{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            if (PtInRect(&metrics.resetButton, point)) {
                RequestReset(true, true);
                Wh_Log(L"Traffic counters reset from the details panel");
                InvalidateRect(hwnd, nullptr, FALSE);
                if (g_hWidget) {
                    InvalidateRect(g_hWidget, nullptr, FALSE);
                }
            }
            return 0;
        }

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            RECT client{};
            GetClientRect(hwnd, &client);

            HDC memDC = CreateCompatibleDC(hdc);
            if (memDC) {
                HBITMAP bitmap =
                    CreateCompatibleBitmap(hdc, client.right, client.bottom);
                if (bitmap) {
                    HBITMAP oldBitmap = (HBITMAP)SelectObject(memDC, bitmap);
                    DrawDetailsPanel(memDC, client.right, client.bottom, hwnd);
                    BitBlt(hdc, 0, 0, client.right, client.bottom, memDC, 0, 0,
                           SRCCOPY);
                    SelectObject(memDC, oldBitmap);
                    DeleteObject(bitmap);
                }
                DeleteDC(memDC);
            }
            EndPaint(hwnd, &ps);
            return 0;
        }

        case WM_CLOSE:
            return 0;

        case APP_WM_CLOSE:
            DestroyWindow(hwnd);
            return 0;

        case WM_DESTROY:
            g_panelVisible = false;
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

static void TogglePanel(HWND widget) {
    if (!g_hPanel) {
        return;
    }

    if (g_panelVisible.load()) {
        KillTimer(g_hPanel, IDT_PANEL_DISMISS);
        ShowWindow(g_hPanel, SW_HIDE);
        g_panelVisible = false;
        g_panelHiddenAt = GetTickCount64();
        return;
    }

    // If the panel was dismissed by this very click (the panel loses activation
    // before the widget sees WM_LBUTTONUP), treat the click as "close" only.
    unsigned long long hiddenAt = g_panelHiddenAt.load();
    if (hiddenAt && GetTickCount64() - hiddenAt < 250) {
        return;
    }

    double scale = GetScaleForWindow(widget);
    PanelMetrics metrics = ComputePanelMetrics(scale);

    RECT widgetRect{};
    GetWindowRect(widget, &widgetRect);

    TaskbarInfo taskbar;
    bool haveTaskbar = GetTaskbarInfo(&taskbar);
    int gap = (int)(8 * scale);

    int x = widgetRect.left;
    int y = widgetRect.top - metrics.height - gap;

    if (haveTaskbar) {
        switch (taskbar.edge) {
            case TaskbarEdge::Top:
                y = taskbar.rect.bottom + gap;
                break;
            case TaskbarEdge::Left:
                x = taskbar.rect.right + gap;
                y = widgetRect.top;
                break;
            case TaskbarEdge::Right:
                x = taskbar.rect.left - metrics.width - gap;
                y = widgetRect.top;
                break;
            case TaskbarEdge::Bottom:
            default:
                y = taskbar.rect.top - metrics.height - gap;
                break;
        }
    }

    HMONITOR monitor = MonitorFromWindow(widget, MONITOR_DEFAULTTONEAREST);
    MONITORINFO monitorInfo{sizeof(MONITORINFO)};
    if (GetMonitorInfoW(monitor, &monitorInfo)) {
        const RECT& work = monitorInfo.rcWork;
        if (x + metrics.width > work.right) {
            x = work.right - metrics.width;
        }
        if (x < work.left) {
            x = work.left;
        }
        if (y < monitorInfo.rcMonitor.top) {
            y = monitorInfo.rcMonitor.top;
        }
    }

    SetWindowPos(g_hPanel, HWND_TOPMOST, x, y, metrics.width, metrics.height,
                 SWP_SHOWWINDOW | SWP_NOACTIVATE);
    g_panelVisible = true;
    InvalidateRect(g_hPanel, nullptr, TRUE);
    // Try for activation (it makes WM_ACTIVATE dismissal work when it succeeds),
    // but do not depend on it: SetForegroundWindow is normally refused for a
    // background process, so a timer polls the mouse as the reliable path.
    SetForegroundWindow(g_hPanel);
    SetTimer(g_hPanel, IDT_PANEL_DISMISS, 120, nullptr);
}

// --- Divider drag hit testing ----------------------------------------------
// The base x is the un-nudged split point: it must be computed the same way the
// painter does, so a drag offset of 0 leaves the line exactly where the layout
// puts it.
static REAL DividerBaseX(HWND hwnd, int width) {
    ModSettings s = GetSettings();
    const double scale = GetScaleForWindow(hwnd);
    REAL padding = (REAL)(9.0 * scale);
    REAL columnGap = (REAL)(10.0 * scale);
    REAL usable = (REAL)width - padding * 2.0f - columnGap;
    REAL leftWidth = usable * (s.dividerPos / 100.0f);
    return padding + leftWidth + columnGap / 2.0f;
}

// True when the pointer is within the grab zone of the line that was last
// painted. Uses the painted x (published by the renderer) rather than
// recomputing it, so hover can never disagree with what is on screen.
static bool DividerHover(HWND hwnd, int mouseX, const ModSettings& s) {
    if (!s.dividerDraggable || s.layout != LayoutMode::Full) {
        return false;
    }
    float painted = g_dividerHitX.load();
    if (painted < 0.0f) {
        return false;  // no divider on screen (other layout / not painted yet)
    }
    const double scale = GetScaleForWindow(hwnd);
    // 5 logical px each side: wide enough to grab without stealing the click
    // that opens the details panel.
    const float slack = (float)(5.0 * scale);
    return mouseX >= painted - slack && mouseX <= painted + slack;
}

// --- Context menu ----------------------------------------------------------
// Apply a live display-mode change: atomic first (painters read it), then
// storage, then repaint. Settings are NOT reloaded - the setting is only the
// default and would stomp the menu choice.
static void ApplyDisplayMode(HWND hwnd, int mode) {
    g_displayMode = ClampInt(mode, 0, 2);
    Wh_SetIntValue(L"DisplayMode", g_displayMode.load());
    InvalidateRect(hwnd, nullptr, TRUE);
    if (g_hPanel && g_panelVisible.load()) {
        InvalidateRect(g_hPanel, nullptr, TRUE);
    }
}

static void ApplyCaptionBits(HWND hwnd, int bits) {
    g_captionBits = ClampInt(bits, 0, 3);
    Wh_SetIntValue(L"CaptionBits", g_captionBits.load());
    InvalidateRect(hwnd, nullptr, TRUE);
}

static void ApplyArrowsHidden(HWND hwnd, bool hidden) {
    g_arrowsHidden = hidden ? 1 : 0;
    Wh_SetIntValue(L"ArrowsHidden", g_arrowsHidden.load());
    InvalidateRect(hwnd, nullptr, TRUE);
    if (g_hPanel && g_panelVisible.load()) {
        InvalidateRect(g_hPanel, nullptr, TRUE);
    }
}

static void ShowContextMenu(HWND hwnd) {
    HMENU menu = CreatePopupMenu();
    if (!menu) {
        return;
    }

    AppendMenuW(menu, MF_STRING, IDM_REFRESH, L"Refresh");
    AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
    {
        // What the widget shows: speed, traffic, or both. Radio check marks so
        // the current choice is obvious and one click switches.
        const int mode = ClampInt(g_displayMode.load(), 0, 2);
        AppendMenuW(menu, MF_STRING | MF_RADIOCHECK | MF_GROUPCHECK |
                                (mode == 0 ? MF_CHECKED : MF_UNCHECKED),
                    IDM_SHOW_BOTH, L"Show both");
        AppendMenuW(menu, MF_STRING | MF_RADIOCHECK |
                                (mode == 1 ? MF_CHECKED : MF_UNCHECKED),
                    IDM_SHOW_SPEED, L"Show speed only");
        AppendMenuW(menu, MF_STRING | MF_RADIOCHECK |
                                (mode == 2 ? MF_CHECKED : MF_UNCHECKED),
                    IDM_SHOW_TRAFFIC, L"Show traffic only");
        AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
        // The small captions above the two halves, and the arrows themselves.
        const int bits = ClampInt(g_captionBits.load(), 0, 3);
        AppendMenuW(menu, MF_STRING |
                                (bits & 1 ? MF_CHECKED : MF_UNCHECKED),
                    IDM_TOGGLE_CAPTION_SPEED, L"Show SPEED caption");
        AppendMenuW(menu, MF_STRING |
                                (bits & 2 ? MF_CHECKED : MF_UNCHECKED),
                    IDM_TOGGLE_CAPTION_TOTAL, L"Show TOTAL caption");
        AppendMenuW(menu, MF_STRING |
                                (g_arrowsHidden.load() ? MF_UNCHECKED
                                                       : MF_CHECKED),
                    IDM_TOGGLE_ARROWS, L"Show arrows");
        AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
    }
    AppendMenuW(menu, MF_STRING, IDM_RESET_BOTH, L"Reset counters (both)");
    AppendMenuW(menu, MF_STRING, IDM_RESET_DL, L"Reset download");
    AppendMenuW(menu, MF_STRING, IDM_RESET_UL, L"Reset upload");
    AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
    {
        ModSettings s = GetSettings();
        // Only offered when there is something to undo, so the menu does not grow
        // an item that does nothing.
        if (s.dividerDraggable && s.layout == LayoutMode::Full &&
            g_dividerOffset.load() != 0) {
            AppendMenuW(menu, MF_STRING, IDM_RESET_DIVIDER,
                        L"Reset divider position");
            AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
        }
    }
    AppendMenuW(menu, MF_STRING, IDM_NET_SETTINGS, L"Open Network Settings");
    AppendMenuW(menu, MF_STRING, IDM_WH_SETTINGS, L"Windhawk Settings");
    AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(menu, MF_STRING, IDM_HIDE, L"Hide widget");

    POINT cursor{};
    GetCursorPos(&cursor);

    SetForegroundWindow(hwnd);
    UINT command = TrackPopupMenu(menu,
                                  TPM_RIGHTBUTTON | TPM_RETURNCMD | TPM_NONOTIFY,
                                  cursor.x, cursor.y, 0, hwnd, nullptr);
    // The classic TrackPopupMenu fix: SetForegroundWindow may be refused on a
    // WS_EX_NOACTIVATE window, and the menu then lingers after a click elsewhere.
    // A dummy message to the owner nudges it out of menu mode.
    PostMessage(hwnd, WM_NULL, 0, 0);
    DestroyMenu(menu);

    switch (command) {
        case IDM_REFRESH:
            if (g_wakeEvent) {
                SetEvent(g_wakeEvent);
            }
            InvalidateRect(hwnd, nullptr, FALSE);
            break;
        case IDM_RESET_DL:
            RequestReset(true, false);
            break;
        case IDM_RESET_UL:
            RequestReset(false, true);
            break;
        case IDM_RESET_BOTH:
            RequestReset(true, true);
            break;
        case IDM_RESET_DIVIDER:
            g_dividerOffset = 0;
            Wh_SetIntValue(L"DividerOffset", 0);
            LoadSettings();
            InvalidateRect(hwnd, nullptr, FALSE);
            break;
        case IDM_SHOW_BOTH:
            ApplyDisplayMode(hwnd, 0);
            break;
        case IDM_SHOW_SPEED:
            ApplyDisplayMode(hwnd, 1);
            break;
        case IDM_SHOW_TRAFFIC:
            ApplyDisplayMode(hwnd, 2);
            break;
        case IDM_TOGGLE_CAPTION_SPEED:
            ApplyCaptionBits(hwnd, g_captionBits.load() ^ 1);
            break;
        case IDM_TOGGLE_CAPTION_TOTAL:
            ApplyCaptionBits(hwnd, g_captionBits.load() ^ 2);
            break;
        case IDM_TOGGLE_ARROWS:
            ApplyArrowsHidden(hwnd, !g_arrowsHidden.load());
            break;
        case IDM_NET_SETTINGS:
            ShellExecuteW(nullptr, L"open", L"ms-settings:network", nullptr,
                          nullptr, SW_SHOWNORMAL);
            break;
        case IDM_WH_SETTINGS: {
            // The helper process is an explorer.exe image, so
            // GetModuleFileNameW(nullptr, ...) would return explorer.exe and just
            // open a File Explorer window. Resolve Windhawk's own path instead.
            WCHAR path[MAX_PATH];
            if (ExpandEnvironmentStringsW(L"%ProgramFiles%\\Windhawk\\windhawk.exe",
                                          path, ARRAYSIZE(path))) {
                ShellExecuteW(nullptr, L"open", path, nullptr, nullptr,
                              SW_SHOWNORMAL);
            }
            break;
        }
        case IDM_HIDE:
            g_hiddenByUser = true;
            ShowWindow(hwnd, SW_HIDE);
            if (g_hPanel && g_panelVisible.load()) {
                ShowWindow(g_hPanel, SW_HIDE);
                g_panelVisible = false;
            }
            Wh_Log(L"Widget hidden from the context menu; re-enable it by reloading the mod or changing settings");
            break;
        default:
            break;
    }
}
// --- Widget window ---------------------------------------------------------
static LRESULT CALLBACK WidgetWndProc(HWND hwnd,
                                      UINT msg,
                                      WPARAM wParam,
                                      LPARAM lParam) {
    switch (msg) {
        case WM_CREATE:
            UpdateAppearance(hwnd);
            // Watchdog: catches taskbar restarts, monitor/DPI/resolution changes
            // and fullscreen transitions that produce no WinEvent for us.
            SetTimer(hwnd, IDT_WATCHDOG, 2000, nullptr);
            RegisterTaskbarHook(hwnd);
            return 0;

        case WM_ERASEBKGND:
            return 1;

        case WM_CLOSE:
            return 0;

        case APP_WM_CLOSE:
            DestroyWindow(hwnd);
            return 0;

        case WM_DESTROY:
            KillTimer(hwnd, IDT_WATCHDOG);
            if (g_taskbarHook) {
                UnhookWinEvent(g_taskbarHook);
                g_taskbarHook = nullptr;
            }
            DestroyTooltip();
            PostQuitMessage(0);
            return 0;

        case WM_SETTINGCHANGE:
        case WM_THEMECHANGED:
        case WM_DWMCOMPOSITIONCHANGED:
        case APP_WM_SETTINGS_CHANGED:
            UpdateAppearance(hwnd);
            InvalidateRect(hwnd, nullptr, TRUE);
            if (g_hPanel) {
                UpdateAppearance(g_hPanel);
                InvalidateRect(g_hPanel, nullptr, TRUE);
            }
            return 0;

        case WM_DPICHANGED:
        case WM_DISPLAYCHANGE:
            PostMessage(hwnd, APP_WM_REPOSITION, 0, 0);
            InvalidateRect(hwnd, nullptr, TRUE);
            return 0;

        case WM_TIMER:
            if (wParam == IDT_WATCHDOG) {
                HWND taskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
                if (taskbar != g_hookedTaskbar) {
                    // Explorer restarted: re-hook and reposition.
                    Wh_Log(L"Taskbar window changed, re-registering hook");
                    RegisterTaskbarHook(hwnd);
                }
                UpdateFullscreenHideState(hwnd);
                RepositionWidget(hwnd);
            }
            return 0;

        case APP_WM_REPOSITION:
            RepositionWidget(hwnd);
            return 0;

        case APP_WM_DATA_UPDATED:
            InvalidateRect(hwnd, nullptr, FALSE);
            if (g_hPanel && g_panelVisible.load()) {
                InvalidateRect(g_hPanel, nullptr, FALSE);
            }
            if (GetSettings().showTooltip) {
                UpdateTooltipText(hwnd);
            }
            return 0;

        case APP_WM_TOGGLE_PANEL:
            TogglePanel(hwnd);
            return 0;

        case WM_MOUSEMOVE: {
            const int mouseX = GET_X_LPARAM(lParam);
            ModSettings settings = GetSettings();

            // --- divider drag ---------------------------------------------
            if (g_dividerDragging.load()) {
                RECT client{};
                GetClientRect(hwnd, &client);
                const double scale = GetScaleForWindow(hwnd);
                // The offset is stored in logical px so it stays correct if the
                // monitor scaling changes later.
                int base = (int)DividerBaseX(hwnd, client.right);
                int offset = (int)((mouseX - base) / (scale > 0 ? scale : 1.0));
                g_dividerOffset = offset;
                InvalidateRect(hwnd, nullptr, FALSE);
                return 0;
            }

            const int state = DividerHover(hwnd, mouseX, settings) ? 2 : 1;
            if (g_widgetHover.load() != state) {
                g_widgetHover = state;
                InvalidateRect(hwnd, nullptr, FALSE);
            }
            TRACKMOUSEEVENT track{sizeof(TRACKMOUSEEVENT), TME_LEAVE, hwnd, 0};
            TrackMouseEvent(&track);
            return 0;
        }

        case WM_SETCURSOR:
            // A west-east cursor over the grab zone is the only affordance that
            // says "this line can be dragged".
            if (LOWORD(lParam) == HTCLIENT &&
                (g_dividerDragging.load() || g_widgetHover.load() == 2)) {
                SetCursor(LoadCursorW(nullptr, IDC_SIZEWE));
                return TRUE;
            }
            break;

        case WM_MOUSELEAVE:
            if (g_widgetHover.load() != 0 && !g_dividerDragging.load()) {
                g_widgetHover = 0;
                InvalidateRect(hwnd, nullptr, FALSE);
            }
            return 0;

        case WM_LBUTTONDOWN: {
            ModSettings settings = GetSettings();
            if (DividerHover(hwnd, GET_X_LPARAM(lParam), settings)) {
                g_dividerDragging = true;
                SetCapture(hwnd);
                InvalidateRect(hwnd, nullptr, FALSE);
                return 0;
            }
            break;
        }

        case WM_LBUTTONUP:
            if (g_dividerDragging.load()) {
                g_dividerDragging = false;
                ReleaseCapture();
                // Persist only on release: writing on every mouse move would hit
                // the registry dozens of times per drag.
                Wh_SetIntValue(L"DividerOffset", g_dividerOffset.load());
                LoadSettings();
                InvalidateRect(hwnd, nullptr, FALSE);
                return 0;  // a drag is not a click - don't open the panel
            }
            if (GetSettings().showDetails) {
                TogglePanel(hwnd);
            }
            return 0;

        case WM_MBUTTONDOWN:
            // Middle click cycles the display: both -> speed -> traffic -> both.
            // A full cycle so the user can always get back to the two-column
            // view without opening the menu.
            if (GetSettings().cycleOnMiddleClick) {
                const int mode = ClampInt(g_displayMode.load(), 0, 2);
                ApplyDisplayMode(hwnd, (mode + 1) % 3);
            }
            return 0;

        case WM_RBUTTONUP:
            ShowContextMenu(hwnd);
            return 0;

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            RECT client{};
            GetClientRect(hwnd, &client);

            HDC memDC = CreateCompatibleDC(hdc);
            if (memDC) {
                HBITMAP bitmap =
                    CreateCompatibleBitmap(hdc, client.right, client.bottom);
                if (bitmap) {
                    HBITMAP oldBitmap = (HBITMAP)SelectObject(memDC, bitmap);
                    DrawNetworkPanel(memDC, client.right, client.bottom, hwnd);
                    BitBlt(hdc, 0, 0, client.right, client.bottom, memDC, 0, 0,
                           SRCCOPY);
                    SelectObject(memDC, oldBitmap);
                    DeleteObject(bitmap);
                }
                DeleteDC(memDC);
            }
            EndPaint(hwnd, &ps);
            return 0;
        }

        default:
            if (g_taskbarCreatedMsg && msg == g_taskbarCreatedMsg) {
                Wh_Log(L"TaskbarCreated received, re-attaching");
                RegisterTaskbarHook(hwnd);
                return 0;
            }
            break;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

// --- UI thread -------------------------------------------------------------
// Owns both windows, the GDI+ token and the message loop. All window handling
// happens here; the network worker only posts messages.
static void UiThread() {
    // Publish the thread id first: shutdown posts WM_QUIT here, and it must work
    // even if the mod is torn down before any window exists.
    g_uiThreadId = GetCurrentThreadId();

    // Per-monitor DPI so GetDpiForWindow reports the real monitor scaling.
    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    if (user32) {
        using SetThreadDpiAwarenessContext_t =
            DPI_AWARENESS_CONTEXT(WINAPI*)(DPI_AWARENESS_CONTEXT);
        auto setThreadDpiAwarenessContext =
            (SetThreadDpiAwarenessContext_t)GetProcAddress(
                user32, "SetThreadDpiAwarenessContext");
        if (setThreadDpiAwarenessContext) {
            setThreadDpiAwarenessContext(
                DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
        }
    }

    HRESULT comInit = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    GdiplusStartupInput startupInput;
    if (GdiplusStartup(&g_gdiplusToken, &startupInput, nullptr) != Ok) {
        Wh_Log(L"GdiplusStartup failed");
        g_gdiplusToken = 0;
    }

    INITCOMMONCONTROLSEX iccex{sizeof(INITCOMMONCONTROLSEX), ICC_BAR_CLASSES};
    InitCommonControlsEx(&iccex);

    g_taskbarCreatedMsg = RegisterWindowMessageW(L"TaskbarCreated");

    HINSTANCE instance = GetModuleHandleW(nullptr);

    WNDCLASSW widgetClass{};
    widgetClass.lpfnWndProc = WidgetWndProc;
    widgetClass.hInstance = instance;
    widgetClass.lpszClassName = kWidgetClass;
    widgetClass.hCursor = LoadCursorW(nullptr, IDC_HAND);
    RegisterClassW(&widgetClass);

    WNDCLASSW panelClass{};
    panelClass.lpfnWndProc = PanelWndProc;
    panelClass.hInstance = instance;
    panelClass.lpszClassName = kPanelClass;
    panelClass.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    RegisterClassW(&panelClass);

    ModSettings settings = GetSettings();
    const DWORD exStyle = WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST |
                          WS_EX_NOACTIVATE;

    // Prefer CreateWindowInBand so the widget can sit above the taskbar band,
    // exactly like the reference mod; fall back to CreateWindowEx when the
    // undocumented export is unavailable.
    pCreateWindowInBand createWindowInBand = nullptr;
    if (user32) {
        createWindowInBand =
            (pCreateWindowInBand)GetProcAddress(user32, "CreateWindowInBand");
    }

    if (createWindowInBand) {
        g_hWidget = createWindowInBand(
            exStyle, kWidgetClass, L"Taskbar Network Lounge", WS_POPUP, 0, 0,
            settings.width, settings.height, nullptr, nullptr, instance, nullptr,
            ZBID_IMMERSIVE_NOTIFICATION);
        if (g_hWidget) {
            Wh_Log(L"Widget created in ZBID_IMMERSIVE_NOTIFICATION band");
        }
    }

    if (!g_hWidget) {
        Wh_Log(L"CreateWindowInBand unavailable, falling back to CreateWindowEx");
        g_hWidget =
            CreateWindowExW(exStyle, kWidgetClass, L"Taskbar Network Lounge",
                            WS_POPUP, 0, 0, settings.width, settings.height,
                            nullptr, nullptr, instance, nullptr);
    }

    if (!g_hWidget) {
        Wh_Log(L"Failed to create the widget window");
        if (g_gdiplusToken) {
            GdiplusShutdown(g_gdiplusToken);
            g_gdiplusToken = 0;
        }
        if (SUCCEEDED(comInit)) {
            CoUninitialize();
        }
        return;
    }

    SetLayeredWindowAttributes(g_hWidget, 0, 255, LWA_ALPHA);

    // Details panel: same acrylic treatment, created hidden. It is activatable
    // (no WS_EX_NOACTIVATE) so clicking elsewhere dismisses it.
    double scale = GetScaleForWindow(g_hWidget);
    PanelMetrics metrics = ComputePanelMetrics(scale);
    g_hPanel = CreateWindowExW(WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST,
                               kPanelClass, L"Network Details", WS_POPUP, 0, 0,
                               metrics.width, metrics.height, nullptr, nullptr,
                               instance, nullptr);
    if (g_hPanel) {
        SetLayeredWindowAttributes(g_hPanel, 0, 255, LWA_ALPHA);
    } else {
        Wh_Log(L"Failed to create the details panel window");
    }

    if (settings.showTooltip) {
        CreateTooltipFor(g_hWidget);
    }

    RepositionWidget(g_hWidget);

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    // Ordered teardown: panel, tooltip, widget, classes, GDI+, COM. The widget is
    // destroyed explicitly because the loop can also exit via WM_QUIT (see
    // WhTool_ModUninit), in which case WM_DESTROY never ran - and a surviving
    // window keeps its timer and WinEvent hook alive and makes UnregisterClassW
    // fail.
    if (g_hPanel) {
        DestroyWindow(g_hPanel);
        g_hPanel = nullptr;
    }
    DestroyTooltip();
    if (g_hWidget) {
        DestroyWindow(g_hWidget);
        g_hWidget = nullptr;
    }

    UnregisterClassW(kPanelClass, instance);
    UnregisterClassW(kWidgetClass, instance);

    if (g_gdiplusToken) {
        GdiplusShutdown(g_gdiplusToken);
        g_gdiplusToken = 0;
    }
    if (SUCCEEDED(comInit)) {
        CoUninitialize();
    }
}

// --- Windhawk tool mod callbacks -------------------------------------------
BOOL WhTool_ModInit() {
    Wh_Log(L"Taskbar Network Lounge starting");

    // Seed the live divider offset from storage BEFORE the first settings read,
    // so a dragged divider is already in place on the first paint. Same for the
    // menu-toggled display state (mode, captions, arrows).
    g_dividerOffset = Wh_GetIntValue(L"DividerOffset", 0);
    g_displayMode = ClampInt(Wh_GetIntValue(L"DisplayMode", 0), 0, 2);
    g_captionBits = ClampInt(Wh_GetIntValue(L"CaptionBits", 3), 0, 3);
    g_arrowStyle = ClampInt(Wh_GetIntValue(L"ArrowStyle", (int)ArrowStyle::Rounded),
                            0, (int)ArrowStyle::Plus);
    g_arrowsHidden = Wh_GetIntValue(L"ArrowsHidden", 0) ? 1 : 0;

    LoadSettings();
    ApplyResetSettingIfChanged();

    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_wakeEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (!g_stopEvent || !g_wakeEvent) {
        Wh_Log(L"CreateEvent failed");
        if (g_stopEvent) {
            CloseHandle(g_stopEvent);
            g_stopEvent = nullptr;
        }
        if (g_wakeEvent) {
            CloseHandle(g_wakeEvent);
            g_wakeEvent = nullptr;
        }
        return FALSE;
    }

    try {
        g_uiThread = new std::thread(UiThread);
        g_workerThread = new std::thread(NetworkWorkerThread);
    } catch (...) {
        Wh_Log(L"Failed to start threads");
        return FALSE;
    }

    return TRUE;
}

void WhTool_ModUninit() {
    Wh_Log(L"Taskbar Network Lounge stopping");
    if (g_stopEvent) {
        SetEvent(g_stopEvent);
    }
    if (g_wakeEvent) {
        SetEvent(g_wakeEvent);
    }

    if (g_workerThread) {
        if (g_workerThread->joinable()) {
            g_workerThread->join();
        }
        delete g_workerThread;
        g_workerThread = nullptr;
    }

    // End the UI thread's message loop. WM_QUIT goes to the THREAD rather than to
    // the widget: if uninit runs before UiThread created its window (mod disabled
    // or reloaded immediately after start) a window-targeted message would be
    // dropped and the join() below would block forever. The loop may also have
    // exited already; PostThreadMessageW simply fails then, which is harmless.
    DWORD uiThreadId = g_uiThreadId.load();
    if (uiThreadId != 0) {
        PostThreadMessageW(uiThreadId, WM_QUIT, 0, 0);
    }

    if (g_uiThread) {
        if (g_uiThread->joinable()) {
            g_uiThread->join();
        }
        delete g_uiThread;
        g_uiThread = nullptr;
    }
    g_uiThreadId = 0;

    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }
    if (g_wakeEvent) {
        CloseHandle(g_wakeEvent);
        g_wakeEvent = nullptr;
    }
}

void WhTool_ModSettingsChanged() {
    LoadSettings();
    ApplyResetSettingIfChanged();

    // Turning "Start enabled" back on un-hides a widget hidden from the menu.
    if (GetSettings().startEnabled) {
        g_hiddenByUser = false;
    }

    if (g_hWidget) {
        PostMessage(g_hWidget, APP_WM_SETTINGS_CHANGED, 0, 0);
        PostMessage(g_hWidget, APP_WM_REPOSITION, 0, 0);
        PostMessage(g_hWidget, APP_WM_DATA_UPDATED, 0, 0);
    }
    if (g_wakeEvent) {
        SetEvent(g_wakeEvent);
    }
}

////////////////////////////////////////////////////////////////////////////////
// Windhawk tool mod implementation for mods which don't need to inject to other
// processes or hook other functions. Context:
// https://github.com/ramensoftware/windhawk/wiki/Mods-as-tools:-Running-mods-in-a-dedicated-process
//
// The mod will load and run in a dedicated windhawk.exe process.
//
// Paste the code below as part of the mod code, and use these callbacks:
// * WhTool_ModInit
// * WhTool_ModSettingsChanged
// * WhTool_ModUninit
//
// Currently, other callbacks are not supported.

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}

BOOL Wh_ModInit() {
    Wh_Log(L"Wh_ModInit in pid %lu, command line: %s", GetCurrentProcessId(),
           GetCommandLine());

    // Never run in the session 0 service instance.
    DWORD sessionId = 0;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) &&
        sessionId == 0) {
        Wh_Log(L"Session 0, not loading");
        return FALSE;
    }

    bool isService = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);
    if (!argv) {
        Wh_Log(L"CommandLineToArgvW failed");
        return FALSE;
    }

    for (int i = 1; i < argc; i++) {
        if (wcscmp(argv[i], L"-service") == 0 ||
            wcscmp(argv[i], L"-service-start") == 0 ||
            wcscmp(argv[i], L"-service-stop") == 0) {
            isService = true;
            break;
        }
    }

    for (int i = 1; i < argc - 1; i++) {
        if (wcscmp(argv[i], L"-tool-mod") == 0) {
            isToolModProcess = true;
            if (wcscmp(argv[i + 1], WH_MOD_ID) == 0) {
                isCurrentToolModProcess = true;
            }
            break;
        }
    }

    LocalFree(argv);

    if (isService) {
        return FALSE;
    }

    if (isCurrentToolModProcess) {
        g_toolModProcessMutex =
            CreateMutex(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
        if (!g_toolModProcessMutex) {
            Wh_Log(L"CreateMutex failed");
            ExitProcess(1);
        }

        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            Wh_Log(L"Tool mod already running (%s)", WH_MOD_ID);
            ExitProcess(1);
        }

        if (!WhTool_ModInit()) {
            ExitProcess(1);
        }

        IMAGE_DOS_HEADER* dosHeader =
            (IMAGE_DOS_HEADER*)GetModuleHandle(nullptr);
        IMAGE_NT_HEADERS* ntHeaders =
            (IMAGE_NT_HEADERS*)((BYTE*)dosHeader + dosHeader->e_lfanew);

        DWORD entryPointRVA = ntHeaders->OptionalHeader.AddressOfEntryPoint;
        void* entryPoint = (BYTE*)dosHeader + entryPointRVA;

        Wh_SetFunctionHook(entryPoint, (void*)EntryPoint_Hook, nullptr);
        return TRUE;
    }

    if (isToolModProcess) {
        return FALSE;
    }

    g_isToolModProcessLauncher = true;
    return TRUE;
}

void Wh_ModAfterInit() {
    if (!g_isToolModProcessLauncher) {
        return;
    }

    Wh_Log(L"Launching the tool mod process");

    WCHAR currentProcessPath[MAX_PATH];
    switch (GetModuleFileName(nullptr, currentProcessPath,
                              ARRAYSIZE(currentProcessPath))) {
        case 0:
        case ARRAYSIZE(currentProcessPath):
            Wh_Log(L"GetModuleFileName failed");
            return;
    }

    WCHAR
    commandLine[MAX_PATH + 2 +
                (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) - 1];
    swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"", currentProcessPath,
               WH_MOD_ID);

    HMODULE kernelModule = GetModuleHandle(L"kernelbase.dll");
    if (!kernelModule) {
        kernelModule = GetModuleHandle(L"kernel32.dll");
        if (!kernelModule) {
            Wh_Log(L"No kernelbase.dll/kernel32.dll");
            return;
        }
    }

    using CreateProcessInternalW_t = BOOL(WINAPI*)(
        HANDLE hUserToken, LPCWSTR lpApplicationName, LPWSTR lpCommandLine,
        LPSECURITY_ATTRIBUTES lpProcessAttributes,
        LPSECURITY_ATTRIBUTES lpThreadAttributes, WINBOOL bInheritHandles,
        DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory,
        LPSTARTUPINFOW lpStartupInfo,
        LPPROCESS_INFORMATION lpProcessInformation,
        PHANDLE hRestrictedUserToken);
    CreateProcessInternalW_t pCreateProcessInternalW =
        (CreateProcessInternalW_t)GetProcAddress(kernelModule,
                                                 "CreateProcessInternalW");
    if (!pCreateProcessInternalW) {
        Wh_Log(L"No CreateProcessInternalW");
        return;
    }

    STARTUPINFO si{
        .cb = sizeof(STARTUPINFO),
        .dwFlags = STARTF_FORCEOFFFEEDBACK,
    };
    PROCESS_INFORMATION pi;
    if (!pCreateProcessInternalW(nullptr, currentProcessPath, commandLine,
                                 nullptr, nullptr, FALSE, NORMAL_PRIORITY_CLASS,
                                 nullptr, nullptr, &si, &pi, nullptr)) {
        Wh_Log(L"CreateProcess failed");
        return;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

void Wh_ModSettingsChanged() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModSettingsChanged();
}

void Wh_ModUninit() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModUninit();
    ExitProcess(0);
}
