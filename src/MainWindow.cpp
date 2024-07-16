/*!
 * \file    MainWindow.cpp
 * \brief   Implementation of the main window.
 * \author  Itachi
 * \date    2021-01-03
 */
#include "MainWindow.h"

#include <commctrl.h>

#include "resource.h"

static constexpr wchar_t CLASS_NAME[] = APP_TITLE;

MainWindow *MainWindow::instance = nullptr;

MainWindow::MainWindow(const HINSTANCE hInstance)
    : windowInstance(hInstance), windowHandle(), hotkeyTable() {
  INITCOMMONCONTROLSEX icex = {};
  icex.dwICC = ICC_LISTVIEW_CLASSES;
  InitCommonControlsEx(&icex);

  WNDCLASSW wc = {};
  wc.lpfnWndProc = windowProc;
  wc.hInstance = hInstance;
  wc.lpszClassName = CLASS_NAME;

  RegisterClassW(&wc);

  mainIcon = LoadIconW(hInstance, MAKEINTRESOURCE(ID_ICON_MAIN));
}

LRESULT MainWindow::windowProc(const HWND hwnd, const UINT uMsg,
                               const WPARAM wParam, const LPARAM lParam) {
  MainWindow *self = instance;

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

      GetWindowThreadProcessId(reinterpret_cast<HWND>(wParam), &proc_id);

      Core::keystrokeToString(lParam, keystrokeBuffer);
      Core::getProcessPath(proc_id, processPathBuffer);

      self->hotkeyTable.addEntry(keystrokeBuffer, processPathBuffer);

      return 0;
    }
    case WM_DESTROY:
      delete self;

      PostQuitMessage(0);
      return 0;
    default:
      break;
  }

  return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

MainWindow *MainWindow::GetInstance(HINSTANCE hInstance) {
  if (instance == nullptr) {
    instance = new MainWindow(hInstance);
    instance->createWindow();
  }

  return instance;
}

void MainWindow::createWindow() {
  windowHandle = CreateWindowExW(0,
                                 CLASS_NAME,
                                 APP_TITLE,
                                 WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
                                 CW_USEDEFAULT,
                                 CW_USEDEFAULT,
                                 600,
                                 300,
                                 nullptr,
                                 nullptr,
                                 windowInstance,
                                 nullptr);

  if (mainIcon != nullptr) {
    SendMessage(
        windowHandle, WM_SETICON, ICON_BIG, reinterpret_cast<LPARAM>(mainIcon));
  }

  core.setMainWindowThreadId(windowHandle);
  core.setHooks();
}

MainWindow::~MainWindow() {
  if (mainIcon != nullptr) {
    DestroyIcon(mainIcon);
  }
}
