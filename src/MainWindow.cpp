//
// Created by Itachi on 2021-01-03.
//

#include "MainWindow.h"
#include "../dll/dllmain.h"
#include <commctrl.h>

static const wchar_t CLASS_NAME[] = APP_TITLE;

MainWindow *MainWindow::instance = nullptr;

MainWindow::MainWindow(const HINSTANCE hInstance)
        : windowInstance(hInstance) {
    INITCOMMONCONTROLSEX icex = {};
    icex.dwICC = ICC_LISTVIEW_CLASSES;
    InitCommonControlsEx(&icex);

    WNDCLASSW wc = {};
    wc.lpfnWndProc = window_proc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClassW(&wc);
}

LRESULT MainWindow::window_proc(HWND hwnd, UINT uMsg, WPARAM wParam,
                                LPARAM lParam) {
    MainWindow *self = MainWindow::instance;

    switch (uMsg) {
        case WM_CREATE:
            self->hotkeyTable.addToWindow(hwnd, self->windowInstance);
            return 0;
        case WM_NOTIFY:
            self->hotkeyTable.handleWmNotify(lParam);
            return 0;
        case WM_NULL: {
            PROCESS_PATH_BUFF processPathBuffer;
            KEYSTROKE_BUFF keystrokeBuffer;
            DWORD proc_id;

            GetWindowThreadProcessId((HWND)wParam, &proc_id);

            Core::keystrokeToString(lParam, keystrokeBuffer);
            Core::getProcessPath(proc_id, processPathBuffer);

            self->hotkeyTable.addEntry(keystrokeBuffer, processPathBuffer);

            return 0;
        }
        case WM_DESTROY:
            delete self;

            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

MainWindow *MainWindow::GetInstance(HINSTANCE hInstance) {
    if (MainWindow::instance == nullptr) {
        MainWindow::instance = new MainWindow(hInstance);
        MainWindow::instance->createWindow();
    }

    return MainWindow::instance;
}

void MainWindow::createWindow() {
    windowHandle = CreateWindowExW(0,
                                   CLASS_NAME,
                                   APP_TITLE,
                                   WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
                                   CW_USEDEFAULT,
                                   CW_USEDEFAULT,
                                   400,
                                   300,
                                   NULL,
                                   NULL,
                                   windowInstance,
                                   NULL);

    core.setMainWindowThreadId(windowHandle);
    core.setHooks();
}
