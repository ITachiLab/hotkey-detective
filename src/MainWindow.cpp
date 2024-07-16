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

MainWindow::MainWindow(const HINSTANCE hInstance)
    : windowInstance(hInstance), windowHandle(), hotkeyTable() {
  INITCOMMONCONTROLSEX icex = {};
  icex.dwICC = ICC_LISTVIEW_CLASSES;
  InitCommonControlsEx(&icex);

  WNDCLASSW wc = {};
  wc.lpfnWndProc = windowProcDispatcher;
  wc.hInstance = hInstance;
  wc.lpszClassName = CLASS_NAME;
  RegisterClass(&wc);

  windowHandle = CreateWindow(CLASS_NAME,
                              APP_TITLE,
                              WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
                              CW_USEDEFAULT,
                              CW_USEDEFAULT,
                              600,
                              300,
                              nullptr,
                              nullptr,
                              windowInstance,
                              this); // MainWindow instance for WM_CREATE message purposes

  mainIcon = LoadIconW(hInstance, MAKEINTRESOURCE(ID_ICON_MAIN));
  if (mainIcon != nullptr) {
    SendMessage(
        windowHandle, WM_SETICON, ICON_BIG, reinterpret_cast<LPARAM>(mainIcon));
  }

  core.setMainWindowThreadId(windowHandle);
  core.setHooks();

  // Keep the MainWindow's instance in the window's extra data, so it can be
  // later received in the window procedure.
  SetWindowLongPtr(
      windowHandle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
}

LRESULT MainWindow::windowProc(const HWND hwnd, const UINT uMsg,
                               const WPARAM wParam, const LPARAM lParam) {
  switch (uMsg) {
    case WM_CREATE:
      hotkeyTable.addToWindow(hwnd, windowInstance);
      return 0;
    case WM_NOTIFY:
      hotkeyTable.handleWmNotify(lParam);
      return 0;
    case WM_NULL: {
      PROCESS_PATH_BUFF processPathBuffer;
      KEYSTROKE_BUFF keystrokeBuffer;
      DWORD proc_id;

      GetWindowThreadProcessId(reinterpret_cast<HWND>(wParam), &proc_id);

      Core::keystrokeToString(lParam, keystrokeBuffer);
      Core::getProcessPath(proc_id, processPathBuffer);

      hotkeyTable.addEntry(keystrokeBuffer, processPathBuffer);

      return 0;
    }
    case WM_PAINT: {
      PAINTSTRUCT ps;
      HDC hdc = BeginPaint(hwnd, &ps);

      // All painting occurs here, between BeginPaint and EndPaint.
      FillRect(hdc, &ps.rcPaint, reinterpret_cast<HBRUSH>((COLOR_WINDOW + 1)));
      EndPaint(hwnd, &ps);
      return 0;
    }
    case WM_DESTROY:
      PostQuitMessage(0);
      return 0;
    default:
      break;
  }

  return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

LRESULT MainWindow::windowProcDispatcher(const HWND hwnd, const UINT uMsg,
                                         const WPARAM wParam,
                                         const LPARAM lParam) {
  auto thiz =
      reinterpret_cast<MainWindow *>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

  if (thiz == nullptr && uMsg == WM_CREATE) {
    const auto cs = reinterpret_cast<CREATESTRUCT *>(lParam);
    thiz = reinterpret_cast<MainWindow *>(cs->lpCreateParams);
  }

  return thiz->windowProc(hwnd, uMsg, wParam, lParam);
}

MainWindow::~MainWindow() {
  if (mainIcon != nullptr) {
    DestroyIcon(mainIcon);
  }
}
