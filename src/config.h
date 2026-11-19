#ifndef CONFIG_H
#define CONFIG_H

#include <windows.h>

#define CONFIG_MAX_ITEMS 64
#define CONFIG_ITEM_LEN 128

typedef struct {
    wchar_t open[CONFIG_MAX_ITEMS][CONFIG_ITEM_LEN];
    size_t open_count;
    wchar_t close[CONFIG_MAX_ITEMS][CONFIG_ITEM_LEN];
    size_t close_count;
} Profile;

BOOL config_path(wchar_t *buffer, DWORD capacity);
BOOL config_load_profile(const wchar_t *path, const wchar_t *name, Profile *profile,
                         wchar_t *error, size_t error_capacity);

#endif
