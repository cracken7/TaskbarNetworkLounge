// Console harness for the network-monitoring half of Taskbar Network Lounge.
// Compiles the real p2/p3/p4 sources with stubbed Windhawk APIs so the sampling
// logic, interface selection, formatting and persistence can be verified
// against Windows' own counters without loading the mod into Windhawk.
//
// Usage: test_netmon.exe [seconds] [interfaceMode] [counterMode]

#include <windows.h>

#include <cstdarg>
#include <cstdio>
#include <map>
#include <string>

// ---- Windhawk API stubs ---------------------------------------------------
static std::map<std::wstring, int> g_intSettings;
static std::map<std::wstring, std::wstring> g_strSettings;
static std::map<std::wstring, std::wstring> g_storage;

static void Wh_Log(const wchar_t* format, ...) {
    va_list args;
    va_start(args, format);
    wchar_t buffer[1024];
    _vsnwprintf_s(buffer, 1024, _TRUNCATE, format, args);
    va_end(args);
    wprintf(L"[log] %s\n", buffer);
}

static int Wh_GetIntSetting(const wchar_t* name, ...) {
    auto it = g_intSettings.find(name);
    return it == g_intSettings.end() ? 0 : it->second;
}

static const wchar_t* Wh_GetStringSetting(const wchar_t* name, ...) {
    auto it = g_strSettings.find(name);
    const std::wstring& value = it == g_strSettings.end() ? std::wstring()
                                                        : it->second;
    wchar_t* copy = new wchar_t[value.size() + 1];
    wcscpy_s(copy, value.size() + 1, value.c_str());
    return copy;
}

static void Wh_FreeStringSetting(const wchar_t* value) {
    delete[] value;
}

static size_t Wh_GetStringValue(const wchar_t* name, wchar_t* buffer,
                                size_t chars) {
    auto it = g_storage.find(name);
    if (it == g_storage.end() || it->second.size() + 1 > chars) {
        if (chars) {
            buffer[0] = L'\0';
        }
        return 0;
    }
    wcscpy_s(buffer, chars, it->second.c_str());
    return it->second.size();
}

static BOOL Wh_SetStringValue(const wchar_t* name, const wchar_t* value) {
    g_storage[name] = value;
    return TRUE;
}

// ---- Mod sources under test ----------------------------------------------
#include "../src/p2_core.inc"
#include "../src/p3_settings.inc"
#include "../src/p4_network.inc"

// ---- Test driver ---------------------------------------------------------
static void SetDefaults(const wchar_t* interfaceMode,
                        const wchar_t* counterMode) {
    g_intSettings[L"Appearance.PanelWidth"] = 200;
    g_intSettings[L"Appearance.PanelHeight"] = 48;
    g_intSettings[L"Appearance.FontSize"] = 11;
    g_intSettings[L"Appearance.OffsetX"] = 12;
    g_intSettings[L"Appearance.OffsetY"] = 0;
    g_intSettings[L"Appearance.DpiScaling"] = 1;
    g_intSettings[L"Appearance.AutoTheme"] = 1;
    g_intSettings[L"Appearance.ColorArrows"] = 1;
    g_intSettings[L"Appearance.BgOpacity"] = 0;
    g_intSettings[L"Network.ExcludeVirtual"] = 1;
    g_intSettings[L"Network.UpdateInterval"] = 1000;
    g_intSettings[L"Network.BinaryUnits"] = 1;
    g_intSettings[L"Behavior.ShowTooltip"] = 1;
    g_intSettings[L"Behavior.ShowDetailsOnClick"] = 1;
    g_intSettings[L"Behavior.HideFullscreen"] = 0;
    g_intSettings[L"Behavior.StartEnabled"] = 1;

    g_strSettings[L"Appearance.LayoutMode"] = L"full";
    g_strSettings[L"Appearance.TextColor"] = L"0xFFFFFF";
    g_strSettings[L"Network.InterfaceMode"] = interfaceMode;
    g_strSettings[L"Network.SelectedInterface"] = L"";
    g_strSettings[L"Network.SpeedUnit"] = L"auto";
    g_strSettings[L"Traffic.CounterMode"] = counterMode;
    g_strSettings[L"Traffic.ResetCounters"] = L"none";
}

