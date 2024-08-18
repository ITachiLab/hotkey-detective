#include "TesterWindow.h"

#include <cstdio>
#include <exception>

#include "resource.h"

INT_PTR TesterWindow::show() const {
  return DialogBoxParam(hInstance,
                        MAKEINTRESOURCE(IDD_MAIN),
                        nullptr,
                        dialogProcDispatcher,
                        reinterpret_cast<LPARAM>(this));
}

TesterWindow::~TesterWindow() {
  UnregisterHotKey(windowHandle, 1);
}

INT_PTR TesterWindow::dialogProc(const HWND hwnd, const UINT uMsg,
                                 const WPARAM wParam, const LPARAM lParam) {
  switch (uMsg) {
    case WM_INITDIALOG:
      RegisterHotKey(hwnd, 1, MOD_ALT | MOD_CONTROL, 0x41);
      return true;
    case WM_HOTKEY:
      SetDlgItemText(hwnd, IDC_EDIT1, L"Got it!");
      return true;
    case WM_SYSCOMMAND:
      if ((wParam & 0xFFF0) == SC_CLOSE) {
        EndDialog(hwnd, true);
        return true;
      }
      break;
    default:
      return false;
  }

  return false;
}

INT_PTR TesterWindow::dialogProcDispatcher(const HWND hwnd, const UINT uMsg,
                                           const WPARAM wParam,
                                           const LPARAM lParam) {
  TesterWindow *thiz;

  // WM_NCCREATE is sent to the window before WM_CREATE, making it the ideal
  // place to store the pointer to the class instance in the window extra data.
  if (uMsg == WM_INITDIALOG) {
    thiz = reinterpret_cast<TesterWindow *>(lParam);
    thiz->windowHandle = hwnd;

    SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(thiz));
  } else {
    thiz =
        reinterpret_cast<TesterWindow *>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
  }

  if (thiz) {
    return thiz->dialogProc(hwnd, uMsg, wParam, lParam);
  }

  return false;
}
