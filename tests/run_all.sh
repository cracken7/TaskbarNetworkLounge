#!/usr/bin/env bash
# Build and run every offline test for Taskbar Network Lounge.
#   tests/run_all.sh [live-sampling-seconds]
#
# Covers: formatter unit tests, persistence round trip + corruption handling,
# geometry (taskbar edges / DPI / multi-monitor), render + GDI leak check, and a
# short live sampling run against the real adapters.
set -euo pipefail

to_native() { echo "$1" | sed -E 's|^/([a-zA-Z])/|\1:/|'; }

WH_ROOT="/c/Program Files/Windhawk"
CLANG="$WH_ROOT/Compiler/bin/clang++.exe"
RUNTIME_DIR="$WH_ROOT/Compiler/x86_64-w64-mingw32/bin"

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
OUT="$ROOT/build"
SECS="${1:-6}"
mkdir -p "$OUT"

COMMON_FLAGS=(-std=c++23 -target x86_64-w64-mingw32 -O1
  -DUNICODE -D_UNICODE -DWINVER=0x0A00 -D_WIN32_WINNT=0x0A00
  -DNTDDI_VERSION=0x0A000008 -D__USE_MINGW_ANSI_STDIO=0
  -Wall -Wextra -Wno-unused-parameter -Wno-missing-field-initializers)
LIBS=(-lgdiplus -ldwmapi -lgdi32 -luser32 -lshell32 -lole32 -lcomctl32
  -liphlpapi -lws2_32 -lshcore)

build_test() {
  local name="$1"
  echo "== compiling $name"
  "$CLANG" "${COMMON_FLAGS[@]}" \
    -o "$(to_native "$OUT/$name.exe")" \
    "$(to_native "$ROOT/tests/$name.cpp")" "${LIBS[@]}"
}

build_test test_netmon
build_test test_geometry
build_test test_render
build_test test_accuracy
build_test test_styles
build_test test_text
build_test test_fit

# clang links libc++ dynamically; provide both the plain and Windhawk-renamed
# import names next to the executables.
for lib in libc++ libunwind; do
  cp -f "$RUNTIME_DIR/$lib.dll" "$OUT/$lib.dll"
  cp -f "$RUNTIME_DIR/$lib.dll" "$OUT/$lib.whl"
done

cd "$OUT"
fail=0

echo
echo "################ geometry ################"
./test_geometry.exe || fail=1

echo
echo "################ render / GDI leak ################"
./test_render.exe 600 || fail=1

echo
echo "################ arrow styles x text weights ################"
./test_styles.exe || fail=1

echo
echo "################ text sharpness measurements ################"
./test_text.exe || fail=1

echo
echo "################ width fit (clipping) ################"
./test_fit.exe || fail=1

echo
echo "################ formatters, persistence, live sampling ################"
./test_netmon.exe "$SECS" auto session || fail=1

echo
if [ "$fail" -eq 0 ]; then
  echo "ALL TEST SUITES PASSED"
else
  echo "SOME TEST SUITES FAILED"
fi
exit "$fail"
