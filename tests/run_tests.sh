#!/usr/bin/env bash
# Build and run the console harness for the network-monitoring layer.
# Usage: bash tests/run_tests.sh [seconds] [interfaceMode] [counterMode]
set -euo pipefail

to_native() { echo "$1" | sed -E 's|^/([a-zA-Z])/|\1:/|'; }

WH_ROOT="/c/Program Files/Windhawk"
CLANG="$WH_ROOT/Compiler/bin/clang++.exe"
RUNTIME_DIR="$WH_ROOT/Compiler/x86_64-w64-mingw32/bin"

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
OUT_DIR="$ROOT/build"
mkdir -p "$OUT_DIR"

"$CLANG" -std=c++23 -target x86_64-w64-mingw32 -O1 \
  -DUNICODE -D_UNICODE -DWINVER=0x0A00 -D_WIN32_WINNT=0x0A00 \
  -DNTDDI_VERSION=0x0A000008 -D__USE_MINGW_ANSI_STDIO=0 \
  -o "$(to_native "$OUT_DIR/test_netmon.exe")" \
  "$(to_native "$ROOT/tests/test_netmon.cpp")" \
  -lgdiplus -ldwmapi -lgdi32 -luser32 -lshell32 -lole32 -lcomctl32 \
  -liphlpapi -lws2_32 -lshcore

# The bundled clang links libc++ dynamically and the import names are the
# .whl-renamed copies Windhawk ships, so provide both names next to the exe.
cp -f "$RUNTIME_DIR/libc++.dll" "$OUT_DIR/libc++.dll"
cp -f "$RUNTIME_DIR/libunwind.dll" "$OUT_DIR/libunwind.dll"
cp -f "$RUNTIME_DIR/libc++.dll" "$OUT_DIR/libc++.whl"
cp -f "$RUNTIME_DIR/libunwind.dll" "$OUT_DIR/libunwind.whl"

cd "$OUT_DIR"
./test_netmon.exe "${1:-8}" "${2:-auto}" "${3:-session}"
