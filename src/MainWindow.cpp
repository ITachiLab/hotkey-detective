/*!
 * \file    MainWindow.cpp
 * \brief   Implementation of the main window.
 * \author  Itachi
 * \date    2021-01-03
 */
#include "MainWindow.h"
#include "CloseDialog.hpp"

#include <KeySequence.h>
#include <commctrl.h>

#include <string>

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

  windowHandle =
      CreateWindow(CLASS_NAME,
                   APP_TITLE,
                   WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
                   CW_USEDEFAULT,
                   CW_USEDEFAULT,
                   600,
                   300,
                   nullptr,
                   nullptr,
                   windowInstance,
                   this);  // MainWindow instance for WM_CREATE message purposes

  mainIcon = LoadIconW(hInstance, MAKEINTRESOURCE(IDI_MAIN));
  if (mainIcon != nullptr) {
    SendMessage(
        windowHandle, WM_SETICON, ICON_BIG, reinterpret_cast<LPARAM>(mainIcon));
  }

  core.setMainWindowHandle(windowHandle);
  core.setHooks();

  // Keep the MainWindow's instance in the window's extra data, so it can be
  // later received in the window procedure.
  SetWindowLongPtr(
      windowHandle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
}

bool MainWindow::processWmKeyDownUp(const UINT message, const LPARAM lParam) {
  Key k = Key::fromWindowMessage(lParam);
  sequencer.addKeyStroke(k);

  if (message == WM_KEYDOWN || message == WM_SYSKEYDOWN) {
    if (sequencer.isCombination()) {
      // If we get this far, that means nothing blocked the key combination, so
      // it can be added to the table as "Unassigned".
      // debugPrint("%ls\n", sequencer.getCombinationString().c_str());
      hotkeyTable.addEntry(sequencer.getCombinationString(), L"[Unassigned]");
      return true;
    }
  }

  return false;
}

LRESULT MainWindow::windowProc(const HWND hwnd, const UINT uMsg,
                               const WPARAM wParam, const LPARAM lParam) {
  switch (uMsg) {
    case WM_KILLFOCUS:
      sequencer.clear();
      break;
    case WM_CREATE:
      hotkeyTable.addToWindow(hwnd, windowInstance);
      return 0;
    case WM_NOTIFY:
      hotkeyTable.handleWmNotify(lParam);
      return 0;
    case WM_NULL: {
      DWORD proc_id;
      GetWindowThreadProcessId(reinterpret_cast<HWND>(wParam), &proc_id);

      hotkeyTable.addEntry(
          KeySequence::fromGlobalHotKey(lParam).getCombinationString(),
          Core::getProcessPath(proc_id));

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
    case WM_CLOSE: {
      CloseDialog::ProcData procData{&core};
      core.removeHooks();
      core.setTerminatingEvent();

      DialogBoxParam(nullptr,
                     MAKEINTRESOURCE(IDD_ON_CLOSE),
                     hwnd,
                     CloseDialog::dialogProc,
                     LPARAM(&procData));

      DestroyWindow(hwnd);
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
