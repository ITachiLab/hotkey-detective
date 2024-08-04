/*!
 * \file HkdHook.cpp
 * \brief This is a DLL that keeps all hook procedures.
 * \author Krzysztof "Itachi" Bąk
 */

#include "HkdHook.h"

#include <shlwapi.h>

#include <string>

static constexpr auto EXPLORER_EXE = L"explorer.exe";
static constexpr auto HKD_EXE = L"hotkeydetective.exe";

static HANDLE sharedDataMapping;
static HANDLE terminatingEvent;
static HkdHookData *sharedData;
static HINSTANCE dllHinst;
static bool injected;

/*!
 * \brief The procedure of the terminating thread.
 *
 * @param lpParameter the unused input parameter of the thread
 * @return The thread's return code.
 */
static DWORD WINAPI terminatorThreadProc([[maybe_unused]] LPVOID lpParameter) {
  WaitForSingleObject(terminatingEvent, INFINITE);
  FreeLibraryAndExitThread(dllHinst, 0);
}

/*!
 * \brief Create the terminating thread.
 *
 * The terminating thread is responsible for freeing the injected library when
 * Hotkey Detective quits. This is done by creating a thread which waits
 * indifinitely until the global event is set.
 *
 * @return The thread's handle.
 */
static HANDLE createTerminatorThread() {
  terminatingEvent = CreateEvent(nullptr, true, false, TERMINATE_EVENT_NAME);

  if (terminatingEvent) {
    // The event should already have been created by Hotkey Detective
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
      const auto threadHandle =
          CreateThread(nullptr, 0, terminatorThreadProc, nullptr, 0, nullptr);

      return threadHandle;
    }

    // Event didn't exist and has been created here, this is unacceptable and
    // the handle must be closed
    CloseHandle(terminatingEvent);
  }

  return nullptr;
}

/*!
 * \brief Check if the current process matches with the supplied name.
 *
 * @return True if process's name under which this DLL instance runs contains
 * the given string.
 */
static bool checkProcessIs(const wchar_t *processName) {
  wchar_t buffer[MAX_PATH];
  GetModuleFileName(nullptr, buffer, MAX_PATH);
  return StrStrI(buffer, processName);
}

/*!
 * \brief Map the memory of the shared data.
 *
 * @return True if mapping of the shared data succeeded.
 */
static bool mapSharedData() {
  sharedDataMapping = OpenFileMappingW(FILE_MAP_ALL_ACCESS, false, MMF_NAME);
  if (!sharedDataMapping) {
    return false;
  }

  sharedData = static_cast<HkdHookData *>(MapViewOfFile(
      sharedDataMapping, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(HkdHookData)));
  if (!sharedData) {
    return false;
  }

  return true;
}

BOOL WINAPI DllMain(HINSTANCE hInst, DWORD reason, LPVOID reserved) {
  dllHinst = hInst;
  DisableThreadLibraryCalls(hInst);

  if (reason == DLL_PROCESS_ATTACH) {
    // No need to inject into explorer.exe, it doesn't like that
    if (checkProcessIs(EXPLORER_EXE)) {
      return false;
    }

    // For Hotkey Detective itself, load the library but do not setup anything
    if (checkProcessIs(HKD_EXE)) {
      return true;
    }

    if (!mapSharedData()) {
      return false;
    }

    if (!createTerminatorThread()) {
      return false;
    }

    InterlockedIncrement(&sharedData->injectCounter);
    injected = true;
  }

  if (reason == DLL_PROCESS_DETACH) {
    if (injected) {
      InterlockedDecrement(&sharedData->injectCounter);
    }

    if (sharedData) {
      UnmapViewOfFile(sharedData);
    }

    if (sharedDataMapping) {
      CloseHandle(sharedDataMapping);
    }

    if (terminatingEvent) {
      CloseHandle(terminatingEvent);
    }
  }

  return true;
}

/*!
 * \brief This is a hook procedure for monitoring messages posted to message
 *        queue.
 *
 * This hook is called for each message posted to the message queue of the
 * injected process. This allows Hotkey Detective to detect programs waiting for
 * WM_HOTKEY messages.
 *
 * \sa
 * https://docs.microsoft.com/en-us/previous-versions/windows/desktop/legacy/ms644981(v=vs.85)
 *
 * @param code   specifies whether the hook procedure must process the message
 * @param wParam specifies whether the message has been removed from the queue
 * @param lParam a pointer to an MSG structure that contains details about the
 *               message
 * @return The result of invoking CallNextHookEx with the same parameters.
 */
static LRESULT CALLBACK hookGetMessage(int code, WPARAM wParam, LPARAM lParam) {
  if (!checkProcessIs(HKD_EXE)) {
    const MSG *msg = reinterpret_cast<MSG *>(lParam);

    if (LOWORD(msg->message) == WM_HOTKEY) {
      PostMessageW(sharedData->hkdWindowHandle,
                   WM_NULL,
                   reinterpret_cast<WPARAM>(msg->hwnd),
                   msg->lParam);
    }
  }

  return CallNextHookEx(nullptr, code, wParam, lParam);
}

/*!
 * \brief This is a hook procedure for monitoring messages right before they are
 *        posted to the message queue.
 *
 * \sa
 * https://docs.microsoft.com/en-us/previous-versions/windows/desktop/legacy/ms644975(v=vs.85)
 *
 * @param code   specifies whether the hook procedure must process the message
 * @param wParam specifies whether the message was sent by the current thread
 * @param lParam a pointer to a CWPSTRUCT structure that contains details about
 *               the message
 * @return The result of invoking CallNextHookEx with the same parameters.
 */
static LRESULT CALLBACK hookWndProc(int code, WPARAM wParam, LPARAM lParam) {
  if (!checkProcessIs(HKD_EXE)) {
    const CWPSTRUCT *cwp = reinterpret_cast<CWPSTRUCT *>(lParam);

    if (LOWORD(cwp->message) == WM_HOTKEY) {
      PostMessageW(sharedData->hkdWindowHandle,
                   WM_NULL,
                   reinterpret_cast<WPARAM>(cwp->hwnd),
                   cwp->lParam);
    }
  }

  return CallNextHookEx(nullptr, code, wParam, lParam);
}

HHOOK setupHook(int hookType) {
  HOOKPROC hookProc;

  if (hookType == WH_GETMESSAGE) {
    hookProc = hookGetMessage;
  } else {
    hookProc = hookWndProc;
  }

  return SetWindowsHookExW(hookType, hookProc, dllHinst, 0);
}