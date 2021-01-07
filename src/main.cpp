/*!
 * \file main.c
 * \brief A main file of the Hotkey Detective.
 * \author Krzysztof "Itachi" Bąk
 */

#include <stdio.h>
#include <HotkeyTable.h>
#include <MainWindow.h>

#include "main.h"

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