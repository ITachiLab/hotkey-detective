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

#include <string>

#include "../dll/HkdHook.h"

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
class Core final {
  HANDLE mappedFileHandle;      //!< A handle of the memory mapped file

  HkdHookData *sharedData;      //!< A pointer to the shared data

  HHOOK getMessageHookHandle;   //!< A handle of the WH_GETMESSAGE hook

  HHOOK wndProcHookHandle;      //!< A handle of the WH_CALLWNDPROC hook

  HANDLE terminatingEventHandle;
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
  static std::wstring getProcessPath(DWORD processId);

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
  ~Core();

  /*!
   * \brief Sets the hotkey hook on all processes.
   *
   * This function calls the DLL's set_hook(int) function to set up hotkey hooks
   * on all threads and windows.
   */
  void setHooks();

  /*!
   * \brief Unhook previously hooked processes.
   *
   * This method must be called manually *BEFORE* the terminating event is
   * signalized. Failing to do so can cause serious issues and applications
   * crashing because the system could still try to invoke hook procedures from
   * the no longer existing DLL.
   */
  void removeHooks();

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
  void setMainWindowHandle(const HWND handle) {
    sharedData->hkdWindowHandle = handle;
  }

  /*!
   * \brief Get the current number of injected processes.
   *
   * @return The number of processes which hold a reference to the DLL. This
   * value should not be blindly trusted, it is used solely for providing a
   * feedback to the user.
   */
  [[nodiscard]] uint32_t getInjectCount() const {
    return sharedData->injectCounter;
  }

  /*!
   * \brief Signalize the terminating event.
   *
   * Once the event is in signalized state, the DLL should unload itself from
   * all affected processes.
   */
  void setTerminatingEvent() const {
    SetEvent(terminatingEventHandle);
  }
};

#endif //HOTKEY_DETECTIVE_SRC_CORE_H_
