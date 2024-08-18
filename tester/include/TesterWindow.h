#ifndef TESTERWINDOW_H
#define TESTERWINDOW_H

#include <windows.h>

/**
 * The implementation of the Hotkey Tester.
 */
class TesterWindow final {
  HWND windowHandle = nullptr;
  HINSTANCE hInstance;

  /**
   * Dispatch a message to the TesterWindow::dialogProc method.
   *
   * This is a callback dispatcher used by the main dialog instance. Due to
   * restrictions, it must be kept as a static method to be callable by the
   * window instance. This method, however, is not the true dialog procedure,
   * it's merely a dispatcher which routes messages to the TesterWindow instance
   * carried in the window's extra data.
   *
   * @param hwnd the dialog handle
   * @param uMsg the message
   * @param wParam the WPARAM of the message
   * @param lParam the LPARAM of the message
   * @return The result of DialogProc.
   */
  static INT_PTR CALLBACK dialogProcDispatcher(HWND hwnd, UINT uMsg,
                                               WPARAM wParam, LPARAM lParam);

  /**
   * The dialog procedure of the main dialog window.
   *
   * @param hwnd the dialog handle
   * @param uMsg the message
   * @param wParam the WPARAM of the message
   * @param lParam the LPARAM of the message
   * @return The result of DialogProc.
   */
  INT_PTR dialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

  /**
   * Enable or disable hotkey registered with RegisterHotkey.
   *
   * @param enabled true to register a hotkey, false to unregister
   */
  void TesterWindow::enableRegisterHotKey(bool enabled);
 public:
  /**
   * Create a TesterWindow instance.
   *
   * @param hInstance the application instance
   */
  explicit TesterWindow(HINSTANCE hInstance) : hInstance(hInstance) {}

  /**
   * Perform additional clean-up tasks when window closes.
   *
   * This is a good place to unregister registered hotkeys (if not unregistered
   * already), and destroy all allocated objects.
   */
  ~TesterWindow();

  /**
   * Show the dialog, and block until it closes.
   *
   * @return The result of the dialog when it closes.
   */
  [[nodiscard]] INT_PTR show() const;

  /**
   * Get the handle of the dialog.
   *
   * @return The handle of the dialog.
   */
  [[nodiscard]] HWND getHandle() const { return windowHandle; }
};

#endif  // TESTERWINDOW_H
