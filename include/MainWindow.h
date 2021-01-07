//
// Created by Itachi on 2021-01-03.
//

#ifndef HOTKEY_DETECTIVE_SRC_MAINWINDOW_H_
#define HOTKEY_DETECTIVE_SRC_MAINWINDOW_H_

#include <windows.h>
#include "HotkeyTable.h"
#include "Core.h"

#define APP_TITLE L"Hotkey Detective"

/*!
 * \brief This is a class wrapping the main window.
 *
 * This class creates the main window itself, as well as other components that
 * belongs to it. It's implemented as a singleton pattern, due to the fact that
 * the window callback procedure must be a static method, and it needs access
 * to the MainWindow instance.
 */
class MainWindow {
 private:
  static MainWindow *instance; //!< A singleton instance of this class

  /*!
   * \brief Main window procedure callback
   *
   * This is a callback used by the main window instance. Due to restrictions,
   * it must be kept as a static method to be callable by the window instance.
   *
   * @param hwnd
   * @param uMsg
   * @param wParam
   * @param lParam
   * @return
   */
  static LRESULT CALLBACK window_proc(HWND hwnd, UINT uMsg, WPARAM wParam,
                                      LPARAM lParam);

  HINSTANCE windowInstance; //!< An instance of the module associated with the
                            //!< window
  HWND windowHandle;        //!< A handle to the window

  HotkeyTable hotkeyTable;  //!< An instance of the HotkeyTable
  Core core;                //!< A core utilities

  /*!
   * \brief Hidden constructor, so the class can be used only as a singleton.
   *
   * If you want to obtain the instance of this class, use the
   * MainWindow::GetInstance method instead.
   *
   * @param[in] hInstance an instance of the module associated with the window
   */
  MainWindow(HINSTANCE hInstance);
 public:
  MainWindow(MainWindow &other) = delete;
  void operator=(const MainWindow &) = delete;

  static MainWindow *GetInstance(HINSTANCE hInstance);

  void createWindow();

  HWND getHandle() { return windowHandle; }
};

#endif //HOTKEY_DETECTIVE_SRC_MAINWINDOW_H_
