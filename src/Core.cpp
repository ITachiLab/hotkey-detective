/*!
 * \file    Core.cpp
 * \brief   An implementation of the Core module.
 * \author  Itachi
 * \date    2021-01-06
 */

#include "Core.h"

#include <exception>

#include "../dll/dllmain.h"

Core::Core() {
  mappedFileHandle = CreateFileMappingW(
      INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(DWORD), MMF_NAME);

  if (!mappedFileHandle) {
    throw std::exception("Couldn't create a memory mapped file.");
  }

  mainWindowHandle = (HWND *)MapViewOfFile(
      mappedFileHandle, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(DWORD));
  if (!mainWindowHandle) {
    CloseHandle(mappedFileHandle);
    throw std::exception("Couldn't create a view of the mapped file.");
  }
}

Core::~Core() {
  UnmapViewOfFile(mainWindowHandle);
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

std::wstring Core::getProcessPath(const DWORD processId) {
  HANDLE process = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, processId);
  if (!process) {
    return {L""};
  }

  wchar_t buffer[MAX_PATH];
  DWORD written = MAX_PATH;

  QueryFullProcessImageName(process, 0, buffer, &written);

  return {buffer};
}
