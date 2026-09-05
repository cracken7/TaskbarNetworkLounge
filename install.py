#!/usr/bin/env python3
"""Install/compile Taskbar Network Lounge into Windhawk without the GUI.

Reproduces exactly what the Windhawk VSCodium extension does when you press
"Compile mod" for a local mod:

  1. write the source to %PROGRAMDATA%\\Windhawk\\ModsSource\\<modId>.wh.cpp
  2. compile it with the bundled clang++ (stdin source, --export-all-symbols)
     into %PROGRAMDATA%\\Windhawk\\Engine\\Mods\\64\\<modId>_<ver>_<rand>.dll
  3. write HKLM\\SOFTWARE\\Windhawk\\Engine\\Mods\\<modId> config + Settings
     (defaults taken from the ==WindhawkModSettings== YAML block) and bump
     SettingsChangeTime so the engine reloads.

Requires an elevated shell (HKLM write). Usage:
    python install.py [--no-settings] [--disabled]
"""

import argparse
import os
import random
import re
import subprocess
import sys
import time
import winreg

import yaml

MOD_ID = "local@taskbar-network-lounge"
SOURCE = os.path.join(os.path.dirname(os.path.abspath(__file__)),
                      "taskbar-network-lounge.wh.cpp")

WH_PROGRAM = r"C:\Program Files\Windhawk"
WH_DATA = r"C:\ProgramData\Windhawk"
CLANG = os.path.join(WH_PROGRAM, "Compiler", "bin", "clang++.exe")
COMPILER_DIR = os.path.join(WH_PROGRAM, "Compiler")
MODS_SOURCE = os.path.join(WH_DATA, "ModsSource")
MODS_BIN = os.path.join(WH_DATA, "Engine", "Mods", "64")
REG_PATH = r"SOFTWARE\Windhawk\Engine\Mods"


def engine_lib() -> str:
    engine_root = os.path.join(WH_PROGRAM, "Engine")
    candidates = []
    for name in os.listdir(engine_root):
        lib = os.path.join(engine_root, name, "64", "windhawk.lib")
        if os.path.isfile(lib):
            candidates.append(lib)
    if not candidates:
        sys.exit("windhawk.lib not found under " + engine_root)
    return sorted(candidates)[-1]


def read_metadata(text: str) -> dict:
    block = re.search(
        r"^//[ \t]+==WindhawkMod==[ \t]*$([\s\S]+?)^//[ \t]+==/WindhawkMod==[ \t]*$",
        text, re.M)
    if not block:
        sys.exit("no ==WindhawkMod== metadata block")
    meta = {}
    for line in block.group(1).splitlines():
        m = re.match(r"^//[ \t]+@([a-zA-Z_]+)[ \t]+(.*)$", line.rstrip())
        if m:
            meta.setdefault(m.group(1), []).append(m.group(2).strip())
    return meta


def read_settings_yaml(text: str):
    block = re.search(
        r"^//[ \t]+==WindhawkModSettings==[ \t]*$\s*/\*\s*([\s\S]+?)\s*\*/\s*"
        r"^//[ \t]+==/WindhawkModSettings==[ \t]*$", text, re.M)
    if not block:
        return None
    parsed = yaml.safe_load(block.group(1))
    if not isinstance(parsed, list):
        sys.exit("settings block is not a YAML array")
    return parsed


def flatten_settings(items, prefix="", out=None):
    """Mirror of the extension's extractInitialSettingsForEngine()."""
    if out is None:
        out = {}
    for item in items:
        keys = [k for k in item if not k.startswith("$")]
        if len(keys) != 1:
            sys.exit(f"setting entry must have exactly one key: {item!r}")
        key = keys[0]
        name = (prefix + "." if prefix else "") + key
        value = item[key]
        if isinstance(value, bool):
            out[name] = 1 if value else 0
        elif isinstance(value, (int, float)):
            out[name] = int(value)
        elif isinstance(value, str):
            out[name] = value
        elif isinstance(value, list):
            if value and isinstance(value[0], dict):
                flatten_settings(value, name, out)
            else:
                for index, element in enumerate(value):
                    out[f"{name}[{index}]"] = element
        else:
            sys.exit(f"unsupported setting value for {name}: {value!r}")
    return out