static void TestFormatters() {
    ModSettings bytes = GetSettings();
    bytes.speedUnit = SpeedUnit::Bytes;
    bytes.binaryUnits = false;  // decimal so the expected values are obvious

    struct Case {
        double bytesPerSec;
        const wchar_t* expected;
    };
    const Case cases[] = {
        {0, L"0 B/s"},
        {850 * 1000.0, L"850 KB/s"},
        {125 * 1000.0, L"125 KB/s"},
        {2.4 * 1000 * 1000, L"2.4 MB/s"},
        {12.4 * 1000 * 1000, L"12.4 MB/s"},
        {18.7 * 1000 * 1000, L"18.7 MB/s"},
        {1.02 * 1000 * 1000 * 1000, L"1.02 GB/s"},
        {1.24 * 1000 * 1000 * 1000, L"1.24 GB/s"},
    };

    int failures = 0;
    wprintf(L"\n--- speed formatter (bytes, decimal) ---\n");
    for (const Case& test : cases) {
        std::wstring actual = FormatSpeed(test.bytesPerSec, bytes);
        bool ok = actual == test.expected;
        failures += ok ? 0 : 1;
        wprintf(L"  %-12s expected %-12s %s\n", actual.c_str(), test.expected,
                ok ? L"OK" : L"MISMATCH");
    }

    ModSettings bits = bytes;
    bits.speedUnit = SpeedUnit::Bits;
    wprintf(L"--- bits (1 byte = 8 bits) ---\n");
    // 12.4 MB/s == 99.2 Mbps
    std::wstring bitsText = FormatSpeed(12.4 * 1000 * 1000, bits);
    bool bitsOk = bitsText == L"99.2 Mbps";
    failures += bitsOk ? 0 : 1;
    wprintf(L"  %-12s expected 99.2 Mbps    %s\n", bitsText.c_str(),
            bitsOk ? L"OK" : L"MISMATCH");

    wprintf(L"--- byte volume (binary) ---\n");
    ModSettings binary = bytes;
    binary.binaryUnits = true;
    struct VolumeCase {
        unsigned long long value;
        const wchar_t* expected;
    };
    const VolumeCase volumes[] = {
        {0, L"0 B"},
        {512, L"512 B"},
        {1024ull * 1024 * 1024 * 4 + 880ull * 1024 * 1024, L"4.86 GB"},
        {1024ull * 1024 * 1024 * 1024 * 3, L"3 TB"},
    };
    for (const VolumeCase& test : volumes) {
        std::wstring actual = FormatBytes(test.value, binary);
        bool ok = actual == test.expected;
        failures += ok ? 0 : 1;
        wprintf(L"  %-12s expected %-12s %s\n", actual.c_str(), test.expected,
                ok ? L"OK" : L"MISMATCH");
    }

    wprintf(failures ? L"\nFORMATTER FAILURES: %d\n" : L"\nformatters OK\n",
            failures);
}

static void TestPersistence() {
    wprintf(L"\n--- persistence round trip ---\n");
    PersistentTotals write;
    write.down = 123456789012ull;
    write.up = 987654321ull;
    bool saved = SavePersistentTotals(write);
    PersistentTotals read;
    bool loaded = LoadPersistentTotals(&read);
    wprintf(L"  file: %s\n", GetStorageFile().c_str());
    wprintf(L"  save=%d load=%d down=%llu up=%llu %s\n", saved ? 1 : 0,
            loaded ? 1 : 0, read.down, read.up,
            (saved && loaded && read.down == write.down && read.up == write.up)
                ? L"OK"
                : L"MISMATCH");

    // Corruption safety: a truncated/garbage file must be rejected, not crash.
    std::wstring path = GetStorageFile();
    HANDLE file = CreateFileW(path.c_str(), GENERIC_WRITE, 0, nullptr,
                              CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file != INVALID_HANDLE_VALUE) {
        const char garbage[] = "TNLv1 not-a-number\n";
        DWORD written = 0;
        WriteFile(file, garbage, (DWORD)strlen(garbage), &written, nullptr);
        CloseHandle(file);
    }
    PersistentTotals corrupt{42, 42};
    bool rejected = !LoadPersistentTotals(&corrupt);
    wprintf(L"  corrupt file rejected: %s\n", rejected ? L"OK" : L"FAILED");
    DeleteFileW(path.c_str());
}

int main(int argc, char** argv) {
    int seconds = argc > 1 ? atoi(argv[1]) : 10;
    const wchar_t* interfaceMode = L"auto";
    const wchar_t* counterMode = L"session";
    wchar_t modeBuffer[64] = {};
    wchar_t counterBuffer[64] = {};
    if (argc > 2) {
        MultiByteToWideChar(CP_UTF8, 0, argv[2], -1, modeBuffer, 64);
        interfaceMode = modeBuffer;
    }
    if (argc > 3) {
        MultiByteToWideChar(CP_UTF8, 0, argv[3], -1, counterBuffer, 64);
        counterMode = counterBuffer;
    }

    SetDefaults(interfaceMode, counterMode);
    LoadSettings();

    ModSettings s = GetSettings();
    wprintf(L"settings: interfaceMode=%s counterMode=%s interval=%dms unit=%d binary=%d\n",
            interfaceMode, counterMode, s.updateInterval, (int)s.speedUnit,
            s.binaryUnits ? 1 : 0);

    TestFormatters();
    TestPersistence();

    wprintf(L"\n--- live sampling for %d seconds ---\n", seconds);
    g_monitor.Initialize();
    ULONGLONG deadline = GetTickCount64() + (ULONGLONG)seconds * 1000;
    int iteration = 0;
    while (GetTickCount64() < deadline) {
        NetSnapshot snapshot;
        bool changed = g_monitor.Sample(&snapshot);
        wprintf(L"[%02d] %-22s %-11s down=%-11s up=%-11s totalDown=%-10s totalUp=%-10s raw(down=%.0f up=%.0f B/s) changed=%d\n",
                ++iteration, snapshot.ifName.c_str(),
                snapshot.connected ? L"connected" : L"disconnected",
                FormatSpeed(snapshot.downBytesPerSec, s).c_str(),
                FormatSpeed(snapshot.upBytesPerSec, s).c_str(),
                FormatBytes(snapshot.totalDown, s).c_str(),
                FormatBytes(snapshot.totalUp, s).c_str(),
                snapshot.downBytesPerSec, snapshot.upBytesPerSec, changed ? 1 : 0);
        fflush(stdout);
        Sleep((DWORD)s.updateInterval);
    }
    g_monitor.Shutdown();
    wprintf(L"done\n");
    return 0;
}
