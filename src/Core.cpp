/*!
 * \file    Core.cpp
 * \brief   An implementation of the Core module.
 * \author  Itachi
 * \date    2021-01-06
 */

#include "Core.h"

#include <exception>

#include "../dll/HkdHook.h"

Core::Core() {
  mappedFileHandle = CreateFileMapping(
      INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(HkdHookData), MMF_NAME);

  if (!mappedFileHandle) {
    throw std::exception("Couldn't create a memory mapped file.");
  }

  sharedData = static_cast<HkdHookData *>(MapViewOfFile(
      mappedFileHandle, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(HkdHookData)));
  if (!sharedData) {
    CloseHandle(mappedFileHandle);
    throw std::exception("Couldn't create a view of the mapped file.");
  }

  terminatingEventHandle = CreateEvent(nullptr, true, false, TERMINATE_EVENT_NAME);
  if (GetLastError() == ERROR_ALREADY_EXISTS) {
    throw std::exception("The event already existed but it shouldn't");
  }
}

Core::~Core() {
  UnmapViewOfFile(sharedData);
  CloseHandle(mappedFileHandle);
  CloseHandle(terminatingEventHandle);
}

void Core::removeHooks() {
  UnhookWindowsHookEx(getMessageHookHandle);
  UnhookWindowsHookEx(wndProcHookHandle);
}

void Core::setHooks() {
  getMessageHookHandle = setupHook(WH_GETMESSAGE);
  if (!getMessageHookHandle) {
    throw std::exception("Couldn't hook WM_GETMESSAGE.");
  }

  wndProcHookHandle = setupHook(WH_CALLWNDPROC);
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
