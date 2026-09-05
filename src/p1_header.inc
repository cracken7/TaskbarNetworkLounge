// ==WindhawkMod==
// @id              taskbar-network-lounge
// @name            Taskbar Network Lounge
// @description     A compact, native network monitor docked on the taskbar: live download/upload speed plus total traffic.
// @version         1.0.0
// @author          cracken7
// @github          https://github.com/cracken7
// @include         explorer.exe
// @compilerOptions -lole32 -ldwmapi -lgdi32 -luser32 -lshcore -lgdiplus -lshell32 -lcomctl32 -liphlpapi -lws2_32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Network Lounge

A tiny, native-looking network meter that lives on the Windows 11 taskbar, in the
same spot and with the same acrylic / rounded / theme-aware styling as
**Taskbar Music Lounge**.

```
  v  12.4 MB/s     v  4.82 GB
  ^  1.8 MB/s      ^  1.17 GB
```

## Features
* **Live speed** - download and upload, measured from real interface counters
  (`GetIfTable2`, IP Helper API) divided by real elapsed time
  (`QueryPerformanceCounter`). No `ipconfig`/`netstat`/PowerShell parsing.
* **Total traffic** - session totals, or persistent totals that survive Explorer /
  Windhawk / Windows restarts (saved to a small file in `%LOCALAPPDATA%`).
* **Smart adapter selection** - Auto (the adapter that actually carries your
  internet traffic), Ethernet, Wi-Fi, all active adapters, or one specific
  adapter. Loopback / tunnels / VMware / Hyper-V / Docker / VPN adapters are
  filtered out by default (configurable).
* **Native styling** - GDI+ anti-aliased drawing, DWM rounded corners, acrylic
  blur, Segoe UI, automatic light/dark theme.
* **Details panel** - click the widget for an acrylic panel with interface,
  status, IPv4, speeds, totals and a **Reset** button.
* **Rich tooltip** on hover, **context menu** on right click (refresh, reset,
  open Windows network settings, hide).
* **Bytes vs bits** - `MB/s` = Megabytes per second, `Mbps` = Megabits per
  second (1 byte = 8 bits). Pick either; the unit is always drawn next to the
  number so the two can never be confused.
* Cheap: one `GetIfTable2` call per interval on a worker thread, repaint only
  when the numbers actually change, no busy waiting.

## Requirements
* Windows 11 (rounded corners + acrylic). Works on Windows 10 with squared corners.
* Taskbar Widgets should be off, otherwise they overlap: Taskbar Settings -> Widgets -> Off.

## Notes / limitations
* The widget attaches to the **primary** taskbar (`Shell_TrayWnd`). Secondary
  taskbars are used only as a fallback when no primary taskbar exists.
* Totals count only the interfaces that are being monitored, from the moment the
  mod started (Session) or from the stored value (Persistent).
