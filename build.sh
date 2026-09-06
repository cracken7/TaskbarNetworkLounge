#!/usr/bin/env bash
# Build Taskbar Network Lounge with Windhawk's bundled clang toolchain.
# Usage: bash build.sh [output.dll]
#
# Note: MSYS path translation is disabled in this environment, so every path
# handed to clang (a native binary) is converted to C:/... form.
set -euo pipefail

to_native() { echo "$1" | sed -E 's|^/([a-zA-Z])/|\1:/|'; }

WH_ROOT="/c/Program Files/Windhawk"
CLANG="$WH_ROOT/Compiler/bin/clang++.exe"
ENGINE_LIB=$(ls -d "$WH_ROOT"/Engine/*/64/windhawk.lib | tail -1)

SRC_DIR="$(cd "$(dirname "$0")" && pwd)"
OUT="${1:-$SRC_DIR/build/taskbar-network-lounge.dll}"
mkdir -p "$(dirname "$OUT")"

MOD_SRC="$SRC_DIR/taskbar-network-lounge.wh.cpp"

# Concatenate the source parts into the single self-contained mod file.
cat "$SRC_DIR"/src/p1_header.inc \
    "$SRC_DIR"/src/p2_core.inc \
    "$SRC_DIR"/src/p3_settings.inc \
    "$SRC_DIR"/src/p4_network.inc \
    "$SRC_DIR"/src/p5_render.inc \
    "$SRC_DIR"/src/p6_window.inc \
    "$SRC_DIR"/src/p7_lifecycle.inc > "$MOD_SRC"

echo "Generated $MOD_SRC ($(wc -l < "$MOD_SRC") lines)"

"$CLANG" \
  -std=c++23 -target x86_64-w64-mingw32 -shared -O2 \
  -DUNICODE -D_UNICODE -DWINVER=0x0A00 -D_WIN32_WINNT=0x0A00 \
  -D_WIN32_IE=0x0A00 -DNTDDI_VERSION=0x0A000008 -D__USE_MINGW_ANSI_STDIO=0 \
  -DWH_MOD '-DWH_MOD_ID=L"local@taskbar-network-lounge"' '-DWH_MOD_VERSION=L"1.0.0"' \
  -include windhawk_api.h \
  -Wno-pragma-pack -Wno-pragma-system-header-outside-header \
  -Wall -Wextra -Wno-unused-parameter -Wno-missing-field-initializers \
  -o "$(to_native "$OUT")" "$(to_native "$MOD_SRC")" \
  "$(to_native "$ENGINE_LIB")" \
  -lole32 -ldwmapi -lgdi32 -luser32 -lgdiplus -lshell32 \
  -lcomctl32 -liphlpapi -lws2_32

echo "Built $OUT"
