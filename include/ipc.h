//
// Created by Itachi on 2021-01-22.
//

#ifndef HOTKEY_DETECTIVE_INCLUDE_IPC_H_
#define HOTKEY_DETECTIVE_INCLUDE_IPC_H_

#include <windows.h>

static const wchar_t MMF_NAME[] = L"Local\\HotkeyDetectiveSharedData";
static const wchar_t EVENT_NAME[] = L"Local\\HotkeyDetectiveShutdownEvent";

typedef struct {
  HWND windowHandle;
  long volatile dllRefCount;
} SharedData, *SharedDataPtr;

#endif //HOTKEY_DETECTIVE_INCLUDE_IPC_H_
