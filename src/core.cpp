/*!
 * \file core.c
 * \brief Contains all core functionalities.
 * \author Krzysztof "Itachi" Bąk
 */

#include <stdio.h>
#include "core.h"
#include "dllmain.h"

HANDLE create_map_file(void) {
    HANDLE map_file_handle = CreateFileMappingW(
            INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(DWORD),
            MMF_NAME);
    if (!map_file_handle) {
        printf("Couldn't create a memory mapped file: %lu\n", GetLastError());
    }

    return map_file_handle;
}

DWORD *get_memory_view(HANDLE map_file) {
    DWORD *address = (DWORD*) MapViewOfFile(map_file, FILE_MAP_ALL_ACCESS, 0, 0,
                                   sizeof(DWORD));
    if (!address) {
        printf("Couldn't create a view of file: %lu\n", GetLastError());
    }

    return address;
}

BOOL WINAPI console_handler(DWORD signal) {
    if (signal == CTRL_C_EVENT) {
        PostThreadMessage(*thread_id, WM_QUIT, 0, 0);
    }

    return TRUE;
}

DWORD get_proc_path(DWORD proc_id, wchar_t *buff) {
    HANDLE process = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, proc_id);
    if (!process) {
        return 0;
    }

    DWORD written = MAX_PATH;
    QueryFullProcessImageNameW(process, 0, buff, &written);

    return written;
}

void keystroke_to_str(LPARAM hotkey_lparam, char *buf) {
    char tmp[8] = {0};
    memset(buf, 0, 32);

    if (LOWORD(hotkey_lparam) & MOD_ALT) {
        strcat_s(buf, 32, "ALT + ");
    }

    if (LOWORD(hotkey_lparam) & MOD_CONTROL) {
        strcat_s(buf, 32, "CTRL + ");
    }

    if (LOWORD(hotkey_lparam) & MOD_SHIFT) {
        strcat_s(buf, 32, "SHIFT + ");
    }

    if (LOWORD(hotkey_lparam) & MOD_WIN) {
        strcat_s(buf, 32, "WIN + ");
    }

    UINT scan_code = MapVirtualKeyW(HIWORD(hotkey_lparam), MAPVK_VK_TO_VSC);
    GetKeyNameTextA((LPARAM)(scan_code << 16), tmp, 8);

    strcat_s(buf, 32, tmp);
}

void display_info(char *key_stroke, wchar_t *proc_path) {
    printf("\n----- %s -----\n", key_stroke);
    printf("%ws\n", proc_path);
    printf("--------------\n");
}