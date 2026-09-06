# Installing Taskbar Network Lounge

*[بالعربية بالأسفل](#تركيب-مؤشر-الشبكة-لشريط-المهام)*

## 1. Install Windhawk

Download and install Windhawk from [windhawk.net](https://windhawk.net). It is the
loader that injects this mod into `explorer.exe`; the mod cannot run without it.

## 2. Get the mod source

Either download
**[taskbar-network-lounge.wh.cpp](https://github.com/cracken7/TaskbarNetworkLounge/releases/latest/download/taskbar-network-lounge.wh.cpp)**
from the latest release, or take it
[straight from the repository](https://raw.githubusercontent.com/cracken7/TaskbarNetworkLounge/main/taskbar-network-lounge.wh.cpp)
(right click → Save as).

## 3. Add it to Windhawk

1. Open Windhawk.
2. Click **Create a new mod**.
3. Select everything in the editor (`Ctrl+A`) and paste the file over it.
4. Click **Compile mod** and wait for the compiler to finish.
5. The widget appears on the taskbar within a few seconds.

## 4. Change settings

Windhawk → **Mods** → *Taskbar Network Lounge* → **Settings**. The settings are
translated: Windhawk shows Arabic automatically when the Windows UI language is
Arabic.

## Uninstalling

Windhawk → Mods → *Taskbar Network Lounge* → **Remove**. To keep the mod but hide
the widget, turn off *Behavior → Start enabled*.

Persistent traffic totals live in Windhawk's per-mod storage, so removing the mod
removes them too. To zero them while keeping the mod, use *Reset traffic counters*
in the settings, the Reset button in the details panel, or the right-click menu.

## Troubleshooting

**Nothing shows up.** Windows Widgets can sit in the same spot — turn them off
(Taskbar Settings → Widgets). Another taskbar mod drawing at the same offset will
also cover it; change *Appearance → X offset*.

**"No Network".** No adapter matched the current Interface mode. Set it to *Auto*,
or turn *Ignore virtual adapters* off if your connection runs through a virtual
adapter that isn't holding the default route.

**Totals look wrong after connecting a VPN.** They reset on purpose when the
internet source changes, so the numbers describe the connection you're on. Turn
*Network → Reset counters when the internet source changes* off to accumulate
across adapters instead.

**Speeds differ from Task Manager.** Both sample; a single reading can differ by a
few percent. The average over a second matches (measured: 0.06 % over 22 s).

**The widget disappeared after an Explorer restart.** It re-attaches on
`TaskbarCreated`. If it doesn't, restart the Windhawk engine from its tray icon.

## Building from source

Requires Windhawk (for its bundled clang) and Python 3 with `pyyaml`.

```bash
git clone https://github.com/cracken7/TaskbarNetworkLounge
cd TaskbarNetworkLounge
bash build.sh              # concatenates src/p1..p7.inc, compiles a test DLL
bash tests/run_all.sh      # all eight offline suites
python install.py          # write into Windhawk and restart the engine
```

---

# تركيب مؤشر الشبكة لشريط المهام

## 1. ثبّت Windhawk

نزّل Windhawk من [windhawk.net](https://windhawk.net) وثبّته. هو البرنامج الذي
يُحمّل هذا المود داخل `explorer.exe`، والمود لا يعمل بدونه.

## 2. نزّل ملف المود

نزّل ملف
**[taskbar-network-lounge.wh.cpp](https://github.com/cracken7/TaskbarNetworkLounge/releases/latest/download/taskbar-network-lounge.wh.cpp)**
من آخر إصدار، أو خُذه
[من المستودع مباشرة](https://raw.githubusercontent.com/cracken7/TaskbarNetworkLounge/main/taskbar-network-lounge.wh.cpp)
(كليك يمين ← حفظ باسم).

## 3. أضِفه إلى Windhawk

1. افتح Windhawk.
2. اضغط **Create a new mod**.
3. حدّد كل ما في المحرّر (`Ctrl+A`) والصق الملف فوقه.
4. اضغط **Compile mod** وانتظر انتهاء الترجمة.
5. يظهر الودجت على شريط المهام في ثوانٍ.

## 4. تغيير الإعدادات

Windhawk ← **Mods** ← *Taskbar Network Lounge* ← **Settings**. والإعدادات مترجمة:
يعرض Windhawk العربية تلقائيًّا حين تكون لغة واجهة ويندوز عربية.

## إزالة المود

Windhawk ← Mods ← *Taskbar Network Lounge* ← **Remove**. ولو أردت الإبقاء على المود
مع إخفاء الودجت، أوقِف *Behavior ← Start enabled*.

الإجماليات الدائمة محفوظة في مخزن Windhawk الخاص بالمود، فإزالة المود تزيلها معه.
ولتصفيرها مع الإبقاء على المود استخدم *Reset traffic counters* في الإعدادات، أو زر
التصفير في لوحة التفاصيل، أو قائمة كليك يمين.

## حلّ المشاكل

**لا يظهر شيء.** زر Widgets في ويندوز قد يشغل نفس المكان — أوقِفه من إعدادات شريط
المهام ← Widgets. وأي مود آخر يرسم عند نفس الإزاحة سيغطّيه أيضًا، فغيّر
*Appearance ← X offset*.

**تظهر "No Network".** لا يوجد كرت مطابق لوضع اختيار الكرت الحالي. اجعله *Auto*، أو
أوقِف *Ignore virtual adapters* لو كان اتصالك يمرّ عبر كرت وهمي لا يحمل مسار
الإنترنت الافتراضي.

**الإجماليات تبدو غلط بعد تشغيل VPN.** هي تتصفّر بشكل مقصود عند تغيّر مصدر الإنترنت
لتصف الاتصال الذي تستخدمه فعلًا. أوقِف
*Network ← Reset counters when the internet source changes* لو أردت التجميع عبر كل
الكروت.

**السرعات مختلفة عن مدير المهام.** كلاهما يقيس بالتقطيع الزمني، فقراءة واحدة قد
تختلف بنسبة قليلة، لكن المتوسط خلال ثانية مطابق (المقيس: فرق 0.06% خلال 22 ثانية).

**الودجت اختفى بعد إعادة تشغيل Explorer.** يعيد الارتباط تلقائيًّا عند
`TaskbarCreated`، ولو لم يحدث فأعد تشغيل محرّك Windhawk من أيقونته في منطقة
الإشعارات.

## البناء من الكود

يحتاج Windhawk (لمترجم clang المرفق معه) وPython 3 مع `pyyaml`.

```bash
git clone https://github.com/cracken7/TaskbarNetworkLounge
cd TaskbarNetworkLounge
bash build.sh              # يدمج src/p1..p7.inc ويبني DLL للاختبار
bash tests/run_all.sh      # كل مجموعات الاختبار الثماني
python install.py          # الكتابة في Windhawk وإعادة تشغيل المحرّك
```
