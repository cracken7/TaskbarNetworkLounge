// shot.cpp - capture a screen region to PNG via GDI + GDI+ (no PowerShell).
// Usage: shot.exe <out.png> [x y w h]   (default: whole primary screen)
#include <windows.h>
#include <gdiplus.h>
#include <stdio.h>
#include <stdlib.h>

using namespace Gdiplus;

static int GetPngEncoder(CLSID* clsid) {
    UINT num = 0, size = 0;
    GetImageEncodersSize(&num, &size);
    if (!size) return 0;
    ImageCodecInfo* info = (ImageCodecInfo*)malloc(size);
    if (!info) return 0;
    GetImageEncoders(num, size, info);
    int ok = 0;
    for (UINT i = 0; i < num; i++) {
        if (wcscmp(info[i].MimeType, L"image/png") == 0) {
            *clsid = info[i].Clsid;
            ok = 1;
            break;
        }
    }
    free(info);
    return ok;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("usage: shot <out.png> [x y w h]\n");
        return 1;
    }
    SetProcessDPIAware();

    int x = 0, y = 0;
    int w = GetSystemMetrics(SM_CXSCREEN);
    int h = GetSystemMetrics(SM_CYSCREEN);
    if (argc >= 6) {
        x = atoi(argv[2]);
        y = atoi(argv[3]);
        w = atoi(argv[4]);
        h = atoi(argv[5]);
    }

    GdiplusStartupInput gdiplusInput;
    ULONG_PTR token = 0;
    if (GdiplusStartup(&token, &gdiplusInput, nullptr) != Ok) {
        printf("GdiplusStartup failed\n");
        return 1;
    }

    HDC screen = GetDC(nullptr);
    HDC mem = CreateCompatibleDC(screen);
    HBITMAP bitmap = CreateCompatibleBitmap(screen, w, h);
    HBITMAP old = (HBITMAP)SelectObject(mem, bitmap);
    // CAPTUREBLT so layered windows (our widget) are included.
    BitBlt(mem, 0, 0, w, h, screen, x, y, SRCCOPY | CAPTUREBLT);
    SelectObject(mem, old);

    wchar_t outPath[MAX_PATH] = {};
    MultiByteToWideChar(CP_UTF8, 0, argv[1], -1, outPath, MAX_PATH);

    CLSID png;
    int rc = 1;
    if (GetPngEncoder(&png)) {
        Bitmap* image = Bitmap::FromHBITMAP(bitmap, nullptr);
        if (image && image->Save(outPath, &png, nullptr) == Ok) {
            printf("saved %s (%dx%d at %d,%d)\n", argv[1], w, h, x, y);
            rc = 0;
        } else {
            printf("save failed\n");
        }
        delete image;
    } else {
        printf("no png encoder\n");
    }

    DeleteObject(bitmap);
    DeleteDC(mem);
    ReleaseDC(nullptr, screen);
    GdiplusShutdown(token);
    return rc;
}
