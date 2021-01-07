//
// Created by Itachi on 2021-01-06.
//

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
  HANDLE mappedFileHandle;
  HWND *mainWindowThreadId;
  HHOOK getMessageHookHandle;
  HHOOK wndProcHookHandle;
 public:
  static void getProcessPath(DWORD processId, PROCESS_PATH_BUFF buffer);

  static void keystrokeToString(LPARAM hotkey_lparam, KEYSTROKE_BUFF buf);

  Core();
  virtual ~Core();

  void setHooks();

  void setMainWindowThreadId(HWND threadId) { *mainWindowThreadId = threadId; }
};

#endif //HOTKEY_DETECTIVE_SRC_CORE_H_
