# Installing Taskbar Network Lounge

*[بالعربية بالأسفل](#تركيب-مؤشر-الشبكة-لشريط-المهام)*

Total time: about two minutes. You do not need to know anything about programming —
Windhawk does the compiling for you when you press one button.

## 1. Install Windhawk

Download and install Windhawk from [windhawk.net](https://windhawk.net).

Windhawk is a small program that loads mods like this one into Windows. This mod
cannot run without it. You install Windhawk once, and it stays.

## 2. Get the mod file

Download
**[taskbar-network-lounge.wh.cpp](https://github.com/cracken7/TaskbarNetworkLounge/releases/latest/download/taskbar-network-lounge.wh.cpp)**
from the latest release.

(Alternative: take it
[straight from the repository](https://raw.githubusercontent.com/cracken7/TaskbarNetworkLounge/main/taskbar-network-lounge.wh.cpp)
with right click → Save as.)

It is a plain text file. Do not worry about the `.cpp` extension — you never open
it yourself.

## 3. Add it to Windhawk

1. Open Windhawk.
2. Click **Create a new mod**. A code editor opens with a template in it.
3. Click inside the editor, press `Ctrl+A` to select everything, then paste the
   file's contents over it (open the downloaded file in Notepad, `Ctrl+A`,
   `Ctrl+C`, then `Ctrl+V` here).
4. Click **Compile mod**, and wait a few seconds for it to finish.
5. The widget appears on your taskbar, near the left end.

That is the whole installation. It survives restarts; you do not repeat this.

## 4. Change the settings

Windhawk → **Mods** → *Taskbar Network Lounge* → **Settings**.

Change a value, then press **Save**. The widget updates immediately — no restart
needed. The settings are fully translated: Windhawk shows Arabic automatically
when your Windows display language is Arabic.

The defaults are the recommended values, so you can leave everything as it is.

## Removing it

Windhawk → Mods → *Taskbar Network Lounge* → **Remove**.

To keep the mod but hide the widget, turn off *Behaviour → Start enabled* instead.

Persistent traffic totals live in Windhawk's own storage for this mod, so removing
the mod removes them too. To zero them while keeping the mod, use *Reset traffic
counters* in the settings, the Reset button in the details panel, or the right
click menu.

## If something is wrong

**Nothing appeared on the taskbar.** The Windows Widgets button sits in the same
place, so it may be covering the meter. Turn Widgets off in Taskbar Settings, or
move the meter to the right by raising *Appearance → X offset*. If you have
another taskbar mod drawing in that spot, the same fix applies.

**It says "No Network".** No adapter matched what you asked for. Set *Network →
Interface mode* back to **Auto**. If your connection runs through a virtual
adapter that is not the one holding the default route, also turn *Ignore virtual
adapters* off.

**The totals reset when I connected a VPN.** That is intentional: the numbers are
meant to describe the connection you are using right now, and a total carried over
from a different connection would be meaningless. If you want one running total
across every connection instead, turn off *Network → Reset counters when the
internet source changes*.

**The speed does not match Task Manager exactly.** Both take snapshots, just at
slightly different instants, so a single reading can differ by a few percent.
Averaged over a second they agree — measured to within 0.06% over 22 seconds.

**The widget vanished after Explorer restarted.** It re-attaches itself when the
taskbar is recreated. If it does not come back, restart the Windhawk engine from
its tray icon.

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

<div dir="rtl">

# تركيب مؤشر الشبكة لشريط المهام

الوقت المطلوب: دقيقتان تقريبًا. ولا تحتاج أن تعرف أي شيء عن البرمجة — فـWindhawk هو
الذي يتولّى الترجمة عند ضغطك زرًّا واحدًا.

## 1. ثبّت Windhawk

نزّل Windhawk من [windhawk.net](https://windhawk.net) وثبّته.

Windhawk برنامج صغير يُحمّل المودات مثل هذا داخل ويندوز، وهذا المود لا يعمل بدونه.
تثبّته مرة واحدة ويبقى.

## 2. نزّل ملف المود

نزّل ملف
**[taskbar-network-lounge.wh.cpp](https://github.com/cracken7/TaskbarNetworkLounge/releases/latest/download/taskbar-network-lounge.wh.cpp)**
من آخر إصدار.

(بديل: خُذه [من المستودع مباشرة](https://raw.githubusercontent.com/cracken7/TaskbarNetworkLounge/main/taskbar-network-lounge.wh.cpp)
بكليك يمين ← حفظ باسم.)

هو ملف نصّي عادي. ولا تقلق من امتداد `.cpp` — فأنت لن تفتحه بنفسك أبدًا.

## 3. أضِفه إلى Windhawk

1. افتح Windhawk.
2. اضغط **Create a new mod**، فيُفتح محرّر كود فيه قالب جاهز.
3. اضغط داخل المحرّر، ثم `Ctrl+A` لتحديد كل ما فيه، ثم الصق محتوى الملف فوقه (افتح
   الملف الذي نزّلته بالـNotepad، ثم `Ctrl+A`، ثم `Ctrl+C`، ثم `Ctrl+V` هنا).
4. اضغط **Compile mod** وانتظر ثوانٍ حتى تنتهي.
5. سيظهر الودجت على شريط المهام قرب طرفه الأيسر.

هذا هو التركيب كله. ويصمد بعد إعادة تشغيل الجهاز، فلا تُعيده مرة أخرى.

## 4. تغيير الإعدادات

Windhawk ← **Mods** ← *Taskbar Network Lounge* ← **Settings**.

غيّر أي قيمة ثم اضغط **Save**، فيتحدّث الودجت فورًا بلا إعادة تشغيل. والإعدادات
مترجمة بالكامل: يعرض Windhawk العربية تلقائيًّا حين تكون لغة عرض ويندوز عربية.

والقيم الافتراضية هي المُستحسنة، فيمكنك ترك كل شيء كما هو.

## إزالة المود

Windhawk ← Mods ← *Taskbar Network Lounge* ← **Remove**.

ولو أردت الإبقاء على المود مع إخفاء الودجت فقط، أوقِف *السلوك ← يبدأ مُفعّلًا*.

الإجماليات الدائمة محفوظة في مخزن Windhawk الخاص بهذا المود، فإزالة المود تزيلها
معه. ولتصفيرها مع الإبقاء على المود، استخدم *تصفير عدادات الترافيك* في الإعدادات، أو
زر التصفير في لوحة التفاصيل، أو قائمة الزر الأيمن.

## لو حدث شيء غير متوقّع

**لم يظهر شيء على شريط المهام.** زر Widgets في ويندوز يقع في نفس المكان، فقد يكون
يغطّي المؤشر. أوقِف Widgets من إعدادات شريط المهام، أو حرّك المؤشر يمينًا بزيادة
*المظهر ← الإزاحة الأفقية*. وينطبق نفس الحل لو كان عندك مود آخر يرسم في نفس المكان.

**تظهر كلمة "No Network".** لا يوجد كرت مطابق لما طلبته. أرجِع *الشبكة ← اختيار كرت
الشبكة* إلى **تلقائي**. ولو كان اتصالك يمرّ عبر كرت وهمي ليس هو حامل المسار
الافتراضي، فأوقِف أيضًا *تجاهل الكروت الوهمية*.

**الإجماليات تصفّرت عند تشغيل VPN.** هذا مقصود: فالأرقام يُفترض أن تصف الاتصال الذي
تستخدمه الآن، وإجماليّ منقول من اتصال آخر لا معنى له. ولو أردت إجماليًّا واحدًا
متراكمًا عبر كل الاتصالات، فأوقِف *الشبكة ← تصفير العدادات عند تغيّر مصدر الإنترنت*.

**السرعة لا تطابق مدير المهام بالضبط.** كلاهما يأخذ لقطات، لكن في لحظات مختلفة
قليلًا، فقراءة واحدة قد تختلف بنسبة قليلة. أما المتوسط خلال ثانية فمتطابق — والمقيس
فرق لا يزيد عن 0.06% خلال 22 ثانية.

**الودجت اختفى بعد إعادة تشغيل Explorer.** يعيد الارتباط بنفسه عند إعادة إنشاء شريط
المهام. ولو لم يرجع، فأعد تشغيل محرّك Windhawk من أيقونته في منطقة الإشعارات.

## البناء من الكود

يحتاج Windhawk (لمترجم clang المرفق معه) وPython 3 مع `pyyaml`.

```bash
git clone https://github.com/cracken7/TaskbarNetworkLounge
cd TaskbarNetworkLounge
bash build.sh              # يدمج src/p1..p7.inc ويبني DLL للاختبار
bash tests/run_all.sh      # كل مجموعات الاختبار الثماني
python install.py          # الكتابة في Windhawk وإعادة تشغيل المحرّك
```

</div>