def compile_mod(source_text: str, dll_path: str, mod_version: str,
                compiler_options: str) -> None:
    args = [
        CLANG,
        "-std=c++23", "-O2", "-shared",
        "-DUNICODE", "-D_UNICODE",
        "-DWINVER=0x0A00", "-D_WIN32_WINNT=0x0A00", "-D_WIN32_IE=0x0A00",
        "-DNTDDI_VERSION=0x0A000008",
        "-D__USE_MINGW_ANSI_STDIO=0",
        "-DWH_MOD",
        f'-DWH_MOD_ID=L"{MOD_ID}"',
        f'-DWH_MOD_VERSION=L"{mod_version}"',
        engine_lib(),
        "-x", "c++", "-",
        "-include", "windhawk_api.h",
        "-target", "x86_64-w64-mingw32",
        "-Wl,--export-all-symbols",
        "-o", dll_path,
    ]
    args += compiler_options.split()

    print("[*] compiling ->", dll_path)
    proc = subprocess.run(args, input=source_text.encode("utf-8"),
                          cwd=COMPILER_DIR, capture_output=True)
    if proc.stdout:
        print(proc.stdout.decode("utf-8", "replace"))
    if proc.stderr:
        print(proc.stderr.decode("utf-8", "replace"))
    if proc.returncode != 0:
        sys.exit(f"compilation failed with exit code {proc.returncode}")


def write_config(dll_name: str, metadata: dict, settings: dict | None,
                 disabled: bool) -> None:
    key_path = REG_PATH + "\\" + MOD_ID
    access = winreg.KEY_ALL_ACCESS | winreg.KEY_WOW64_64KEY
    with winreg.CreateKeyEx(winreg.HKEY_LOCAL_MACHINE, key_path, 0,
                            access) as key:
        winreg.SetValueEx(key, "LibraryFileName", 0, winreg.REG_SZ, dll_name)
        winreg.SetValueEx(key, "Disabled", 0, winreg.REG_DWORD,
                          1 if disabled else 0)
        winreg.SetValueEx(key, "LoggingEnabled", 0, winreg.REG_DWORD, 1)
        winreg.SetValueEx(key, "Include", 0, winreg.REG_SZ,
                          "|".join(metadata.get("include", [])))
        winreg.SetValueEx(key, "Exclude", 0, winreg.REG_SZ,
                          "|".join(metadata.get("exclude", [])))
        winreg.SetValueEx(key, "Architecture", 0, winreg.REG_SZ,
                          "|".join(metadata.get("architecture", [])))
        winreg.SetValueEx(key, "Version", 0, winreg.REG_SZ,
                          metadata.get("version", [""])[0])

    if settings is not None:
        settings_path = key_path + "\\Settings"
        with winreg.CreateKeyEx(winreg.HKEY_LOCAL_MACHINE, settings_path, 0,
                                access) as key:
            existing = []
            index = 0
            while True:
                try:
                    existing.append(winreg.EnumValue(key, index)[0])
                except OSError:
                    break
                index += 1
            for name in existing:
                if name not in settings:
                    winreg.DeleteValue(key, name)
            for name, value in settings.items():
                if isinstance(value, int):
                    winreg.SetValueEx(key, name, 0, winreg.REG_DWORD,
                                      value & 0xFFFFFFFF)
                else:
                    winreg.SetValueEx(key, name, 0, winreg.REG_SZ, str(value))

    # SettingsChangeTime must change for the engine to reload the mod.
    with winreg.CreateKeyEx(winreg.HKEY_LOCAL_MACHINE, key_path, 0,
                            access) as key:
        winreg.SetValueEx(key, "SettingsChangeTime", 0, winreg.REG_DWORD,
                          int(time.time()) & 0x7FFFFFFF)


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--no-settings", action="store_true",
                       help="keep the settings already stored in the registry")
    parser.add_argument("--disabled", action="store_true",
                       help="install the mod but leave it disabled")
    args = parser.parse_args()

    with open(SOURCE, "r", encoding="utf-8") as handle:
        source_text = handle.read()

    metadata = read_metadata(source_text)
    mod_id = metadata["id"][0]
    version = metadata.get("version", ["1.0"])[0]
    compiler_options = metadata.get("compilerOptions", [""])[0]

    if MOD_ID != "local@" + mod_id:
        sys.exit(f"mod id mismatch: {mod_id} vs {MOD_ID}")

    os.makedirs(MODS_SOURCE, exist_ok=True)
    os.makedirs(MODS_BIN, exist_ok=True)

    source_copy = os.path.join(MODS_SOURCE, MOD_ID + ".wh.cpp")
    with open(source_copy, "w", encoding="utf-8") as handle:
        handle.write(source_text)
    print("[*] source ->", source_copy)

    while True:
        dll_name = f"{MOD_ID}_{version}_{random.randint(100000, 999999)}.dll"
        dll_path = os.path.join(MODS_BIN, dll_name)
        if not os.path.exists(dll_path):
            break

    compile_mod(source_text, dll_path, version, compiler_options)

    settings = None
    if not args.no_settings:
        parsed = read_settings_yaml(source_text)
        settings = flatten_settings(parsed) if parsed else {}
        print("[*] settings:")
        for name, value in settings.items():
            print(f"      {name} = {value!r}")

    write_config(dll_name, {k: v for k, v in metadata.items()}, settings,
                 args.disabled)
    print("[*] registry updated for", MOD_ID)
    print("[+] installed:", dll_name)


if __name__ == "__main__":
    main()
