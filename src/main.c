#include "config.h"
#include <wchar.h>
#include <stdio.h>

static void print_items(const wchar_t *label, wchar_t items[CONFIG_MAX_ITEMS][CONFIG_ITEM_LEN], size_t count) {
    wprintf(L"%ls:", label);
    if (!count) { wprintf(L" (none)\n"); return; }
    wprintf(L"\n");
    for (size_t i = 0; i < count; ++i) wprintf(L"  - %ls\n", items[i]);
}

int wmain(int argc, wchar_t **argv) {
    if (argc != 2 || argv[1][0] == L'-') {
        wprintf(L"Usage: launcher.exe <profile>\nExample: launcher.exe college\n");
        return 2;
    }
    wchar_t path[32768], error[512];
    Profile profile;
    if (!config_path(path, _countof(path))) {
        fwprintf(stderr, L"Could not locate profiles.ini next to the executable.\n");
        return 1;
    }
    DWORD attrs = GetFileAttributesW(path);
    if (attrs == INVALID_FILE_ATTRIBUTES || (attrs & FILE_ATTRIBUTE_DIRECTORY)) {
        fwprintf(stderr, L"Configuration file not found: %ls\n", path);
        return 1;
    }
    if (!config_load_profile(path, argv[1], &profile, error, _countof(error))) {
        fwprintf(stderr, L"Error: %ls\n", error);
        return 1;
    }
    wprintf(L"Profile '%ls' validated (%ls).\n", argv[1], path);
    print_items(L"Open", profile.open, profile.open_count);
    print_items(L"Close", profile.close, profile.close_count);
    wprintf(L"\nValidation mode: no processes were started or closed.\n");
    return 0;
}
