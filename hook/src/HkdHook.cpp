/*!
 * \file HkdHook.cpp
 * \brief This is a DLL that keeps all hook procedures.
 * \author Krzysztof "Itachi" Bąk
 */

#include "HkdHook.h"

#include <shlwapi.h>

#include <string>

static constexpr auto EXPLORER_EXE = L"explorer.exe";
static constexpr auto HKD_EXE = L"hotkey detective.exe";

static HANDLE sharedDataMapping;
static HANDLE terminatingEvent;
static HkdHookData *sharedData;
static HINSTANCE dllHinst;
static bool injected;
static unsigned dllMessageId;

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
    dllMessageId = RegisterWindowMessage(dllMessage);
    if (!dllMessageId) {
      return false;
    }

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

/**
 * Handle normal messages intercepted by `WH_GETMESSAGE` and `WH_CALLWNDPROC`
 * hooks.
 *
 * @param messageType the type of the message
 * @param lParam      the LPARAM of the message
 * @param hwnd        the handle of the window which received the message
 */
static void handleNormalMessage(WORD messageType, LPARAM lParam, HWND hwnd) {
  if (messageType == WM_HOTKEY) {
    PostMessage(sharedData->hkdWindowHandle,
                dllMessageId,
                reinterpret_cast<WPARAM>(hwnd),
                lParam);
  } else if (messageType == WM_ACTIVATE) {
    if (const LRESULT result = SendMessage(hwnd, WM_GETHOTKEY, 0, 0)) {
      const WORD hotkeyData = LOWORD(result);

      PostMessage(sharedData->hkdWindowHandle,
                  dllMessageId,
                  reinterpret_cast<WPARAM>(hwnd),
                  MAKELPARAM(hotkeyData, 0));
    }
  }
}

/**
 * This is a hook procedure for monitoring messages posted to message queue.
 *
 * This hook is called for each message posted to the message queue of the
 * injected process. This allows Hotkey Detective to detect programs waiting for
 * `WM_HOTKEY` messages.
 *
 * @see https://docs.microsoft.com/en-us/previous-versions/windows/desktop/legacy/ms644981(v=vs.85)
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
    const WORD messageType = LOWORD(msg->message);

    handleNormalMessage(messageType, msg->lParam, msg->hwnd);
  }

  return CallNextHookEx(nullptr, code, wParam, lParam);
}

/**
 * This is a hook procedure for monitoring messages right before they are posted
 * to the message queue.
 *
 * @see https://docs.microsoft.com/en-us/previous-versions/windows/desktop/legacy/ms644975(v=vs.85)
 *
 * @param code   specifies whether the hook procedure must process the message
 * @param wParam specifies whether the message was sent by the current thread
 * @param lParam a pointer to a Win32::CWPSTRUCT structure that contains details
 *               about the message
 * @return The result of invoking CallNextHookEx with the same parameters.
 */
static LRESULT CALLBACK hookWndProc(int code, WPARAM wParam, LPARAM lParam) {
  if (!checkProcessIs(HKD_EXE)) {
    const CWPSTRUCT *cwp = reinterpret_cast<CWPSTRUCT *>(lParam);
    const WORD messageType = LOWORD(cwp->message);

    handleNormalMessage(messageType, cwp->lParam, cwp->hwnd);
  }

  return CallNextHookEx(nullptr, code, wParam, lParam);
}

/**
 * The system calls this function after an input event occurs in a dialog box,
 * message box, menu, or scroll bar, but before the message generated by the
 * input event is processed.
 *
 * @param code   the type of input event that generated the message
 * @param wParam this parameter is not used
 * @param lParam a pointer to an MSG message structure
 * @return If nCode is less than zero, the hook procedure must return the value
 *         returned by CallNextHookEx.

 *         If nCode is greater than or equal to zero, and the hook procedure did
 *         not process the message, it is highly recommended that you call
 *         CallNextHookEx and return the value it returns; otherwise, other
 *         applications that have installed WH_SYSMSGFILTER hooks will not
 *         receive hook notifications and may behave incorrectly as a result.

 *         If the hook procedure processed the message, it may return a nonzero
 *         value to prevent the system from passing the message to the target
 *         window procedure.
 */
LRESULT CALLBACK SysMsgProc(int code, WPARAM wParam, LPARAM lParam) {
  if (!checkProcessIs(HKD_EXE)) {
    const MSG *msg = reinterpret_cast<MSG *>(lParam);

     if ((msg->wParam & 0xFFF0) == SC_HOTKEY) {
      if (const LRESULT result = SendMessage(msg->hwnd, WM_GETHOTKEY, 0, 0)) {
        const WORD hotkeyData = LOWORD(result);

        PostMessage(sharedData->hkdWindowHandle,
                    dllMessageId,
                    reinterpret_cast<WPARAM>(msg->hwnd),
                    MAKELPARAM(hotkeyData, 0));
      }
    }
  }

  return CallNextHookEx(nullptr, code, wParam, lParam);
}

HHOOK setupHook(int hookType) {
  HOOKPROC hookProc;

  if (hookType == WH_GETMESSAGE) {
    hookProc = hookGetMessage;
  } else if (hookType == WH_CALLWNDPROC) {
    hookProc = hookWndProc;
  } else if (hookType == WH_SYSMSGFILTER) {
    hookProc = SysMsgProc;
  } else {
    return nullptr;
  }

  return SetWindowsHookEx(hookType, hookProc, dllHinst, 0);
}