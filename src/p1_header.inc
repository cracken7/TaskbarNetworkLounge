// ==WindhawkMod==
// @id              taskbar-network-lounge
// @name            Taskbar Network Lounge
// @description     Live network speed and traffic totals on the taskbar, in a native acrylic widget
// @version         1.2.0
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

A native network meter docked on the taskbar. Live download/upload speed, total
traffic, and an acrylic details panel — styled like the rest of the Windows 11
shell.

![Widget](https://raw.githubusercontent.com/cracken7/TaskbarNetworkLounge/main/docs/widget.png)

## Features

**Live speed and traffic totals.** Read from the real interface counters
(`GetIfTable2`, IP Helper) and divided by measured elapsed time
(`QueryPerformanceCounter`) — no `ipconfig`, `netstat` or PowerShell parsing.
Totals can be per session or persistent across restarts.

**Follows the connection you are actually using.** With a VPN running, the tunnel
adapter and the physical adapter both carry the same bytes — the tunnel sees the
plaintext, the NIC sees the encrypted copy — so summing them reports a 1 GB
download as 2 GB. Auto mode tracks the single adapter holding the default route,
so a VPN is followed while it carries the internet and dropped when it doesn't.
Traffic totals reset when the source changes (Ethernet → VPN → Wi-Fi), so they
always describe the connection in use. You can also force Ethernet, Wi-Fi, all
adapters, or one specific adapter.

**Adjustable appearance.** Five arrow styles, four text weights, font/arrow/widget
/panel sizes, and a divider between the SPEED and TOTAL columns that you can
**drag with the mouse** — only the line moves, the text stays put.

**Details panel** on click: interface, status, IPv4, speeds, totals and a Reset
button. Rich tooltip on hover, context menu on right click.

**Bytes vs bits, never mixed up.** `MB/s` is megabytes per second, `Mbps` is
megabits per second; the unit is always drawn next to the number.

**Cheap.** One API call per interval on a worker thread, repaint only when the
numbers change. Measured: 0.3–0.5 % of one core, 27–31 MB, no handle leaks.

## Requirements

Windows 11 for rounded corners and acrylic; works on Windows 10 with square
corners. If the Windows Widgets button overlaps the meter, turn it off in
Taskbar Settings → Widgets.

## Notes

* The widget runs in a dedicated `explorer.exe` helper process (Windhawk's
  "mods as tools" pattern), so a fault in it cannot take down the real shell. Two
  visible consequences: a second "Windows Explorer" entry appears in Task Manager,
  and other mods that target `explorer.exe` are injected into the helper too.
* Attaches to the primary taskbar (`Shell_TrayWnd`).
* Speeds are sampled, so a single reading can differ from Task Manager by a few
  percent; the average over a second matches (measured 0.06 % over 22 s).
* Source, tests and full documentation:
  [github.com/cracken7/TaskbarNetworkLounge](https://github.com/cracken7/TaskbarNetworkLounge)
* Design references: the window creation, z-band placement and acrylic treatment
  follow the approach used by **Taskbar Music Lounge** and **Taskbar Network Speed
  Indicator** (`net-speed-taskbar` by NarayanChetri), both read while building this.
  No code was copied from either.

---

# مؤشر الشبكة لشريط المهام

مؤشر شبكة أصلي يستقرّ على شريط المهام: سرعة التحميل والرفع الحيّة، وإجمالي
الترافيك، ولوحة تفاصيل زجاجية — بنفس هوية ويندوز 11.

## الميزات

**سرعة حيّة وإجماليات ترافيك.** تُقرأ من عدّادات كرت الشبكة الحقيقية
(`GetIfTable2`) وتُقسَم على الزمن المقيس فعليًّا (`QueryPerformanceCounter`) — بدون
قراءة مخرجات `ipconfig` أو `netstat` أو PowerShell. والإجماليات إما للجلسة أو
دائمة تُحفَظ على القرص.

**يتابع الاتصال المستخدَم فعلًا.** أثناء تشغيل VPN يحمل كرت النفق والكرت الفيزيائي
نفس البيانات — النفق يرى المحتوى المفتوح والكرت يرى المشفَّر — فجمعهما يجعل تحميل
1 جيجا يظهر 2 جيجا. الوضع التلقائي يتابع الكرت الواحد الحامل لمسار الإنترنت
الافتراضي، فيتابع الـVPN وهو حامل الإنترنت ويتركه حين لا يكون كذلك. وتتصفّر
الإجماليات عند تغيّر المصدر (إيثرنت ← VPN ← واي فاي) لتبقى الأرقام دائمًا عن
الاتصال الحالي. ويمكنك أيضًا تثبيت إيثرنت أو واي فاي أو كل الكروت أو كرت محدد.

**مظهر قابل للضبط.** خمسة أشكال للأسهم، وأربعة أوزان للخط، وأحجام للخط والسهم
والودجت واللوحة، وخط فاصل بين عمودَي SPEED و TOTAL **يمكن سحبه بالماوس** — الخط
وحده هو الذي يتحرك والكتابة تبقى في مكانها.

**لوحة تفاصيل** عند الضغط: الكرت والحالة وعنوان IPv4 والسرعات والإجماليات وزر
تصفير. وتلميح غنيّ عند المرور بالماوس، وقائمة عند كليك يمين.

**بايت أم بِت بلا لبس.** `MB/s` تعني ميجابايت في الثانية و`Mbps` تعني ميجابِت في
الثانية، والوحدة مكتوبة دائمًا بجانب الرقم.

**خفيف.** نداء واحد لكل دورة على خيط منفصل، وإعادة رسم فقط عند تغيّر الأرقام.
بالقياس: 0.3–0.5% من نواة واحدة، و27–31 ميجابايت، وبلا تسريب مقابض.

## المتطلبات

ويندوز 11 للحصول على الحواف الدائرية والأكريليك، ويعمل على ويندوز 10 بحواف قائمة.
لو تعارض زر Widgets مع المؤشر، أوقِفه من إعدادات شريط المهام ← Widgets.

## ملاحظات

* يعمل الودجت في عملية `explorer.exe` مساعدة مخصّصة (نمط "المودات كأدوات" في
  Windhawk)، فلا يستطيع أي خطأ فيه إسقاط الشل الحقيقي. ولهذا أثران ظاهران: يظهر
  سطر ثانٍ باسم "Windows Explorer" في مدير المهام، وأي مود آخر يستهدف
  `explorer.exe` يُحمَّل في العملية المساعدة أيضًا.
* يرتبط بشريط المهام الأساسي (`Shell_TrayWnd`).
* السرعات تُقاس بالتقطيع الزمني، فقراءة واحدة قد تختلف عن مدير المهام بنسبة قليلة،
  لكن المتوسط خلال ثانية مطابق (المقيس: فرق 0.06% خلال 22 ثانية).
* واجهة الإعدادات مترجمة بالكامل: يعرض Windhawk العربية تلقائيًّا حين تكون لغة
  واجهة ويندوز عربية.
* الكود والاختبارات والشرح الكامل:
  [github.com/cracken7/TaskbarNetworkLounge](https://github.com/cracken7/TaskbarNetworkLounge)
* مراجع التصميم: طريقة إنشاء النافذة ووضعها في نطاق الـz والمظهر الزجاجي تتبع ما
  يفعله مودَا **Taskbar Music Lounge** و**Taskbar Network Speed Indicator**
  (`net-speed-taskbar` لـNarayanChetri)، وقد قرأتُهما أثناء بناء هذا المود، ولم
  يُنسَخ أي كود منهما.
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
    $description: 13 is the sharpest default (measured); 11-12 for a smaller widget.
    $name:ar: حجم الخط
    $description:ar: 13 هو الأوضح (بالقياس الفعلي)؛ 11-12 لودجت أصغر.
  - TextWeight: bold
    $name: Text weight
    $options:
    - bold: Bold (sharpest, recommended)
    - black: Black (heaviest - Segoe UI Black)
    - semibold: Semibold
    - regular: Regular
    $name:ar: وزن الخط
    $options:ar:
    - bold: عريض (الأوضح - مُستحسن)
    - black: أسود (الأثقل - Segoe UI Black)
    - semibold: نصف عريض
    - regular: عادي
  - ArrowStyle: rounded
    $name: Arrow style
    $options:
    - solid: Solid arrow (stem + head)
    - rounded: Rounded arrow (soft, Fluent-like)
    - chevron: Chevron (thin, modern)
    - triangle: Triangle (compact, no stem)
    - circle: Arrow in a circle (badge)
    $name:ar: شكل السهم
    $options:ar:
    - solid: سهم صلب (ساق + رأس)
    - rounded: سهم دائري الأطراف (ناعم، شبيه بـFluent)
    - chevron: شيفرون (رفيع وعصري)
    - triangle: مثلث (مضغوط، بدون ساق)
    - circle: سهم داخل دائرة (شعار)
  - ArrowScale: 120
    $name: Arrow size (% of font size)
    $description: 120 = arrow is 1.2x the font height. Capped so it cannot overflow the widget - raise Panel height for bigger arrows. Range 60-400.
    $name:ar: حجم السهم (% من حجم الخط)
    $description:ar: 120 = السهم 1.2 ضعف ارتفاع الخط. مقيّد تلقائيًا حتى لا يخرج من الودجت - زوّد ارتفاع الودجت لأسهم أكبر. المدى 60-400.
  - ShowColumnLabels: true
    $name: Show SPEED / TOTAL captions
    $description: Small captions above the columns so it is obvious which is which.
    $name:ar: إظهار عنواني SPEED / TOTAL
    $description:ar: عنوانان صغيران فوق العمودين حتى يتضح أي عمود هو السرعة وأيهما الإجمالي.
  - DividerPos: 50
    $name: Divider position (% of width)
    $description: Where the line between SPEED and TOTAL sits, and how the width is split between them. Range 20-80.
    $name:ar: موضع الخط الفاصل (% من العرض)
    $description:ar: مكان الخط بين SPEED و TOTAL، وكيف يُقسَّم العرض بينهما. المدى 20-80.
  - DividerOpacity: 46
    $name: Divider opacity (0-255)
    $description: 0 hides the line completely.
    $name:ar: شفافية الخط الفاصل (0-255)
    $description:ar: القيمة 0 تخفي الخط تمامًا.
  - DividerDraggable: true
    $name: Drag the divider with the mouse
    $description: Grab the line and drag it left/right - only the line moves, the text stays put. Right click > Reset divider position to centre it again.
    $name:ar: تحريك الخط الفاصل بالماوس
    $description:ar: اسحب الخط يمين/شمال - الخط وحده هو الذي يتحرك، والكتابة تبقى في مكانها. للرجوع للمنتصف اعمل كليك يمين ثم Reset divider position.
  - DetailsWidth: 240
    $name: Details panel width
    $description: Size of the pop-up panel opened by clicking the widget.
    $name:ar: عرض لوحة التفاصيل
    $description:ar: حجم اللوحة التي تفتح عند الضغط على الودجت.
  - DetailsHeight: 276
    $name: Details panel height
    $description: A minimum, not a fixed size - the panel grows if its content needs more room.
    $name:ar: ارتفاع لوحة التفاصيل
    $description:ar: هذا حدّ أدنى لا مقاس ثابت - اللوحة تكبر لو احتاج محتواها مساحة أكبر.
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
  - OffsetX: 12
    $name: X offset
    $description: Distance from the left edge of the taskbar (from the top edge for a vertical taskbar).
    $name:ar: الإزاحة الأفقية
    $description:ar: المسافة من الحد الأيسر لشريط المهام (من الحد الأعلى لو الشريط رأسي).
  - OffsetY: 0
    $name: Y offset
    $name:ar: الإزاحة الرأسية
  - DpiScaling: true
    $name: Scale with DPI
    $description: Multiply sizes by the monitor scaling (125%, 150%, 175%, 200%...).
    $name:ar: التحجيم مع DPI
    $description:ar: ضرب المقاسات في نسبة تحجيم الشاشة (125%، 150%، 175%، 200%...).
  - AutoTheme: true
    $name: Auto theme
    $description: Follow the Windows light/dark theme.
    $name:ar: الثيم التلقائي
    $description:ar: يتبع ثيم ويندوز الفاتح/الغامق.
  - TextColor: "0xFFFFFF"
    $name: Manual text color (hex)
    $description: Used only when Auto theme is off.
    $name:ar: لون النص اليدوي (hex)
    $description:ar: يُستخدم فقط عند إيقاف الثيم التلقائي.
  - ColorArrows: true
    $name: Colored arrows
    $description: Blue for download, green for upload. Off = monochrome.
    $name:ar: أسهم ملوّنة
    $description:ar: أزرق للتحميل وأخضر للرفع. إيقاف = لون واحد.
  - BgOpacity: 0
    $name: Acrylic tint opacity (0-255)
    $description: Keep 0 for pure glass.
    $name:ar: شفافية طبقة الأكريليك (0-255)
    $description:ar: اتركها 0 للزجاج الصافي.
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
    $description: Name or part of the name/description of the adapter, e.g. "Ethernet" or "Realtek".
    $name:ar: الكرت المحدد
    $description:ar: اسم الكرت أو جزء من اسمه/وصفه، مثل "Ethernet" أو "Realtek".
  - ExcludeVirtual: true
    $name: Ignore virtual adapters
    $description: Skip loopback, tunnels, VPN, VMware, Hyper-V, Docker, TAP and other non-physical adapters. In Auto mode a VPN still wins when it carries the internet.
    $name:ar: تجاهل الكروت الوهمية
    $description:ar: تخطّي loopback والأنفاق وVPN وVMware وHyper-V وDocker وTAP وغيرها من الكروت غير الفيزيائية. في الوضع التلقائي يفوز الـVPN رغم ذلك لو كان هو حامل الإنترنت.
  - ResetOnSourceChange: true
    $name: Reset counters when the internet source changes
    $description: On = switching Ethernet -> VPN -> Wi-Fi zeroes the traffic totals, so they always describe the connection currently in use. Off = keep accumulating across adapters.
    $name:ar: تصفير العدادات عند تغيّر مصدر الإنترنت
    $description:ar: تشغيل = الانتقال من إيثرنت إلى VPN أو واي فاي يصفّر إجمالي الترافيك، فتصبح الأرقام دائمًا عن الاتصال المستخدم حاليًا. إيقاف = التجميع المستمر عبر كل الكروت.
  - UpdateInterval: 1000
    $name: Update interval (ms)
    $description: 250-5000 ms. 1000 ms is a good balance.
    $name:ar: زمن التحديث (ملي ثانية)
    $description:ar: من 250 إلى 5000. القيمة 1000 توازن جيد.
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
    $description: On = 1 MB is 1048576 bytes (like Explorer). Off = 1 MB is 1000000 bytes. Bit units are always 1000-based.
    $name:ar: وحدات بايت بأساس 1024
    $description:ar: تشغيل = الميجابايت = 1048576 بايت (مثل مستكشف الملفات). إيقاف = 1000000 بايت. وحدات البِت دائمًا بأساس 1000.
  $name: Network
  $name:ar: الشبكة
- Traffic:
  - CounterMode: session
    $name: Traffic counter mode
    $options:
    - session: Session (reset when the mod starts)
    - persistent: Persistent (saved to disk)
    $name:ar: طريقة عدّ الترافيك
    $options:ar:
    - session: للجلسة (يتصفّر عند بدء المود)
    - persistent: دائم (يُحفظ على القرص)
  - ResetCounters: none
    $name: Reset traffic counters
    $description: Pick a value and save to zero the counters. It is applied once, then remembered; the details panel and the right click menu can reset too.
    $options:
    - none: Do not reset
    - download: Reset download
    - upload: Reset upload
    - both: Reset both
    $name:ar: تصفير عدادات الترافيك
    $description:ar: اختر قيمة واحفظ لتصفير العدادات. تُطبَّق مرة واحدة ثم تُحفظ؛ ويمكن التصفير أيضًا من لوحة التفاصيل أو قائمة كليك يمين.
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
    $description: Off = the widget stays hidden until this is turned back on.
    $name:ar: يبدأ مُفعّلًا
    $description:ar: إيقاف = الودجت يبقى مخفيًا حتى يُعاد تشغيل هذا الخيار.
  $name: Behavior
  $name:ar: السلوك
*/
// ==/WindhawkModSettings==
