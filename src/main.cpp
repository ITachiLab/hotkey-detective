/*!
 * \file    main.cpp
 * \brief   The main file of Hotkey Detective.
 * \author  Itachi
 * \date    2020-12-01
 */

#include "main.h"

#include <HotkeyTable.h>
#include <MainWindow.h>

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    PWSTR cmdLine, int cmdShow) {

    MainWindow *window = MainWindow::GetInstance(hInstance);

    ShowWindow(window->getHandle(), cmdShow);

    MSG msg = { };

    while (GetMessageW(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    return 0;
}