// dpiscale.cpp - read / set the display scaling of the primary monitor at
// runtime, so DPI handling can be tested without signing out.
//
// Uses the same undocumented DisplayConfig DPI ioctl the Settings app uses
// (DISPLAYCONFIG_DEVICE_INFO_TYPE -3 get / -4 set). The change is live and
// fully reversible; this tool always prints the original value so it can be
// restored.
//
// Usage: dpiscale.exe get
//        dpiscale.exe set <percent>     (100 125 150 175 200 ... must be one of
//                                        the values Windows reports as valid)
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DISPLAYCONFIG_DEVICE_INFO_GET_DPI ((DISPLAYCONFIG_DEVICE_INFO_TYPE)-3)
#define DISPLAYCONFIG_DEVICE_INFO_SET_DPI ((DISPLAYCONFIG_DEVICE_INFO_TYPE)-4)

struct DISPLAYCONFIG_SOURCE_DPI_SCALE_GET {
    DISPLAYCONFIG_DEVICE_INFO_HEADER header;
    int minScaleRel;      // relative index of the smallest supported scale
    int curScaleRel;      // relative index of the current scale
    int maxScaleRel;      // relative index of the largest supported scale
};

struct DISPLAYCONFIG_SOURCE_DPI_SCALE_SET {
    DISPLAYCONFIG_DEVICE_INFO_HEADER header;
    int scaleRel;
};

// The DPI values Windows exposes, in order; index 0 is 100%.
static const int kScales[] = {100, 125, 150, 175, 200, 225, 250, 300, 350,
                              400, 450, 500};
static const int kScaleCount = (int)(sizeof(kScales) / sizeof(kScales[0]));

static bool GetPrimaryPath(LUID* adapterId, UINT32* sourceId) {
    UINT32 pathCount = 0, modeCount = 0;
    if (GetDisplayConfigBufferSizes(QDC_ONLY_ACTIVE_PATHS, &pathCount,
                                    &modeCount) != ERROR_SUCCESS) {
        return false;
    }
    DISPLAYCONFIG_PATH_INFO* paths = (DISPLAYCONFIG_PATH_INFO*)calloc(
        pathCount, sizeof(DISPLAYCONFIG_PATH_INFO));
    DISPLAYCONFIG_MODE_INFO* modes = (DISPLAYCONFIG_MODE_INFO*)calloc(
        modeCount, sizeof(DISPLAYCONFIG_MODE_INFO));
    bool ok = false;
    if (paths && modes &&
        QueryDisplayConfig(QDC_ONLY_ACTIVE_PATHS, &pathCount, paths, &modeCount,
                           modes, nullptr) == ERROR_SUCCESS) {
        for (UINT32 i = 0; i < pathCount; i++) {
            // Primary source has its desktop origin at (0,0).
            for (UINT32 m = 0; m < modeCount; m++) {
                if (modes[m].infoType == DISPLAYCONFIG_MODE_INFO_TYPE_SOURCE &&
                    modes[m].id == paths[i].sourceInfo.id &&
                    modes[m].adapterId.LowPart ==
                        paths[i].sourceInfo.adapterId.LowPart &&
                    modes[m].sourceMode.position.x == 0 &&
                    modes[m].sourceMode.position.y == 0) {
                    *adapterId = paths[i].sourceInfo.adapterId;
                    *sourceId = paths[i].sourceInfo.id;
                    ok = true;
                    break;
                }
            }
            if (ok) break;
        }
    }
    free(paths);
    free(modes);
    return ok;
}

int main(int argc, char** argv) {
    const char* cmd = argc > 1 ? argv[1] : "get";

    LUID adapterId{};
    UINT32 sourceId = 0;
    if (!GetPrimaryPath(&adapterId, &sourceId)) {
        printf("could not locate the primary display path\n");
        return 1;
    }

    DISPLAYCONFIG_SOURCE_DPI_SCALE_GET get{};
    get.header.type = DISPLAYCONFIG_DEVICE_INFO_GET_DPI;
    get.header.size = sizeof(get);
    get.header.adapterId = adapterId;
    get.header.id = sourceId;
    if (DisplayConfigGetDeviceInfo(&get.header) != ERROR_SUCCESS) {
        printf("DisplayConfigGetDeviceInfo(GET_DPI) failed\n");
        return 1;
    }

    // curScaleRel is relative to the recommended scale, whose absolute index is
    // -minScaleRel in the kScales table.
    int recommendedIndex = -get.minScaleRel;
    int currentIndex = recommendedIndex + get.curScaleRel;
    if (currentIndex < 0) currentIndex = 0;
    if (currentIndex >= kScaleCount) currentIndex = kScaleCount - 1;

    if (!strcmp(cmd, "get")) {
        printf("current scale   : %d%%\n", kScales[currentIndex]);
        printf("recommended     : %d%%\n",
               kScales[recommendedIndex < kScaleCount && recommendedIndex >= 0
                           ? recommendedIndex
                           : 0]);
        printf("supported range : ");
        for (int rel = get.minScaleRel; rel <= get.maxScaleRel; rel++) {
            int index = recommendedIndex + rel;
            if (index >= 0 && index < kScaleCount) {
                printf("%d%% ", kScales[index]);
            }
        }
        printf("\n");
        return 0;
    }

    if (!strcmp(cmd, "set")) {
        if (argc < 3) {
            printf("usage: dpiscale.exe set <percent>\n");
            return 1;
        }
        int wanted = atoi(argv[2]);
        int wantedIndex = -1;
        for (int i = 0; i < kScaleCount; i++) {
            if (kScales[i] == wanted) {
                wantedIndex = i;
                break;
            }
        }
        if (wantedIndex < 0) {
            printf("%d%% is not a known Windows scale value\n", wanted);
            return 1;
        }
        int rel = wantedIndex - recommendedIndex;
        if (rel < get.minScaleRel || rel > get.maxScaleRel) {
            printf("%d%% is outside the supported range for this display\n",
                   wanted);
            return 1;
        }

        DISPLAYCONFIG_SOURCE_DPI_SCALE_SET set{};
        set.header.type = DISPLAYCONFIG_DEVICE_INFO_SET_DPI;
        set.header.size = sizeof(set);
        set.header.adapterId = adapterId;
        set.header.id = sourceId;
        set.scaleRel = rel;
        LONG result = DisplayConfigSetDeviceInfo(&set.header);
        if (result != ERROR_SUCCESS) {
            printf("DisplayConfigSetDeviceInfo(SET_DPI) failed: %ld\n", result);
            return 1;
        }
        printf("scale set to %d%% (was %d%%)\n", wanted, kScales[currentIndex]);
        return 0;
    }

    printf("usage: dpiscale.exe get | set <percent>\n");
    return 1;
}