* Speeds are sampled, so they can differ from Task Manager by a few percent
  within one sampling window; the running average over a second matches.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- Appearance:
  - PanelWidth: 220
    $name: Panel width
    $description: Width of the widget in pixels (at 100% scaling).
  - PanelHeight: 52
    $name: Panel height
    $description: Height of the widget in pixels (at 100% scaling).
  - FontSize: 13
    $name: Font size
    $description: 13 is the sharpest default (measured); 11-12 for a smaller widget.
  - BoldText: true
    $name: Bold text
    $description: Bold is much sharper at small sizes. Off = regular weight.
  - TextWeight: bold
    $name: Text weight
    $options:
    - bold: Bold (sharpest, recommended)
    - black: Black (heaviest - Segoe UI Black)
    - semibold: Semibold
    - regular: Regular
  - ArrowStyle: rounded
    $name: Arrow style
    $options:
    - solid: Solid arrow (stem + head)
    - rounded: Rounded arrow (soft, Fluent-like)
    - chevron: Chevron (thin, modern)
    - triangle: Triangle (compact, no stem)
    - circle: Arrow in a circle (badge)
  - ArrowScale: 120
    $name: Arrow size (% of font size)
    $description: 120 = arrow is 1.2x the font height. Capped so it cannot overflow the widget - raise Panel height for bigger arrows. Range 60-400.
  - ShowColumnLabels: true
    $name: Show SPEED / TOTAL captions
    $description: Small captions above the columns so it is obvious which is which.
  - DividerPos: 50
    $name: Divider position (% of width)
    $description: Where the line between SPEED and TOTAL sits. 55 = speed gets a bit more room. Range 20-80.
  - DividerOpacity: 46
    $name: Divider opacity (0-255)
    $description: 0 hides the line completely.
  - DetailsWidth: 240
    $name: Details panel width
    $description: Size of the pop-up panel opened by clicking the widget.
  - DetailsHeight: 252
    $name: Details panel height
  - LayoutMode: full
    $name: Layout
    $options:
    - full: Speeds + totals (two columns)
    - speeds: Speeds only (two rows)
    - oneline: Speeds only (single line)
  - OffsetX: 12
    $name: X offset
    $description: Distance from the left edge of the taskbar (from the top edge for a vertical taskbar).
  - OffsetY: 0
    $name: Y offset
  - DpiScaling: true
    $name: Scale with DPI
    $description: Multiply sizes by the monitor scaling (125%, 150%, 175%, 200%...).
  - AutoTheme: true
    $name: Auto theme
    $description: Follow the Windows light/dark theme.
  - TextColor: 0xFFFFFF
    $name: Manual text color (hex)
    $description: Used only when Auto theme is off.
  - ColorArrows: true
    $name: Colored arrows
    $description: Blue for download, green for upload. Off = monochrome.
  - BgOpacity: 0
    $name: Acrylic tint opacity (0-255)
    $description: Keep 0 for pure glass.
  $name: Appearance
- Network:
  - InterfaceMode: auto
    $name: Interface mode
    $options:
    - auto: Auto (adapter used for the internet)
    - ethernet: Ethernet only
    - wifi: Wi-Fi only
    - all: All active interfaces
    - specific: Specific interface (below)
  - SelectedInterface: ""
    $name: Specific interface
    $description: Name or part of the name/description of the adapter, e.g. "Ethernet" or "Realtek".
  - ExcludeVirtual: true
    $name: Ignore virtual adapters
    $description: Skip loopback, tunnels, VPN, VMware, Hyper-V, Docker, TAP and other non-physical adapters.
  - UpdateInterval: 1000
    $name: Update interval (ms)
    $description: 250-5000 ms. 1000 ms is a good balance.
  - SpeedUnit: auto
    $name: Speed unit
    $options:
    - auto: Auto (bytes per second, e.g. 12.4 MB/s)
    - bytes: Bytes per second (MB/s)
    - bits: Bits per second (Mbps)
  - BinaryUnits: true
    $name: Use 1024-based byte units
    $description: On = 1 MB is 1048576 bytes (like Explorer). Off = 1 MB is 1000000 bytes. Bit units are always 1000-based.
  $name: Network
- Traffic:
  - CounterMode: session
    $name: Traffic counter mode
    $options:
    - session: Session (reset when the mod starts)
    - persistent: Persistent (saved to disk)
  - ResetCounters: none
    $name: Reset traffic counters
    $description: Pick a value and save to zero the counters. It is applied once, then remembered; the details panel and the right click menu can reset too.
    $options:
    - none: Do not reset
    - download: Reset download
    - upload: Reset upload
    - both: Reset both
  $name: Traffic
- Behavior:
  - ShowTooltip: true
    $name: Show tooltip on hover
  - ShowDetailsOnClick: true
    $name: Show details panel on click
  - HideFullscreen: false
    $name: Hide when fullscreen
  - StartEnabled: true
    $name: Start enabled
    $description: Off = the widget stays hidden until this is turned back on.
  $name: Behavior
*/
// ==/WindhawkModSettings==
