/*!
 * \file    MainWindow.h
 * \brief   Contains stuff related to the main window.
 * \author  Itachi
 * \date    2021-01-03
 *
 * Hotkey Detective's main window is enclosed in a class to form a convenient
 * component easy to interact with.
 */

#ifndef HOTKEY_DETECTIVE_SRC_MAINWINDOW_H_
#define HOTKEY_DETECTIVE_SRC_MAINWINDOW_H_

#include <KeySequence.h>
#include <windows.h>

#include "Core.h"
#include "HotkeyTable.h"

#define APP_TITLE L"Hotkey Detective"

/*!
 * \brief This is a class wrapping the main window.
 *
 * This class creates the main window itself, as well as other components that
 * belongs to it.
 */
class MainWindow final {
  HINSTANCE windowInstance;
  HWND windowHandle;
  HICON mainIcon;

  HotkeyTable hotkeyTable;
  Core core;
  KeySequence sequencer;

  /*!
   * \brief Main window procedure dispatcher.
   *
   * This is a callback dispatcher used by the main window instance. Due to
   * restrictions, it must be kept as a static method to be callable by the
   * window instance. This method, however, is not the true window procedure,
   * it's merely a dispatcher which routes messages to the MainWindow instance
   * carried in the window's extra data.
   *
   * @param hwnd
   * @param uMsg
   * @param wParam
   * @param lParam
   * @return
   */
  static LRESULT CALLBACK windowProcDispatcher(HWND hwnd, UINT uMsg,
                                               WPARAM wParam, LPARAM lParam);

  /*!
   * \brief Set keyboard hooks for the main window.
   *
   * Main window hooks are used to detect keystrokes which are not assigned to
   * any process, in order to provide the user with a feedback and possibly
   * useful information that either no process owns the global shortcut, or the
   * process can't be detected by HKD.
   */
  void setMainWindowKeyboardHook();

  /*!
   * \brief Main wnidow procedure.
   *
   * This method processes all main window's messages.
   *
   * @param hwnd the window's handle
   * @param uMsg the message
   * @param wParam the WPARAM
   * @param lParam the LPARAM
   *
   * @return The returned value differs depending on the received message.
   */
  LRESULT windowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

  /*!
   * \brief Process WM_(SYS)KEY(DOWN/UP) messages.
   *
   * @param message the message
   * @param lParam  the LPARAM assigned to the message
   */
  void processWmKeyDownUp(UINT message, LPARAM lParam);

 public:
  ~MainWindow();

  /*!
   * \brief Constructs the main window from scratch.
   *
   * @param[in] hInstance an instance of the module associated with the window
   */
  explicit MainWindow(HINSTANCE hInstance);

  /*!
   * \brief Returns a handle of the main window.
   *
   * @return A handle of the main window.
   */
  [[nodiscard]] HWND getHandle() const { return windowHandle; }
};

#endif  // HOTKEY_DETECTIVE_SRC_MAINWINDOW_H_
