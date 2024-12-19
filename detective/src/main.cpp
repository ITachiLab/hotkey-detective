/*!
 * \file    main.cpp
 * \brief   The main file of Hotkey Detective.
 * \author  Itachi
 * \date    2020-12-01
 */

#include <windows.h>

#include "MainWindow.h"
#include "WindowsUtils.h"
#include "debug.h"
#include "resource.h"

constexpr const auto localizationPath = L"localization.dll";

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
                    PWSTR lpCmdLine, int nShowCmd) try {
  allocateConsole();
  auto localizationHandle = WindowsUtils::loadModule(
      localizationPath,
      LOAD_LIBRARY_AS_IMAGE_RESOURCE | LOAD_LIBRARY_AS_DATAFILE);

  if (!WindowsUtils::isUserAdmin()) {
    const auto warningMsg =
        WindowsUtils::resStr(IDS_ELEVATION_WARNING, localizationHandle.get());
    // TODO: Should appTitle be localized? This seems redundant, maybe its
    // better to hardcode this in app
    const auto appTitle =
        WindowsUtils::resStr(IDS_APP_TITLE, localizationHandle.get());
    if (const int userSelection = MessageBoxW(nullptr,
                                              warningMsg.c_str(),
                                              appTitle.c_str(),
                                              MB_YESNO | MB_ICONWARNING);
        userSelection == IDNO) {
      return EXIT_SUCCESS;
    }
  }

  auto window = MainWindow(hInstance, localizationHandle.get());
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
  return EXIT_SUCCESS;
} catch (const std::exception& e) {
  const auto errorMsg = WindowsUtils::toWString(e.what());
  MessageBoxW(
      NULL, errorMsg.c_str(), L"Hotkey Detective ERROR!", MB_OK | MB_ICONERROR);
  return EXIT_FAILURE;
}