/*!
 * \file    Core.cpp
 * \brief   An implementation of the Core module.
 * \author  Itachi
 * \date    2021-01-06
 */

#include "Core.h"

#include "ipc.h"
#include "../dll/dllmain.h"

#include <exception>

Core::Core() {
    mappedFileHandle = CreateFileMappingW(
            INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(SharedData),
            MMF_NAME);

    if (!mappedFileHandle) {
        throw std::exception("Couldn't create a memory mapped file.");
    }

    sharedData = (SharedDataPtr)MapViewOfFile(mappedFileHandle,
                                             FILE_MAP_ALL_ACCESS,
                                             0, 0, sizeof(SharedData));
    if (!sharedData) {
        CloseHandle(mappedFileHandle);
        throw std::exception("Couldn't create a view of the mapped file.");
    }

    ZeroMemory(sharedData, sizeof(SharedData));
}

Core::~Core() {
    UnmapViewOfFile(sharedData);
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
        strcat_s(buf, KEYSTROKE_BUFF_SIZE, "Alt + ");
    }

    if (LOWORD(hotkey_lparam) & MOD_CONTROL) {
        strcat_s(buf, KEYSTROKE_BUFF_SIZE, "Ctrl + ");
    }

    if (LOWORD(hotkey_lparam) & MOD_SHIFT) {
        strcat_s(buf, KEYSTROKE_BUFF_SIZE, "Shift + ");
    }

    if (LOWORD(hotkey_lparam) & MOD_WIN) {
        strcat_s(buf, KEYSTROKE_BUFF_SIZE, "Win + ");
    }

    UINT scan_code = MapVirtualKeyW(HIWORD(hotkey_lparam), MAPVK_VK_TO_VSC);
    GetKeyNameTextA((LPARAM)(scan_code << 16), tmp, 8);

    strcat_s(buf, KEYSTROKE_BUFF_SIZE, tmp);
}

SharedDataPtr Core::getSharedData() const {
    return sharedData;
}