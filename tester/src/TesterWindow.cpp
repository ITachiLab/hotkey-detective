#include "TesterWindow.h"

#include <commctrl.h>
#include <debug.h>

#include <array>
#include <exception>

#include "resource.h"

static constexpr std::array<int, 5> rhkControls = {
    IDC_RHK_ALT, IDC_RHK_CTRL, IDC_RHK_KEY, IDC_RHK_SHIFT, IDC_RHK_WIN};

static constexpr std::array<int, 4> wmsControls = {
    IDC_WMS_ALT, IDC_WMS_CTRL, IDC_WMS_KEY, IDC_WMS_SHIFT};

INT_PTR TesterWindow::show() const {
  return DialogBoxParam(hInstance,
                        MAKEINTRESOURCE(IDD_MAIN),
                        nullptr,
                        dialogProcDispatcher,
                        reinterpret_cast<LPARAM>(this));
}

TesterWindow::~TesterWindow() {
  enableRegisterHotKey(false);
  enableWmSetHotKey(false);
}

template <std::size_t N>
void TesterWindow::enableControlsCollection(const std::array<int, N> &controls,
                                            bool newState) {
  for (auto id : controls) {
    EnableWindow(GetDlgItem(windowHandle, id), newState);
  }
}

void TesterWindow::enableRegisterHotKey(const bool enabled) {
  if (enabled) {
    enableControlsCollection(rhkControls, false);

    std::array<wchar_t, 2> buffer = {};
    GetDlgItemText(getHandle(), IDC_RHK_KEY, buffer.data(), 2);
    const unsigned key = LOBYTE(VkKeyScanEx(buffer[0], GetKeyboardLayout(0)));

    unsigned modifiers = 0;

    modifiers = IsDlgButtonChecked(windowHandle, IDC_RHK_SHIFT) * MOD_SHIFT |
                IsDlgButtonChecked(windowHandle, IDC_RHK_ALT) * MOD_ALT |
                IsDlgButtonChecked(windowHandle, IDC_RHK_CTRL) * MOD_CONTROL |
                IsDlgButtonChecked(windowHandle, IDC_RHK_WIN) * MOD_WIN;

    RegisterHotKey(windowHandle, 1, modifiers, key);
  } else {
    UnregisterHotKey(windowHandle, 1);
    enableControlsCollection(rhkControls, true);
  }
}

void TesterWindow::enableWmSetHotKey(const bool enabled) {
  if (enabled) {
    enableControlsCollection(wmsControls, false);

    std::array<wchar_t, 2> buffer = {};
    GetDlgItemText(getHandle(), IDC_WMS_KEY, buffer.data(), 2);
    const unsigned key = LOBYTE(VkKeyScanEx(buffer[0], GetKeyboardLayout(0)));

    unsigned modifiers = 0;

    modifiers =
        IsDlgButtonChecked(windowHandle, IDC_WMS_SHIFT) * HOTKEYF_SHIFT |
        IsDlgButtonChecked(windowHandle, IDC_WMS_ALT) * HOTKEYF_ALT |
        IsDlgButtonChecked(windowHandle, IDC_WMS_CTRL) * HOTKEYF_CONTROL;

    // Documentation of WM_SETHOTKEY is wrong, it says that key should be placed
    // in the low-order word of wParam, and modifiers in the high-order word of
    // wParam, while the correct layout is: key in the low byte of the low-order
    // word, and modifiers in the high byte of the low-order word. The
    // high-order word is ignored by the system.
    SendMessage(
        windowHandle, WM_SETHOTKEY, MAKEWPARAM(MAKEWORD(key, modifiers), 0), 0);
  } else {
    enableControlsCollection(wmsControls, true);
    SendMessage(windowHandle, WM_SETHOTKEY, 0, 0);
  }
}

INT_PTR TesterWindow::dialogProc(const HWND hwnd, const UINT uMsg,
                                 const WPARAM wParam, const LPARAM lParam) {
  bool ret = false;

  if (uMsg == WM_COMMAND) {
    const unsigned loWord = LOWORD(wParam);

    if (loWord == IDC_RHK_ENABLED) {
      enableRegisterHotKey(IsDlgButtonChecked(hwnd, IDC_RHK_ENABLED));
      ret = true;
    } else if (loWord == IDC_WMS_ENABLED) {
      enableWmSetHotKey(IsDlgButtonChecked(hwnd, IDC_WMS_ENABLED));
      ret = true;
    }
  } else if (uMsg == WM_SYSCOMMAND) {
    const unsigned param = wParam & 0xFFF0;

    if (param == SC_CLOSE) {
      EndDialog(hwnd, true);
      ret = true;
    }
  }

  return ret;
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
