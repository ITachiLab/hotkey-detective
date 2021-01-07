//
// Created by Itachi on 2021-01-06.
//

#include <exception>
#include "Core.h"
#include "../dll/dllmain.h"

Core::Core() {
    mappedFileHandle = CreateFileMappingW(
            INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(DWORD),
            MMF_NAME);

    if (!mappedFileHandle) {
        throw std::exception("Couldn't create a memory mapped file.");
    }

    mainWindowThreadId = (HWND *)MapViewOfFile(mappedFileHandle,
                                                FILE_MAP_ALL_ACCESS,
                                                0, 0, sizeof(DWORD));
    if (!mainWindowThreadId) {
        CloseHandle(mappedFileHandle);
        throw std::exception("Couldn't create a view of the mapped file.");
    }
}

Core::~Core() {
    UnmapViewOfFile(mainWindowThreadId);
    CloseHandle(mappedFileHandle);
    UnhookWindowsHookEx(getMessageHookHandle);
    UnhookWindowsHookEx(wndProcHookHandle);
}

void Core::setHooks() {
    getMessageHookHandle = set_hook(WH_GETMESSAGE);
    if (!getMessageHookHandle) {
        throw std::exception("Couldn't hook WM_GETMESSAGE.");
    }

    wndProcHookHandle = set_hook(WH_CALLWNDPROC);
    if (!wndProcHookHandle) {
        UnhookWindowsHookEx(getMessageHookHandle);
        throw std::exception("Couldn't hook WH_CALLWNDPROC.");
    }
}

void Core::getProcessPath(DWORD processId, PROCESS_PATH_BUFF buffer) {
    HANDLE process = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, processId);
    if (!process) {
        wcscpy_s(buffer, MAX_PATH, L"");
    }

    DWORD written = MAX_PATH;
    QueryFullProcessImageNameW(process, 0, buffer, &written);
}

void Core::keystrokeToString(LPARAM hotkey_lparam, KEYSTROKE_BUFF buf) {
    char tmp[8] = {0};
    memset(buf, 0, KEYSTROKE_BUFF_SIZE);

    if (LOWORD(hotkey_lparam) & MOD_ALT) {
        strcat_s(buf, KEYSTROKE_BUFF_SIZE, "ALT + ");
    }

    if (LOWORD(hotkey_lparam) & MOD_CONTROL) {
        strcat_s(buf, KEYSTROKE_BUFF_SIZE, "CTRL + ");
    }

    if (LOWORD(hotkey_lparam) & MOD_SHIFT) {
        strcat_s(buf, KEYSTROKE_BUFF_SIZE, "SHIFT + ");
    }

    if (LOWORD(hotkey_lparam) & MOD_WIN) {
        strcat_s(buf, KEYSTROKE_BUFF_SIZE, "WIN + ");
    }

    UINT scan_code = MapVirtualKeyW(HIWORD(hotkey_lparam), MAPVK_VK_TO_VSC);
    GetKeyNameTextA((LPARAM)(scan_code << 16), tmp, 8);

    strcat_s(buf, KEYSTROKE_BUFF_SIZE, tmp);
}
