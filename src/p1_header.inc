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

A small network meter that sits on your taskbar and shows how fast you are
downloading and uploading right now, plus how much data you have used in total.

![Widget](https://raw.githubusercontent.com/cracken7/TaskbarNetworkLounge/main/docs/widget.png)

## What you see

The widget is split into two halves by a thin line:

* **SPEED** (left) — your download and upload speed at this moment.
* **TOTAL** (right) — how much you have downloaded and uploaded in total.

A blue arrow pointing down always means download. A green arrow pointing up
always means upload.

**Hover** over it for a tooltip with the same numbers plus your adapter name.
**Left click** it to open a details panel: which adapter is being measured,
whether it is connected, its IP address, the speeds, the totals, and a button to
reset the totals. **Right click** it for a menu: refresh, reset the download or
upload total, open Windows network settings, open Windhawk, or hide the widget.

![Details panel](https://raw.githubusercontent.com/cracken7/TaskbarNetworkLounge/main/docs/panel.png)

## What makes it different

### It counts your VPN correctly

This is the main reason the mod exists.

When you turn on a VPN, Windows creates a second, virtual network adapter. Your
real traffic now passes through **both** of them: once through the VPN adapter,
and once through your real Ethernet or Wi-Fi adapter that actually carries it out
to the internet. Most network meters add up every adapter they can see, so a 1 GB
download gets counted twice and shows up as 2 GB.

This mod measures **one adapter only** — the one Windows is actually using to
reach the internet at that moment. So:

* Turn a VPN on, and it follows the VPN.
* Turn it off, and it goes back to your Ethernet or Wi-Fi.
* Nothing is ever counted twice.

And because the totals for one connection say nothing about a different one, the
totals restart from zero whenever your internet source changes. If you would
rather keep one running total across every connection, there is a setting to turn
that off.

You can also override the choice entirely: measure only Ethernet, only Wi-Fi, all
adapters at once, or one specific adapter you name yourself.

### The numbers are honest

Speed is measured by asking Windows how many bytes your adapter has moved, twice,
and dividing the difference by the exact time that passed between the two
readings. Nothing is estimated, and no text output from `ipconfig`, `netstat` or
PowerShell is parsed.

Units are never ambiguous, because the unit is always written next to the number:
`MB/s` means megabytes per second, `Mbps` means megabits per second. (There are 8
bits in a byte, so these differ by 8x — mixing them up is the most common way a
network reading gets misread.) You choose which one you want.

### You can shape how it looks

* **Arrows** — five styles: rounded, solid, chevron, triangle, or inside a
  circle. You set the size too, as a percentage of the text size.
* **Text** — four weights (bold, black, semibold, regular) and any size you want.
* **The dividing line** — drag it left or right with the mouse to give one side
  more room. Only the line moves; the numbers stay exactly where they are. You can
  also dim it, hide it completely, or reset it from the right click menu.
* **Sizes** — the widget and the details panel are both resizable, and you can
  move the widget along the taskbar.
* **Colours** — it follows your Windows light/dark theme by default, or you can
  set the text colour yourself and switch the coloured arrows off.
* **Layout** — speeds and totals side by side, speeds only in two rows, or
  everything on a single line.

### It stays out of the way

It asks Windows for the numbers once a second (you can change that) on a
background thread, and it only redraws when a number on screen actually changes.
Measured on a normal desktop: **0.3–0.5% of one CPU core** and about **30 MB** of
memory, with no leaks after 1500 redraws.

The look is native, not a copy of one: real acrylic glass, rounded corners, Segoe
UI, and it follows your light/dark theme. Text rendering was tuned by measurement
rather than by eye, so small numbers stay sharp on top of the blurred background.

### It speaks Arabic

Every setting name, description and dropdown option is translated. Windhawk shows
Arabic automatically when your Windows display language is Arabic.

## Requirements

Windows 11 for the rounded corners and glass effect. It works on Windows 10 too,
with square corners. If the Windows Widgets button sits in the same place and
covers it, turn Widgets off in Taskbar Settings, or move this widget with the
**X offset** setting.

## Good to know

* The widget runs in its own separate helper process, so if something ever goes
  wrong with it, your desktop and taskbar are unaffected. Because of that you will
  see a second "Windows Explorer" entry in Task Manager, and other mods that
  target Explorer are loaded into that helper as well.
* It attaches to your main taskbar.
* A single reading can differ from Task Manager by a few percent, because both are
  taking snapshots at slightly different moments. Measured over 22 seconds, the
  totals agreed with Windows' own counters to within 0.06%.
* Source code, tests and full documentation:
  [github.com/cracken7/TaskbarNetworkLounge](https://github.com/cracken7/TaskbarNetworkLounge)
* Design references: the way the window is created, placed above the taskbar and
  given its glass effect follows the approach used by **Taskbar Music Lounge** and
  **Taskbar Network Speed Indicator** (`net-speed-taskbar` by NarayanChetri), both
  of which were read while building this. No code was copied from either.

---

# مؤشر الشبكة لشريط المهام

مؤشر صغير يستقرّ على شريط المهام ويعرض سرعة التحميل والرفع في اللحظة الحالية،
وإجمالي ما استهلكته من بيانات.

## ما تراه

الودجت مقسوم إلى نصفين بخط رفيع:

* **SPEED** (يسار) — سرعة التحميل والرفع في هذه اللحظة.
* **TOTAL** (يمين) — إجمالي ما نزّلته وما رفعته.

السهم الأزرق المتّجه لأسفل يعني التحميل دائمًا، والسهم الأخضر المتّجه لأعلى يعني
الرفع دائمًا.

**مرِّر الماوس** فوقه ليظهر تلميح بنفس الأرقام مع اسم الكرت. **اضغط بالزر الأيسر**
لتفتح لوحة التفاصيل: أي كرت يُقاس، وهل هو متصل، وعنوانه، والسرعات، والإجماليات، وزر
لتصفير الإجماليات. **اضغط بالزر الأيمن** لتظهر قائمة: تحديث، تصفير إجمالي التحميل
أو الرفع، فتح إعدادات شبكة ويندوز، فتح Windhawk، أو إخفاء الودجت.

![لوحة التفاصيل](https://raw.githubusercontent.com/cracken7/TaskbarNetworkLounge/main/docs/panel.png)

## ما يميّزه

### يحسب الـVPN بشكل صحيح

هذا هو السبب الأساسي لوجود هذا المود.

عند تشغيل VPN ينشئ ويندوز كرت شبكة ثانيًا وهميًّا. وبياناتك الحقيقية تمرّ عبر
**الاثنين**: مرة عبر كرت الـVPN، ومرة عبر كرت الإيثرنت أو الواي فاي الحقيقي الذي
يحملها فعلًا إلى الإنترنت. ومعظم مؤشرات الشبكة تجمع كل كرت تراه، فيُحسَب تحميل
حجمه 1 جيجا مرتين ويظهر 2 جيجا.

هذا المود يقيس **كرتًا واحدًا فقط** — الكرت الذي يستخدمه ويندوز فعلًا للوصول إلى
الإنترنت في تلك اللحظة. أي:

* شغّل VPN فيتابعه.
* أوقِفه فيرجع إلى الإيثرنت أو الواي فاي.
* ولا يُحسَب أي شيء مرتين.

ولأن إجماليات اتصال معيّن لا تقول شيئًا عن اتصال آخر، تبدأ الإجماليات من الصفر عند
تغيّر مصدر الإنترنت. ولو كنت تفضّل إجماليًّا واحدًا متراكمًا عبر كل الاتصالات، فهناك
إعداد لإيقاف هذا السلوك.

ويمكنك أيضًا تجاوز الاختيار التلقائي بالكامل: قِس الإيثرنت وحده، أو الواي فاي وحده،
أو كل الكروت مجتمعة، أو كرتًا محددًا تكتب اسمه بنفسك.

### الأرقام صادقة

تُقاس السرعة بسؤال ويندوز عن عدد البايتات التي نقلها الكرت، مرتين، ثم قسمة الفرق
على الزمن الذي مضى بين القراءتين بدقّة. لا يوجد أي تقدير، ولا قراءة لمخرجات
`ipconfig` أو `netstat` أو PowerShell.

والوحدات لا تلتبس أبدًا لأن الوحدة مكتوبة دائمًا بجانب الرقم: `MB/s` تعني ميجابايت
في الثانية، و`Mbps` تعني ميجابِت في الثانية. (في البايت 8 بِتات، فالفرق بينهما 8
أضعاف — والخلط بينهما هو أشهر سبب لقراءة أرقام الشبكة قراءة خاطئة.) والاختيار لك.

### تتحكّم في شكله

* **الأسهم** — خمسة أشكال: دائري الأطراف، أو صلب، أو شيفرون، أو مثلث، أو داخل
  دائرة. وتحدّد حجمها أيضًا كنسبة من حجم الخط.
* **الخط** — أربعة أوزان (عريض، أسود، نصف عريض، عادي) وأي حجم تريده.
* **الخط الفاصل** — اسحبه يمينًا أو شمالًا بالماوس لتوسّع أحد الجانبين. الخط وحده
  هو الذي يتحرك، والأرقام تبقى في مكانها تمامًا. ويمكنك تخفيته أو إخفاؤه كليًّا أو
  إرجاعه للمنتصف من قائمة الزر الأيمن.
* **المقاسات** — الودجت ولوحة التفاصيل كلاهما قابل لتغيير الحجم، ويمكنك تحريك
  الودجت على طول شريط المهام.
* **الألوان** — يتبع ثيم ويندوز الفاتح/الغامق افتراضيًّا، أو حدّد لون النص بنفسك
  وأوقِف تلوين الأسهم.
* **التخطيط** — السرعات والإجماليات جنبًا إلى جنب، أو السرعات وحدها في سطرين، أو
  كل شيء في سطر واحد.

### لا يثقل على جهازك

يسأل ويندوز عن الأرقام مرة كل ثانية (والمدة قابلة للتغيير) على خيط في الخلفية، ولا
يعيد الرسم إلا عند تغيّر رقم ظاهر على الشاشة فعلًا. وبالقياس على جهاز عادي:
**0.3–0.5% من نواة معالج واحدة** وحوالي **30 ميجابايت** من الذاكرة، وبلا أي تسريب
بعد 1500 عملية رسم.

والمظهر أصلي لا مجرد محاكاة: زجاج أكريليك حقيقي، وحواف دائرية، وخط Segoe UI،
ويتبع الثيم الفاتح والغامق. وقد ضُبط رسم النص بالقياس لا بالنظر، فتبقى الأرقام
الصغيرة حادّة فوق الخلفية الضبابية.

### يتكلّم العربية

كل اسم إعداد ووصفه وكل خيار في القوائم مترجم. ويعرض Windhawk العربية تلقائيًّا حين
تكون لغة عرض ويندوز عربية.

## المتطلبات

ويندوز 11 للحصول على الحواف الدائرية والتأثير الزجاجي، ويعمل على ويندوز 10 أيضًا
بحواف قائمة. ولو كان زر Widgets في ويندوز يشغل نفس المكان ويغطّيه، فأوقِف Widgets
من إعدادات شريط المهام، أو حرّك هذا الودجت بإعداد **الإزاحة الأفقية**.

## معلومات مفيدة

* يعمل الودجت في عملية مساعدة منفصلة خاصة به، فلو حدث أي خطأ فيه لن يتأثر سطح
  المكتب ولا شريط المهام. ولهذا سترى سطرًا ثانيًا باسم "Windows Explorer" في مدير
  المهام، وأي مود آخر يستهدف Explorer سيُحمَّل في تلك العملية المساعدة أيضًا.
* يرتبط بشريط المهام الرئيسي.
* قد تختلف قراءة واحدة عن مدير المهام بنسبة قليلة لأن كلًّا منهما يأخذ لقطاته في
  لحظات مختلفة قليلًا. وبالقياس على مدى 22 ثانية، طابقت الإجماليات عدّادات ويندوز
  نفسها بفرق لا يزيد عن 0.06%.
* الكود والاختبارات والشرح الكامل:
  [github.com/cracken7/TaskbarNetworkLounge](https://github.com/cracken7/TaskbarNetworkLounge)
* مراجع التصميم: طريقة إنشاء النافذة ووضعها فوق شريط المهام وإعطاؤها التأثير
  الزجاجي تتبع ما يفعله مودَا **Taskbar Music Lounge** و**Taskbar Network Speed
  Indicator** (`net-speed-taskbar` لـNarayanChetri)، وقد قرأتُ الاثنين أثناء بناء
  هذا المود، ولم يُنسَخ أي كود من أيٍّ منهما.
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
    $description: Arrow size relative to the text. 120 means the arrow is 1.2x as tall as the numbers. It never grows past what fits, so for really big arrows increase Panel height too. Range 60-400.
    $name:ar: حجم السهم (% من حجم الخط)
    $description:ar: حجم السهم بالنسبة للنص. القيمة 120 تعني أن السهم أطول من الأرقام بمقدار 1.2 ضعف. ولن يكبر أبدًا أكثر من المساحة المتاحة، فلو أردت أسهمًا كبيرة فعلًا زوّد ارتفاع الودجت أيضًا. المدى 60-400.
  - ShowColumnLabels: true
    $name: Show SPEED / TOTAL captions
    $description: Small labels above the two halves, so it is clear which side is your current speed and which is your total usage.
    $name:ar: إظهار عنواني SPEED / TOTAL
    $description:ar: عنوانان صغيران فوق النصفين، ليتضح أي جانب هو سرعتك الحالية وأيهما إجمالي استهلاكك.
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
  - ColorArrows: true
    $name: Colored arrows
    $description: Blue arrow for download, green for upload. Turn this off to draw both in the text colour.
    $name:ar: أسهم ملوّنة
    $description:ar: سهم أزرق للتحميل وأخضر للرفع. أوقِفه لرسم الاثنين بلون النص.
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
    $description: Ignores adapters that are not real hardware - VPN tunnels, VMware, Hyper-V, Docker, TAP, loopback. Note that in Auto mode a VPN is still measured while it is the one carrying your internet; this setting only stops idle virtual adapters from being picked.
    $name:ar: تجاهل الكروت الوهمية
    $description:ar: يتجاهل الكروت التي ليست عتادًا حقيقيًّا - أنفاق VPN وVMware وHyper-V وDocker وTAP وloopback. ولاحظ أنه في الوضع التلقائي يُقاس الـVPN رغم ذلك طالما كان هو حامل الإنترنت، وهذا الإعداد يمنع فقط اختيار الكروت الوهمية غير المستخدمة.
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
