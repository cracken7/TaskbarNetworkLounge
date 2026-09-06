# Installing Taskbar Network Lounge

## 1. Install Windhawk

Download and install Windhawk from [windhawk.net](https://windhawk.net). It is
the loader that injects this mod into `explorer.exe`; the mod cannot run without
it.

## 2. Get the mod source

Either:

* download **[taskbar-network-lounge.wh.cpp](https://raw.githubusercontent.com/cracken7/TaskbarNetworkLounge/main/taskbar-network-lounge.wh.cpp)**
  (right click → Save as), or
* grab it from the [latest release](https://github.com/cracken7/TaskbarNetworkLounge/releases/latest).

## 3. Add it to Windhawk

1. Open Windhawk.
2. Click **Create a new mod**.
3. Select everything in the editor (`Ctrl+A`) and paste the file over it.
4. Click **Compile mod** and wait for the compiler to finish.
5. The widget appears on the taskbar within a few seconds.

If nothing appears, see Troubleshooting below.

## 4. Change settings

Windhawk → **Mods** → *Taskbar Network Lounge* → **Settings**. The settings are
translated: Windhawk shows Arabic automatically when the Windows UI language is
Arabic.

## Uninstalling

Windhawk → Mods → *Taskbar Network Lounge* → **Remove**. To keep the mod but hide
the widget, turn off *Behavior → Start enabled*.

Persistent traffic totals live in
`%LOCALAPPDATA%\TaskbarNetworkLounge\traffic.dat`; delete the folder to clear
them.

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

**Speeds differ from Task Manager.** Both sample; a single reading can differ by
a few percent. The average over a second matches (measured: 0.06 % over 22 s).

**The widget disappeared after an Explorer restart.** It re-attaches on
`TaskbarCreated`. If it doesn't, restart the Windhawk engine from its tray icon.

## Building from source

Requires Windhawk (for its bundled clang) and Python 3 with `pyyaml`.

```bash
git clone https://github.com/cracken7/TaskbarNetworkLounge
cd TaskbarNetworkLounge
bash build.sh              # concatenates src/p1..p7.inc, compiles a test DLL
bash tests/run_all.sh      # all seven offline suites
python install.py          # write into Windhawk and restart the engine
```
