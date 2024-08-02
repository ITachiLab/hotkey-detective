/*!
 * \file HkdHook.cpp
 * \brief This is a DLL that keeps all hook procedures.
 * \author Krzysztof "Itachi" Bąk
 */

#include "HkdHook.h"

#include <cstdio>

HANDLE mappingHandle;
HWND *listeningThreadId;
HINSTANCE dllHinst;

BOOL WINAPI DllMain(HINSTANCE hInst, DWORD reason, LPVOID reserved) {
  switch (reason) {
    case DLL_PROCESS_ATTACH:
      dllHinst = hInst;

      mappingHandle = OpenFileMappingW(FILE_MAP_READ, FALSE, MMF_NAME);
      if (!mappingHandle) {
        OutputDebugString("Couldn't open memory file mapping.");
        break;
      }

      listeningThreadId = static_cast<HWND *>(
          MapViewOfFile(mappingHandle, FILE_MAP_READ, 0, 0, sizeof(DWORD)));
      if (!listeningThreadId) {
        OutputDebugString("Couldn't create a view of file.");
        return FALSE;
      }
      break;
    case DLL_PROCESS_DETACH:
      UnmapViewOfFile(listeningThreadId);
      CloseHandle(mappingHandle);
      break;
    default:
      break;
  }
  return TRUE;
}

/*!
 * \brief This is s hook procedure for WH_GETMESSAGE hooks.
 * \sa https://docs.microsoft.com/en-us/previous-versions/windows/desktop/legacy/ms644981(v=vs.85)
 *
 * @param n_code
 * @param w_param
 * @param l_param
 * @return
 */
static LRESULT CALLBACK hookGetMessage(int nCode, WPARAM wParam, LPARAM lParam) {
  if (nCode == HC_ACTION) {
    const MSG *msg = reinterpret_cast<MSG *>(lParam);

    if (LOWORD(msg->message) == WM_HOTKEY) {
      OutputDebugString("WH_GETMESSAGE");
      PostMessageW(*listeningThreadId,
                   WM_NULL,
                   reinterpret_cast<WPARAM>(msg->hwnd),
                   msg->lParam);
    }
  }

  return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

/*!
 * \brief This is a hook procedure for WH_CALLWNDPROC hooks.
 * \sa https://docs.microsoft.com/en-us/previous-versions/windows/desktop/legacy/ms644975(v=vs.85)
 *
 * @param n_code
 * @param w_param
 * @param l_param
 * @return
 */
static LRESULT CALLBACK hookWndProc(int nCode, WPARAM wParam, LPARAM lParam) {
  if (nCode == HC_ACTION) {
    const CWPSTRUCT *cwp = reinterpret_cast<CWPSTRUCT *>(lParam);

    if (LOWORD(cwp->message) == WM_HOTKEY) {
      OutputDebugString("WNDPROC");
      PostMessageW(*listeningThreadId,
                   WM_NULL,
                   reinterpret_cast<WPARAM>(cwp->hwnd),
                   cwp->lParam);
    }
  }

  return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

HHOOK setHooks(int hookType) {
  HOOKPROC hookProc;

  if (hookType == WH_GETMESSAGE) {
    hookProc = hookGetMessage;
  } else {
    hookProc = hookWndProc;
  }

  HHOOK hookHandle = SetWindowsHookExW(hookType, hookProc, dllHinst, 0);
  if (!hookHandle) {
    printf("Couldn't apply hook: %lu\n", GetLastError());
  }

  return hookHandle;
}