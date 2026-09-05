// Accuracy harness: download a file of known size through WinINet-free means
// (the caller does the download) while this process samples the mod's monitor,
// then report how the counted bytes compare to the payload size.
//
// Usage: test_accuracy.exe <seconds> <expectedPayloadBytes>
#include <windows.h>

#include <cstdarg>
#include <cstdio>
#include <map>
#include <string>

static std::map<std::wstring, int> g_intSettings;
static std::map<std::wstring, std::wstring> g_strSettings;
static std::map<std::wstring, std::wstring> g_storage;

static void Wh_Log(const wchar_t*, ...) {}

static int Wh_GetIntSetting(const wchar_t* name, ...) {
    auto it = g_intSettings.find(name);
    return it == g_intSettings.end() ? 0 : it->second;
}

static const wchar_t* Wh_GetStringSetting(const wchar_t* name, ...) {
    auto it = g_strSettings.find(name);
    const std::wstring& value =
        it == g_strSettings.end() ? std::wstring() : it->second;
    wchar_t* copy = new wchar_t[value.size() + 1];
    wcscpy_s(copy, value.size() + 1, value.c_str());
    return copy;
}

static void Wh_FreeStringSetting(const wchar_t* value) { delete[] value; }

static size_t Wh_GetStringValue(const wchar_t* name, wchar_t* buffer,
                                size_t chars) {
    auto it = g_storage.find(name);
    if (it == g_storage.end() || it->second.size() + 1 > chars) {
        if (chars) buffer[0] = L'\0';
        return 0;
    }
    wcscpy_s(buffer, chars, it->second.c_str());
    return it->second.size();
}

static BOOL Wh_SetStringValue(const wchar_t* name, const wchar_t* value) {
    g_storage[name] = value;
    return TRUE;
}

#include "../src/p2_core.inc"
#include "../src/p3_settings.inc"
#include "../src/p4_network.inc"

int main(int argc, char** argv) {
    int seconds = argc > 1 ? atoi(argv[1]) : 20;
    unsigned long long expected = argc > 2 ? _strtoui64(argv[2], nullptr, 10) : 0;

    g_intSettings[L"Appearance.PanelWidth"] = 200;
    g_intSettings[L"Appearance.PanelHeight"] = 48;
    g_intSettings[L"Appearance.FontSize"] = 11;
    g_intSettings[L"Network.ExcludeVirtual"] = 1;
    g_intSettings[L"Network.UpdateInterval"] = 500;
    g_intSettings[L"Network.BinaryUnits"] = 1;
    g_intSettings[L"Behavior.StartEnabled"] = 1;
    g_strSettings[L"Appearance.LayoutMode"] = L"full";
    g_strSettings[L"Network.InterfaceMode"] = L"auto";
    g_strSettings[L"Network.SpeedUnit"] = L"auto";
    g_strSettings[L"Traffic.CounterMode"] = L"session";
    g_strSettings[L"Traffic.ResetCounters"] = L"none";
    LoadSettings();

    ModSettings s = GetSettings();
    g_monitor.Initialize();

    NetSnapshot snapshot;
    g_monitor.Sample(&snapshot);  // baseline

    double peakDown = 0.0;
    ULONGLONG deadline = GetTickCount64() + (ULONGLONG)seconds * 1000;
    printf("READY\n");
    fflush(stdout);

    while (GetTickCount64() < deadline) {
        Sleep((DWORD)s.updateInterval);
        g_monitor.Sample(&snapshot);
        if (snapshot.downBytesPerSec > peakDown) {
            peakDown = snapshot.downBytesPerSec;
        }
    }

    printf("counted_down_bytes=%llu\n", snapshot.totalDown);
    printf("counted_up_bytes=%llu\n", snapshot.totalUp);
    printf("peak_down_bytes_per_sec=%.0f\n", peakDown);
    wprintf(L"peak_down_formatted=%s\n", FormatSpeed(peakDown, s).c_str());
    if (expected) {
        double ratio = (double)snapshot.totalDown / (double)expected;
        printf("expected_payload=%llu\n", expected);
        printf("counted/expected=%.4f\n", ratio);
        printf("verdict=%s\n",
               (ratio >= 0.97 && ratio <= 1.25) ? "OK (within overhead)" : "OUT OF RANGE");
    }
    g_monitor.Shutdown();
    return 0;
}
