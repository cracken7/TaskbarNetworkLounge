#!/usr/bin/env python3
"""Change one Windhawk setting for this mod and bump SettingsChangeTime so the
engine calls WhTool_ModSettingsChanged (live reload, no restart).

Usage: python setopt.py Network.InterfaceMode wifi
       python setopt.py Appearance.PanelWidth 260
       python setopt.py --show
"""
import sys
import time
import winreg

KEY = r"SOFTWARE\Windhawk\Engine\Mods\local@taskbar-network-lounge"
ACCESS = winreg.KEY_ALL_ACCESS | winreg.KEY_WOW64_64KEY


def show() -> None:
    with winreg.OpenKeyEx(winreg.HKEY_LOCAL_MACHINE, KEY + r"\Settings", 0,
                          ACCESS) as key:
        index = 0
        while True:
            try:
                name, value, kind = winreg.EnumValue(key, index)
            except OSError:
                break
            print(f"  {name} = {value!r}")
            index += 1


def main() -> None:
    if len(sys.argv) == 2 and sys.argv[1] == "--show":
        show()
        return
    if len(sys.argv) < 3:
        sys.exit("usage: setopt.py <Setting.Name> <value> [more pairs...]")

    pairs = list(zip(sys.argv[1::2], sys.argv[2::2]))
    with winreg.CreateKeyEx(winreg.HKEY_LOCAL_MACHINE, KEY + r"\Settings", 0,
                            ACCESS) as key:
        for name, raw in pairs:
            try:
                _, kind = winreg.QueryValueEx(key, name)
            except OSError:
                kind = winreg.REG_SZ if not raw.lstrip("-").isdigit() else winreg.REG_DWORD
            if kind == winreg.REG_DWORD:
                winreg.SetValueEx(key, name, 0, winreg.REG_DWORD,
                                  int(raw) & 0xFFFFFFFF)
            else:
                winreg.SetValueEx(key, name, 0, winreg.REG_SZ, raw)
            print(f"[*] {name} = {raw}")

    with winreg.CreateKeyEx(winreg.HKEY_LOCAL_MACHINE, KEY, 0, ACCESS) as key:
        winreg.SetValueEx(key, "SettingsChangeTime", 0, winreg.REG_DWORD,
                          int(time.time()) & 0x7FFFFFFF)
    print("[*] SettingsChangeTime bumped")


if __name__ == "__main__":
    main()
