#!/usr/bin/env python3
"""Install an arbitrary .wh.cpp as a local Windhawk mod (diagnostic A/B tool).

Usage: python install_any.py <path-to-mod.wh.cpp>
Reuses install.py's machinery but derives the mod id from the source file.
"""
import os
import random
import sys

sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
import install as base  # noqa: E402


def main() -> None:
    if len(sys.argv) < 2:
        sys.exit("usage: install_any.py <mod.wh.cpp>")
    path = sys.argv[1]
    with open(path, "r", encoding="utf-8") as handle:
        text = handle.read()

    metadata = base.read_metadata(text)
    mod_id = "local@" + metadata["id"][0]
    version = metadata.get("version", ["1.0"])[0]
    options = metadata.get("compilerOptions", [""])[0]

    base.MOD_ID = mod_id
    os.makedirs(base.MODS_SOURCE, exist_ok=True)
    os.makedirs(base.MODS_BIN, exist_ok=True)

    source_copy = os.path.join(base.MODS_SOURCE, mod_id + ".wh.cpp")
    with open(source_copy, "w", encoding="utf-8") as handle:
        handle.write(text)

    while True:
        dll_name = f"{mod_id}_{version}_{random.randint(100000, 999999)}.dll"
        dll_path = os.path.join(base.MODS_BIN, dll_name)
        if not os.path.exists(dll_path):
            break

    base.compile_mod(text, dll_path, version, options)

    parsed = base.read_settings_yaml(text)
    settings = base.flatten_settings(parsed) if parsed else {}
    base.write_config(dll_name, metadata, settings, False)
    print("[+] installed", mod_id, "->", dll_name)


if __name__ == "__main__":
    main()
