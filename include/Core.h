/*!
 * \file    Core.h
 * \brief   A header of the Core module.
 * \author  Itachi
 * \date    2021-01-06
 *
 * The Core module contains logic necessary to set up hooks and a shred memory
 * mapped file. It also provides a set of definitions.
 */

#ifndef HOTKEY_DETECTIVE_SRC_CORE_H_
#define HOTKEY_DETECTIVE_SRC_CORE_H_

#include <windows.h>

#define KEYSTROKE_BUFF_SIZE 32

typedef char KEYSTROKE_BUFF[KEYSTROKE_BUFF_SIZE];
typedef wchar_t PROCESS_PATH_BUFF[MAX_PATH];

/*!
 * \brief This class contains the core functions used by Hotkey Detective.
 *
 * The core functions includes:
 *  - creation of memory mapped files,
 *  - setting a hook,
 *  - helper methods for process identification.
 *
 * The Core module automatically creates a memory mapped file when constructed.
 */
class Core {
 private:
  HANDLE mappedFileHandle;      //!< A handle of the memory mapped file

  HWND *mainWindowHandle;       //!< A pointer to the shared memory where the
                                //!< main window's handle will be stored

  HHOOK getMessageHookHandle;   //!< A handle of the WH_GETMESSAGE hook

  HHOOK wndProcHookHandle;      //!< A handle of the WH_CALLWNDPROC hook
 public:
  /*!
   * \brief Returns a full path of the process EXE by its ID.
   *
   * \param[in]   proc_id a process ID
   * \param[out]  buff    a pointer to the buffer which will receive the EXE
   *                      path
   * \return If the function succeeds, the return value is the number of
   * characters written to the buffer. In case of the error, the returned
   * value is 0.
   */
  static void getProcessPath(DWORD processId, PROCESS_PATH_BUFF buffer);

  /*!
   * \brief Converts a keystroke from WM_HOTKEY's lParam to human readable form.
   *
   * This function extracts the keystroke from the \c hotkey_lparam \c,
   * converts it to the string form (like: Alt + A), and writes the result to
   * \c buf \c.
   *
   * @param[in]   hotkey_lparam   an lParam from WM_HOTKEY message
   * @param[out]  buf             a buffer to which the string representation
   *                              will be written
   */
  static void keystrokeToString(LPARAM hotkey_lparam, KEYSTROKE_BUFF buf);

  /*!
   * \brief The main constructor, sets up the memory mapped file.
   */
  Core();

  /*!
   * \brief Destroys the memory mapped file and unhooks hooked processes.
   *
   * TODO: This won't unload the DLL from the injected processes, this must be
   *       done separately in the future.
   */
  virtual ~Core();

  /*!
   * \brief Sets the hotkey hook on all processes.
   *
   * This function calls the DLL's set_hook(int) function to set up hotkey hooks
   * on all threads and windows.
   */
  void setHooks();

  /*!
   * \brief Sets the main window's handle in the shared memory, to use by
   *        hooked processes.
   *
   * In order to allow hooked processes to communicate with Hotkey Detective,
   * they must know the main window's handle. This way they can send messages
   * directly to the window's procedure.
   *
   * The mainWindowHandle pointer is set up to point to a named, shared memory
   * area.
   *
   * @param handle a handle of the main window
   */
  void setMainWindowThreadId(HWND handle) { *mainWindowHandle = handle; }
};

#endif //HOTKEY_DETECTIVE_SRC_CORE_H_
