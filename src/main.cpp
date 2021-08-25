/*!
 * \file    main.cpp
 * \brief   The main file of Hotkey Detective.
 * \author  Itachi
 * \date    2020-12-01
 */

#include "main.h"

#include <HotkeyTable.h>
#include <MainWindow.h>
#include <WindowsUtils.h>
#include <resource.h>

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    PWSTR cmdLine, int cmdShow) {

    if (!WindowsUtils::isUserAdmin()) {
        int userSelection = MessageBoxW(
                nullptr, WindowsUtils::resStr(ID_STRING_ADMIN_WARNING),
                WindowsUtils::resStr(ID_STRING_APP_NAME),
                MB_YESNO | MB_ICONWARNING);

        if (userSelection == IDNO) {
            return 0;
        }
    }

    MainWindow *window = MainWindow::GetInstance(hInstance);

    ShowWindow(window->getHandle(), cmdShow);

    MSG msg = {};

    while (GetMessageW(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    return 0;
}