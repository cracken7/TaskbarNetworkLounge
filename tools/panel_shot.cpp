// Open the details panel (left click on the widget) and screenshot it.
#include <windows.h>
#include <gdiplus.h>
#include <cstdio>
#include <vector>
using namespace Gdiplus;

static HWND findPanel() {
    return FindWindowW(L"WindhawkNetworkLoungePanel", nullptr);
}

static bool SavePng(HDC src, RECT& r, PCWSTR path) {
    int w = r.right - r.left, h = r.bottom - r.top;
    HDC mem = CreateCompatibleDC(src);
    HBITMAP bmp = CreateCompatibleBitmap(src, w, h);
    HBITMAP old = (HBITMAP)SelectObject(mem, bmp);
    BitBlt(mem, 0, 0, w, h, src, r.left, r.top, SRCCOPY | CAPTUREBLT);
    SelectObject(mem, old);
    DeleteDC(mem);
    Bitmap out(bmp, nullptr);
    DeleteObject(bmp);
    CLSID pngClsid{};
    UINT num = 0, size = 0;
    GetImageEncodersSize(&num, &size);
    std::vector<BYTE> buf(size);
    ImageCodecInfo* infos = (ImageCodecInfo*)buf.data();
    GetImageEncoders(num, size, infos);
    for (UINT i = 0; i < num; i++) {
        if (wcscmp(infos[i].MimeType, L"image/png") == 0) {
            pngClsid = infos[i].Clsid;
            break;
        }
    }
    return out.Save(path, &pngClsid, nullptr) == Ok;
}

int wmain() {
    SetProcessDPIAware();
    ULONG_PTR token = 0;
    GdiplusStartupInput si;
    GdiplusStartup(&token, &si, nullptr);

    HWND widget = FindWindowW(L"WindhawkNetworkLoungeWidget", nullptr);
    if (!widget) {
        fwprintf(stderr, L"widget not found\n");
        return 1;
    }
    // Open the panel with a real click sequence.
    PostMessageW(widget, WM_LBUTTONDOWN, MK_LBUTTON, 0);
    Sleep(120);
    PostMessageW(widget, WM_LBUTTONUP, 0, 0);
    Sleep(900);

    HWND panel = findPanel();
    if (!panel) {
        fwprintf(stderr, L"panel not open\n");
        return 2;
    }
    RECT r{};
    GetWindowRect(panel, &r);
    SetCursorPos(400, 400);  // move cursor off the panel (kills hover state)
    Sleep(400);
    HDC screen = GetDC(nullptr);
    bool ok = SavePng(screen, r, L"C:\\Users\\Admin\\AppData\\Local\\Temp\\tnl_repo\\docs\\panel.png");
    ReleaseDC(nullptr, screen);
    // Close the panel again.
    PostMessageW(widget, WM_LBUTTONDOWN, MK_LBUTTON, 0);
    Sleep(120);
    PostMessageW(widget, WM_LBUTTONUP, 0, 0);
    wprintf(L"panel=%dx%d saved=%d\n", r.right - r.left, r.bottom - r.top, ok);
    GdiplusShutdown(token);
    return ok ? 0 : 3;
}
