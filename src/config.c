#include "config.h"
#include <wchar.h>
#include <stdio.h>

static BOOL read_list(const wchar_t *path, const wchar_t *section, const wchar_t *key,
                      wchar_t items[CONFIG_MAX_ITEMS][CONFIG_ITEM_LEN], size_t *count,
                      wchar_t *error, size_t error_capacity) {
    wchar_t value[8192];
    GetPrivateProfileStringW(section, key, L"", value, (DWORD)(sizeof(value) / sizeof(value[0])), path);
    *count = 0;
    wchar_t *context = NULL;
    for (wchar_t *token = wcstok_s(value, L",;", &context); token; token = wcstok_s(NULL, L",;", &context)) {
        while (*token == L' ' || *token == L'\t') ++token;
        size_t len = wcslen(token);
        while (len && (token[len - 1] == L' ' || token[len - 1] == L'\t')) token[--len] = L'\0';
        if (!len) continue;
        if (*count >= CONFIG_MAX_ITEMS || len >= CONFIG_ITEM_LEN) {
            _snwprintf_s(error, error_capacity, _TRUNCATE, L"List too large or item name too long in %ls.%ls", section, key);
            return FALSE;
        }
        wcscpy_s(items[*count], CONFIG_ITEM_LEN, token);
        ++*count;
    }
    return TRUE;
}

static BOOL app_exists(const wchar_t *path, const wchar_t *app) {
    wchar_t section[CONFIG_ITEM_LEN + 8];
    wchar_t value[2];
    _snwprintf_s(section, _countof(section), _TRUNCATE, L"app.%ls", app);
    return GetPrivateProfileStringW(section, L"path", L"", value, _countof(value), path) > 0;
}

BOOL config_path(wchar_t *buffer, DWORD capacity) {
    DWORD n = GetModuleFileNameW(NULL, buffer, capacity);
    if (!n || n >= capacity) return FALSE;
    wchar_t *slash = wcsrchr(buffer, L'\\');
    if (!slash) return FALSE;
    slash[1] = L'\0';
    return wcscat_s(buffer, capacity, L"profiles.ini") == 0;
}

BOOL config_load_profile(const wchar_t *path, const wchar_t *name, Profile *profile,
                         wchar_t *error, size_t error_capacity) {
    wchar_t section[CONFIG_ITEM_LEN + 8];
    wchar_t marker[2];
    _snwprintf_s(section, _countof(section), _TRUNCATE, L"profile.%ls", name);
    if (!GetPrivateProfileStringW(section, L"open", NULL, marker, _countof(marker), path)) {
        _snwprintf_s(error, error_capacity, _TRUNCATE, L"Profile '%ls' does not exist or is missing the open key.", name);
        return FALSE;
    }
    ZeroMemory(profile, sizeof(*profile));
    if (!read_list(path, section, L"open", profile->open, &profile->open_count, error, error_capacity) ||
        !read_list(path, section, L"close", profile->close, &profile->close_count, error, error_capacity)) return FALSE;
    for (size_t i = 0; i < profile->open_count; ++i) {
        if (!app_exists(path, profile->open[i])) {
            _snwprintf_s(error, error_capacity, _TRUNCATE, L"Application '%ls' in profile '%ls' has no [app.%ls] section with a path.", profile->open[i], name, profile->open[i]);
            return FALSE;
        }
    }
    for (size_t i = 0; i < profile->close_count; ++i) {
        if (!app_exists(path, profile->close[i])) {
            _snwprintf_s(error, error_capacity, _TRUNCATE, L"Application '%ls' in profile '%ls' has no [app.%ls] section with a path.", profile->close[i], name, profile->close[i]);
            return FALSE;
        }
    }
    return TRUE;
}
