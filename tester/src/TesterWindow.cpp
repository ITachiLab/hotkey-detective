#include "TesterWindow.h"

#include <array>
#include <exception>

#include "resource.h"

static constexpr std::array<int, 5> rhkControls = {
    IDC_RHK_ALT, IDC_RHK_CTRL, IDC_RHK_KEY, IDC_RHK_SHIFT, IDC_RHK_WIN};

INT_PTR TesterWindow::show() const {
  return DialogBoxParam(hInstance,
                        MAKEINTRESOURCE(IDD_MAIN),
                        nullptr,
                        dialogProcDispatcher,
                        reinterpret_cast<LPARAM>(this));
}

TesterWindow::~TesterWindow() { UnregisterHotKey(windowHandle, 1); }

void TesterWindow::enableRegisterHotKey(bool enabled) {
  if (enabled) {
    for (auto id : rhkControls) {
      EnableWindow(GetDlgItem(windowHandle, id), false);
    }

    std::array<wchar_t, 2> buffer = {};
    GetDlgItemText(getHandle(), IDC_RHK_KEY, buffer.data(), 2);
    const unsigned key = LOBYTE(VkKeyScanEx(buffer[0], GetKeyboardLayout(0)));

    unsigned modifiers = 0;

    if (IsDlgButtonChecked(windowHandle, IDC_RHK_SHIFT)) {
      modifiers |= MOD_SHIFT;
    }

    if (IsDlgButtonChecked(windowHandle, IDC_RHK_ALT)) {
      modifiers |= MOD_ALT;
    }

    if (IsDlgButtonChecked(windowHandle, IDC_RHK_CTRL)) {
      modifiers |= MOD_CONTROL;
    }

    if (IsDlgButtonChecked(windowHandle, IDC_RHK_WIN)) {
      modifiers |= MOD_WIN;
    }

    RegisterHotKey(windowHandle, 1, modifiers, key);
  } else {
    UnregisterHotKey(windowHandle, 1);

    for (auto id : rhkControls) {
      EnableWindow(GetDlgItem(windowHandle, id), true);
    }
  }
}

INT_PTR TesterWindow::dialogProc(const HWND hwnd, const UINT uMsg,
                                 const WPARAM wParam, const LPARAM lParam) {
  switch (uMsg) {
    case WM_COMMAND:
      switch (LOWORD(wParam)) {
        case IDC_RHK_ENABLED:
          enableRegisterHotKey(IsDlgButtonChecked(hwnd, IDC_RHK_ENABLED));
      }
      break;
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
