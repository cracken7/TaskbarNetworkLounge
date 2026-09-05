// winfind.cpp - list top-level windows whose class or title matches a substring.
// Usage: winfind.exe <needle> [more needles...]
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>

static const wchar_t* g_needle;
static int g_found;

static BOOL CALLBACK EnumProc(HWND hwnd, LPARAM) {
    wchar_t cls[256] = {};
    wchar_t title[256] = {};
    GetClassNameW(hwnd, cls, 256);
    GetWindowTextW(hwnd, title, 256);

    wchar_t clsLower[256], titleLower[256], needleLower[256];
    wcscpy_s(clsLower, cls);
    wcscpy_s(titleLower, title);
    wcscpy_s(needleLower, g_needle);
    _wcslwr_s(clsLower, 256);
    _wcslwr_s(titleLower, 256);
    _wcslwr_s(needleLower, 256);

    if (wcsstr(clsLower, needleLower) || wcsstr(titleLower, needleLower)) {
        DWORD pid = 0;
        GetWindowThreadProcessId(hwnd, &pid);
        RECT r{};
        GetWindowRect(hwnd, &r);
        LONG exStyle = GetWindowLongW(hwnd, GWL_EXSTYLE);
        LONG style = GetWindowLongW(hwnd, GWL_STYLE);
        BYTE alpha = 0;
        DWORD flags = 0;
        COLORREF key = 0;
        GetLayeredWindowAttributes(hwnd, &key, &alpha, &flags);
        wprintf(L"  pid=%-6lu hwnd=%p class=[%ls] title='%ls' visible=%d rect=(%ld,%ld)-(%ld,%ld) size=%ldx%ld ex=0x%08lX style=0x%08lX alpha=%u\n",
                pid, hwnd, cls, title, IsWindowVisible(hwnd) ? 1 : 0, r.left,
                r.top, r.right, r.bottom, r.right - r.left, r.bottom - r.top,
                (unsigned long)exStyle, (unsigned long)style, alpha);
        g_found++;
    }
    return TRUE;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("usage: winfind <needle> [needle...]\n");
        return 1;
    }
    for (int i = 1; i < argc; i++) {
        wchar_t needle[256] = {};
        MultiByteToWideChar(CP_UTF8, 0, argv[i], -1, needle, 256);
        g_needle = needle;
        g_found = 0;
        wprintf(L"--- %ls\n", needle);
        EnumWindows(EnumProc, 0);
        if (!g_found) {
            wprintf(L"  (no match)\n");
        }
    }
    return 0;
}
