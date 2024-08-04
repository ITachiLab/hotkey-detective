/*!
 * \file    main.cpp
 * \brief   The main file of Hotkey Detective.
 * \author  Itachi
 * \date    2020-12-01
 */

#include <windows.h>

#include "HotkeyTable.h"
#include "MainWindow.h"
#include "WindowsUtils.h"
#include "debug.h"
#include "resource.h"

/*!
 * \brief Just a WinMain function.
 *
 * @param hInstance[in]     a handle to the current instance of the application
 * @param hPrevInstance[in] a handle to the previous instance of the application
 * @param lpCmdLine[in]     the command line for the application
 * @param nShowCmd[in]      specify how an application is to be displayed when
 *                          it is opened
 * @return If the function succeeds, terminating when it receives a WM_QUIT
 * message, it should return the exit value contained in that message's wParam
 * parameter. If the function terminates before entering the message loop, it
 * should return zero.
 */
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    PWSTR lpCmdLine, int nShowCmd) {
  allocateConsole();

  if (!WindowsUtils::isUserAdmin()) {
    if (const int userSelection =
            MessageBoxW(nullptr,
                        WindowsUtils::resStr(IDS_ELEVATION_WARNING),
                        WindowsUtils::resStr(IDS_APP_TITLE),
                        MB_YESNO | MB_ICONWARNING);
        userSelection == IDNO) {
      return 0;
    }
  }

  auto window = MainWindow(hInstance);
  ShowWindow(window.getHandle(), nShowCmd);

  MSG msg = {};
  while (GetMessageW(&msg, nullptr, 0, 0)) {
    switch (msg.message) {
      case WM_SYSKEYDOWN:
      case WM_SYSKEYUP:
      case WM_KEYDOWN:
      case WM_KEYUP:
        if (window.processWmKeyDownUp(msg.message, msg.lParam)) {
          // If the function returns "true" that means the key combination has
          // been detected, so we must not dispatch the message to other
          // controls.
          break;
        }
      default:
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
  }

  return 0;
}