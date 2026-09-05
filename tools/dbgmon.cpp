// dbgmon.cpp - minimal OutputDebugString monitor (DBWIN protocol).
// Used to read Wh_Log() output from Windhawk mods without the GUI DbgView.
// Usage: dbgmon.exe [seconds] [filter-substring]
#include <windows.h>
#include <stdio.h>
#include <string.h>

struct DbwinBuffer {
    DWORD processId;
    char data[4096 - sizeof(DWORD)];
};

int main(int argc, char** argv) {
    int seconds = (argc > 1) ? atoi(argv[1]) : 30;
    const char* filter = (argc > 2) ? argv[2] : nullptr;

    SECURITY_ATTRIBUTES sa{};
    SECURITY_DESCRIPTOR sd{};
    InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);
    SetSecurityDescriptorDacl(&sd, TRUE, nullptr, FALSE);
    sa.nLength = sizeof(sa);
    sa.lpSecurityDescriptor = &sd;
    sa.bInheritHandle = FALSE;

    HANDLE bufferReady = CreateEventA(&sa, FALSE, FALSE, "DBWIN_BUFFER_READY");
    HANDLE dataReady = CreateEventA(&sa, FALSE, FALSE, "DBWIN_DATA_READY");
    if (!bufferReady || !dataReady) {
        printf("[dbgmon] CreateEvent failed (%lu)\n", GetLastError());
        return 1;
    }
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        printf("[dbgmon] warning: another debug monitor may be active\n");
    }

    HANDLE mapping = CreateFileMappingA(INVALID_HANDLE_VALUE, &sa,
                                        PAGE_READWRITE, 0, sizeof(DbwinBuffer),
                                        "DBWIN_BUFFER");
    if (!mapping) {
        printf("[dbgmon] CreateFileMapping failed (%lu)\n", GetLastError());
        return 1;
    }
    DbwinBuffer* buffer =
        (DbwinBuffer*)MapViewOfFile(mapping, FILE_MAP_READ, 0, 0, 0);
    if (!buffer) {
        printf("[dbgmon] MapViewOfFile failed (%lu)\n", GetLastError());
        return 1;
    }

    printf("[dbgmon] listening for %d seconds%s%s\n", seconds,
           filter ? ", filter=" : "", filter ? filter : "");
    fflush(stdout);

    ULONGLONG deadline = GetTickCount64() + (ULONGLONG)seconds * 1000;
    SetEvent(bufferReady);

    while (GetTickCount64() < deadline) {
        DWORD remaining = (DWORD)(deadline - GetTickCount64());
        DWORD wait = WaitForSingleObject(dataReady, remaining > 500 ? 500 : remaining);
        if (wait == WAIT_OBJECT_0) {
            DWORD pid = buffer->processId;
            char text[sizeof(buffer->data) + 1];
            memcpy(text, buffer->data, sizeof(buffer->data));
            text[sizeof(buffer->data)] = '\0';
            size_t length = strlen(text);
            while (length && (text[length - 1] == '\n' || text[length - 1] == '\r')) {
                text[--length] = '\0';
            }
            if (!filter || strstr(text, filter)) {
                printf("[%lu] %s\n", pid, text);
                fflush(stdout);
            }
            SetEvent(bufferReady);
        }
    }

    printf("[dbgmon] done\n");
    UnmapViewOfFile(buffer);
    CloseHandle(mapping);
    CloseHandle(dataReady);
    CloseHandle(bufferReady);
    return 0;
}
