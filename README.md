<div align="center">

# Taskbar Network Lounge

**A network meter that lives on your Windows 11 taskbar.**
Watch your download and upload speed as it happens, and how much data you have used — in a small glass widget that looks like it came with Windows.

[![Windhawk](https://img.shields.io/badge/Windhawk-mod-0078D4?style=flat-square)](https://windhawk.net)
[![Release](https://img.shields.io/github/v/release/cracken7/TaskbarNetworkLounge?style=flat-square&color=success)](https://github.com/cracken7/TaskbarNetworkLounge/releases/latest)
[![License](https://img.shields.io/badge/license-MIT-blue?style=flat-square)](LICENSE)
[![C++](https://img.shields.io/badge/C%2B%2B-23-00599C?style=flat-square)](https://en.cppreference.com/w/cpp/23)

**English** · [العربية](README.ar.md)

![Widget](docs/widget.png)

</div>

---

## Install it in three steps

1. **Install [Windhawk](https://windhawk.net).** Windhawk is the program that runs
   mods like this one. Install it once and it handles everything else.
2. **Download the mod file:**
   **[taskbar-network-lounge.wh.cpp](https://github.com/cracken7/TaskbarNetworkLounge/releases/latest/download/taskbar-network-lounge.wh.cpp)**
3. **In Windhawk**, click *Create a new mod*, select everything in the editor,
   paste the file over it, then click *Compile mod*.

The meter appears on your taskbar a few seconds later. If anything goes wrong, or
you want the click-by-click version, read **[INSTALL.md](INSTALL.md)**.

---

## What you are looking at

The widget is one small rectangle split into two halves by a thin line.

| Half | Label | What it shows |
| --- | --- | --- |
| Left | **SPEED** | How fast you are downloading and uploading **right now** |
| Right | **TOTAL** | How much you have downloaded and uploaded **in total** |

In both halves the top row is download and the bottom row is upload:

- **Blue arrow pointing down** = download
- **Green arrow pointing up** = upload

Every number carries its unit next to it, so there is never any doubt about what
you are reading.

### Four ways to interact with it

**Hover the mouse** over the widget and a tooltip appears with the same numbers
plus the name of the network adapter being measured.

**Left click** it to open the details panel — a glass card with everything the mod
knows: which adapter it is watching, whether that adapter is connected, its IP
address, the current speeds, the running totals, and a button that resets the
totals to zero.

![Details panel](docs/panel.png)

**Middle click** (press the wheel) to cycle what the widget shows:
both halves → speed only → traffic only → both.

**Right click** it for a menu:

| Menu item | What it does |
| --- | --- |
| Refresh | Take a new reading immediately |
| Show both / Show speed only / Show traffic only | Choose which half the widget displays |
| Show SPEED caption / Show TOTAL caption | Tick the small labels above each half on or off |
| Show arrows | Hide the arrows completely and keep only the numbers |
| Reset download / upload / both | Zero the totals |
| Reset divider position | Put the dividing line back in the middle |
| Open Network Settings | The Windows network settings page |
| Windhawk Settings | This mod's settings |
| Hide widget | Hide the meter (bring it back from *Start enabled* in the settings) |

Everything you pick in the menu is remembered across restarts.

---

## Why this mod exists: the VPN problem

This is the one thing that makes it different from other network meters, so it is
worth two minutes.

**The problem.** When you turn on a VPN, Windows creates a second network adapter
— a virtual one. From that moment your traffic passes through **two** adapters
instead of one:

1. through the **VPN adapter**, which sees your data before it is encrypted, and
2. through your **real Ethernet or Wi-Fi adapter**, which carries the encrypted
   copy out to the internet.

It is the same data, counted at two different points. Most network meters simply
add up every adapter they can find, so a 1 GB download shows up as **2 GB**. That
is not a rounding error, it is double.

**The fix.** This mod asks Windows a more precise question: *which single adapter
are you actually using to reach the internet right now?* Then it measures that one
adapter, and only that one. In practice:

- Turn a VPN on → the meter keeps measuring your real Ethernet or Wi-Fi, the
  same adapter Task Manager and your speedtest show.
- Turn the VPN off → nothing changes, it was measuring that adapter all along.
- Nothing is ever counted twice, whatever combination you are running.

Why not follow the VPN adapter itself? Because some VPN clients (NekoBox among
them) inflate their own virtual adapter's counters — measured at 2.66× the real
payload in a controlled test — so a meter that follows the tunnel reports speeds
your line cannot deliver. The physical adapter's counters are what the ISP,
Task Manager and every speedtest agree on, so that is what the meter reads.

**And the totals stay meaningful.** 4 GB downloaded over your Ethernet says
nothing about the VPN you just connected to, so by default the totals restart from
zero whenever your internet source changes. If you would rather have one lifetime
total across every connection, turn off *Reset counters when the internet source
changes*.

A brief drop is not treated as a switch. Unplugging the cable for two seconds, or
a tunnel that has not finished connecting, will not wipe your totals.

**If you would rather choose yourself,** you can: measure only Ethernet, only
Wi-Fi, all adapters added together, or one specific adapter you name.

---

## What it can do

### Show speed, show traffic, or both

The widget can display both halves side by side, the live speeds alone, or the
traffic totals alone. Switch from the right click menu, from the *Show traffic or
speed* setting, or with one middle click. Your choice is remembered, and the
layout adapts: a single half fills the width instead of leaving dead space.

### The numbers are measured, not estimated

The mod asks Windows how many bytes your adapter has moved, waits, asks again, and
divides the difference by the exact amount of time that passed. That is the whole
method. It never reads the text output of `ipconfig`, `netstat` or PowerShell, and
it never guesses.

Checked against Windows' own performance counters over 22 seconds, it agreed to
within **0.06%**.

### Bytes and bits are never mixed up

These two look similar and mean very different things:

- **`MB/s`** = megabytes per second — the unit File Explorer and download managers use.
- **`Mbps`** = megabits per second — the unit internet providers advertise.

There are 8 bits in a byte, so the same connection reads 8× higher in `Mbps`.
Mixing them up is the most common way a network number gets misread, so the mod
always prints the unit right next to the value, and lets you pick which one you
prefer.

### You decide how it looks

| You can change | Options |
| --- | --- |
| **Arrow shape** | Twenty-two designs — solid, rounded, chevron, triangle, double and triple chevrons, outline, hairline, block, arrowhead-only, in a circle, ring or rounded square, fading, dashed, barbed, needle, tail-dot, fletched, two-tone, arrow-into-a-tray, arrow-with-a-plus-bar — or hide the arrows entirely |
| **Arrow size** | As a percentage of the text size; it never grows past what fits |
| **Font** | Any installed font (empty for Segoe UI), five weights (bold, black, semibold, regular, italic), any size |
| **Colours per direction** | Give download and upload their own colour — arrow and numbers together — or keep the default blue/green |
| **SPEED / TOTAL captions** | Show both, one of them, or neither — from the settings or the right click menu |
| **The dividing line** | **Drag it with the mouse** to give one side more room, dim it, or hide it completely |
| **Widget size** | Width and height, plus where it sits along the taskbar |
| **Details panel size** | Width and a minimum height — it grows on its own if the content needs more room |
| **Layout** | Speeds and totals side by side, speeds only in two rows, or everything on one line |

![Arrow styles](docs/arrow-styles.png)

About dragging the line: grab it and move it left or right, and **only the line
moves**. The numbers stay exactly where they were — nothing shifts or re-flows.
Your position is remembered, and *Reset divider position* in the right click menu
puts it back in the middle.

### It stays out of the way

It takes one reading per second (you can change that) on a background thread, and
it only redraws the widget when a number that is actually on screen has changed.
Measured on a normal desktop: **0.3–0.5% of one CPU core**, about **30 MB** of
memory, and no leaked handles after 1500 redraws.

### It looks like part of Windows

Real acrylic glass, rounded corners, and it follows your light/dark theme. There
is no console window, no browser engine, no .NET or Python runtime — it is a
single native C++ component.

Text sharpness was tuned by measuring pixels rather than by eyeballing
screenshots, which matters at this size: small light text on a blurred background
is exactly where anti-aliasing normally goes wrong.

### It speaks Arabic

Every setting name, every description and every dropdown option is translated.
Windhawk shows Arabic automatically when your Windows display language is Arabic —
there is nothing to switch on.

---

## Settings reference

Windhawk shows these under the mod's **Settings** tab. Nothing here needs to be
touched for the mod to work; the defaults are the recommended values.

<details>
<summary><b>Appearance</b> — sizes, arrows, text, colours, layout</summary>

| Setting | Default | What it does |
| --- | --- | --- |
| Panel width / height | 220 × 52 | Size of the widget itself, in pixels at 100% display scaling |
| Font size | 13 | Size of the numbers. 13 is the clearest; 11–12 for a smaller widget |
| Font family | *(empty)* | Any installed font, e.g. `Consolas`, `Arial`, `Bahnschrift`. Empty = Segoe UI; a wrong name falls back to Segoe UI automatically |
| Text weight | Bold | Bold, Black, Semibold, Regular or Italic |
| Arrow style | Rounded | Twenty-two shapes, or *No arrows* to hide them |
| Arrow size | 120% | Arrow height relative to the text. Never overflows — raise Panel height for genuinely bigger arrows |
| SPEED / TOTAL captions | Both | Show both labels, one of them, or neither |
| Divider position | 50% | Where the dividing line sits, which also sets how much room each half gets |
| Divider opacity | 46 | How visible the line is. 0 hides it |
| Drag the divider | On | Lets you move the line with the mouse |
| Details panel width | 240 | Width of the panel that opens on click |
| Details panel height | 276 | A *minimum*, not a fixed size — the panel grows if its contents need more room |
| Layout | Full | Speeds + totals, speeds only in two rows, or one single line |
| Show traffic or speed | Both | Which half the widget displays; the right click menu and middle click switch it live and the choice is remembered |
| Middle click switches speed / traffic | On | Press the wheel on the widget to cycle both → speed → traffic |
| X / Y offset | 12 / 0 | Where the widget sits along the taskbar. Raise X to move it right |
| Scale with DPI | On | Keeps the widget the same apparent size when display scaling is not 100% |
| Auto theme | On | Take the text colour from the Windows light/dark theme |
| Manual text colour | `0xFFFFFF` | Only used when Auto theme is off. Format `0xRRGGBB` |
| Download color | *(empty)* | A custom colour for the download half — arrow and numbers together. Empty keeps the default blue |
| Upload color | *(empty)* | A custom colour for the upload half — arrow and numbers together. Empty keeps the default green |
| Coloured arrows | On | Blue download, green upload. Off draws both in the text colour. The two colour settings above override it per direction |
| Acrylic tint opacity | 0 | Adds a tint behind the glass. Raise it if a busy wallpaper hurts readability |

</details>

<details>
<summary><b>Network</b> — which adapter is measured, and in what units</summary>

| Setting | Default | What it does |
| --- | --- | --- |
| Interface mode | Auto | **Auto** = the physical adapter actually carrying your internet (Ethernet/Wi-Fi, even under a VPN). Or force Ethernet, Wi-Fi, all adapters together, or a specific one |
| Specific interface | — | Only used in *Specific* mode. The adapter name or any part of it, e.g. `Realtek` |
| Ignore virtual adapters | On | Skips adapters that are not real hardware (VPN tunnels, VMware, Hyper-V, Docker, TAP, loopback). In Auto mode the physical carrier is always measured anyway |
| Reset counters when the internet source changes | On | Totals start from zero each time you switch connection, so they always describe the one you are using |
| Update interval | 1000 ms | How often the numbers refresh. Range 250–5000 |
| Speed unit | Auto | Bytes per second (`MB/s`) or bits per second (`Mbps`) |
| 1024-based byte units | On | On: 1 MB = 1048576 bytes, like File Explorer. Off: 1 MB = 1000000 bytes, like ISP marketing |

</details>

<details>
<summary><b>Traffic</b> — how the totals are counted</summary>

| Setting | Default | What it does |
| --- | --- | --- |
| Traffic counter mode | This session only | **Session**: totals start from zero every time the mod starts. **Keep counting**: they survive Explorer, Windhawk and Windows restarts |
| Reset traffic counters | None | Pick download, upload or both and press Save to zero them once. You can also use the button in the details panel or the right click menu |

Persistent totals are stored in Windhawk's own storage for this mod, so removing
the mod removes them too — no stray files left on your disk.

</details>

<details>
<summary><b>Behaviour</b> — hover, click, visibility</summary>

| Setting | Default | What it does |
| --- | --- | --- |
| Show tooltip on hover | On | |
| Show details panel on click | On | |
| Hide when fullscreen | Off | Gets out of the way of games and full-screen video |
| Start enabled | On | Turn off to hide the widget without uninstalling. This is also how you bring it back after *Hide widget* |

</details>

---

## Requirements

- **Windows 11** for the rounded corners and the glass effect. It runs on
  **Windows 10** too, with square corners.
- **[Windhawk](https://windhawk.net)** installed.
- If the Windows **Widgets** button sits in the same spot and covers the meter,
  either turn Widgets off in Taskbar Settings or move the meter with the
  **X offset** setting.

---

## Known limitations

Stated plainly, so nothing comes as a surprise:

- The widget runs in its own separate helper process. That is deliberate — a fault
  in the mod cannot take your desktop down with it. The visible side effects: Task
  Manager shows a second *Windows Explorer* entry, and other mods that target
  Explorer get loaded into that helper too.
- It attaches to your **main** taskbar. Secondary taskbars on other monitors are a
  fallback only.
- Vertical taskbars are handled in code but have only been verified offline.
- ARM64 has not been tested.
- *Hide widget* has no tray icon to bring it back; use *Start enabled* in the
  settings.
- A single reading can differ from Task Manager by a few percent, because the two
  take their snapshots at slightly different instants. Averaged over a second they
  agree.

---

## For developers

<details>
<summary><b>Source layout, build, and tests</b></summary>

### Layout

```
src/p1_header.inc      metadata, @include explorer.exe, settings YAML (EN + AR)
src/p2_core.inc        headers, ModSettings, atomics/globals, menu IDs
src/p3_settings.inc    settings read + clamping, unit formatting, persistence
src/p4_network.inc     GetIfTable2 sampling, adapter filtering and selection,
                       source-change detection and counter reset
src/p5_render.inc      GDI+ painting: 22 arrow styles, sharp text, widget + panel
src/p6_window.inc      taskbar geometry, divider hit test, tooltip, context menu
src/p7_lifecycle.inc   window proc (incl. divider drag), Wh_ModInit/AfterInit
```

`build.sh` concatenates these into the single `taskbar-network-lounge.wh.cpp` that
Windhawk requires. `install.py` writes it into Windhawk and restarts the engine.
`tools/arrow_showcase.cpp` renders `docs/arrow-styles.png` from the mod's real
arrow code, so the picture can never drift from what the mod draws.

### Commands

```bash
bash build.sh              # regenerate the .wh.cpp and compile a test DLL
bash tests/run_all.sh      # all eight offline suites
python install.py          # install into Windhawk and restart the engine
python tools/setopt.py --show
```

### Test suites

| Suite | What it proves |
| --- | --- |
| `test_netmon` | Formatters, persistence round trip and corruption handling, live sampling |
| `test_geometry` | Widget placement for every taskbar edge, multi-monitor |
| `test_render` | 1500 paint cycles leak no GDI/USER handles |
| `test_accuracy` | Measured bytes vs the Windows performance counters |
| `test_styles` | Every arrow style × text weight actually draws (Segoe UI Semibold/Black are separate font *families* — a wrong family silently draws nothing in GDI+) |
| `test_text` | Text sharpness per rendering hint, size and weight |
| `test_fit` | Worst-case string widths vs the available column width |
| `test_panel` | Details-panel layout measured from the rendered pixels |

### Measurements

Ethernet (Realtek PCIe GbE), 1920 × 1080 at 100%, Windows 11, Windhawk 1.7.3.

| | |
| --- | --- |
| Accuracy vs Windows counters | 1.0006 down / 1.0008 up over 22 s (0.06%) |
| CPU | 0.09–0.14 s per 30 s ≈ 0.3–0.5% of one core |
| RAM | 27–31 MB working set, ±60 KB drift |
| GDI / USER handles | +1 / +2 after 1500 paints |
| Text sharpness | 57.2% fully saturated glyph pixels, 0 colour fringing |

Text rendering was tuned by measurement, not by eye: ClearType on acrylic produced
112 fringed pixels, so the mod uses greyscale `AntiAliasGridFit` plus a 1 px
contrast shadow, and `GenericTypographic` string formatting — the default format
adds ~1/6 em of side padding, which was clipping `12.4 MB/s`.

</details>

---

## Credits

The window creation, z-band placement and acrylic treatment follow the approach
used by [Taskbar Music Lounge](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/taskbar-music-lounge.wh.cpp)
and [Taskbar Network Speed Indicator](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/net-speed-taskbar.wh.cpp)
by [NarayanChetri](https://github.com/NarayanChetri) — both were read as
references while building this. The latter's README is also where the
`explorer.exe`-rather-than-`windhawk.exe` requirement for `CreateWindowInBand` is
documented. No code was copied from either.

## License

MIT — see [LICENSE](LICENSE).
