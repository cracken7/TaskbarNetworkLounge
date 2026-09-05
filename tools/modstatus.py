#!/usr/bin/env python3
"""Read Windhawk's per-process mod status files (they are opened exclusively by
the engine, so a plain open() fails with EACCES; CreateFileW with full sharing
works)."""
import ctypes
import ctypes.wintypes as wintypes
import glob
import os
import sys
import time

kernel32 = ctypes.WinDLL("kernel32", use_last_error=True)
kernel32.CreateFileW.restype = wintypes.HANDLE
INVALID = wintypes.HANDLE(-1).value


def read_shared(path: str) -> str:
    handle = kernel32.CreateFileW(path, 0x80000000, 0x7, None, 3, 0x80, None)
    if handle in (None, -1, INVALID):
        return f"OPEN FAILED {ctypes.get_last_error()}"
    buffer = ctypes.create_string_buffer(1024)
    read = wintypes.DWORD()
    ok = kernel32.ReadFile(wintypes.HANDLE(handle), buffer, 1024,
                           ctypes.byref(read), None)
    kernel32.CloseHandle(wintypes.HANDLE(handle))
    if not ok:
        return f"READ FAILED {ctypes.get_last_error()}"
    return buffer.raw[:read.value].decode("utf-16-le", "replace")


def main() -> None:
    patterns = sys.argv[1:] or ["network-lounge"]
    directory = r"C:\ProgramData\Windhawk\Engine\ModsWritable\mod-status"
    for pattern in patterns:
        files = glob.glob(os.path.join(directory, "*" + pattern + "*"))
        print(f"--- {pattern}: {len(files)} file(s)")
        for path in sorted(files, key=os.path.getmtime):
            stamp = time.strftime("%H:%M:%S", time.localtime(os.path.getmtime(path)))
            print(f"   {stamp}  {os.path.basename(path)[:78]}  =>  {read_shared(path)}")


if __name__ == "__main__":
    main()
