#ifndef HOTKEY_DETECTIVE_INCLUDE_CORE_H_
#define HOTKEY_DETECTIVE_INCLUDE_CORE_H_

#include <windows.h>

extern DWORD *thread_id;
extern const char GETMESSAGE_HOOK_PROC[];
extern const char WNDPROC_HOOK_PROC[];

/*!
 * \brief Creates a memory mapped file for sharing this thread's ID.
 *
 * To enable communication between the detective process and all processes with
 * the DLL injected, the main process's thread ID must be shared with them.
 * The easiest solution is to create a memory mapped file without a physical
 * file on a disk.
 *
 * \return A HANDLE to the memory mapped file object, or NULL on failure.
 */
HANDLE create_map_file(void);

/*!
 * \brief Loads and returns a handle to the DLL with the hook procedure.
 *
 * The hook procedure must be kept in a separate DLL to be properly injected.
 * The handle must be closed using the \c FreeLibrary \c function.
 *
 * \return A HMODULE pointing to the loaded library.
 */
HMODULE get_hook_library(void);

/*!
 * \brief Returns a handle to the hook procedure.
 *
 * \param[in] hook_library	a handle to the DLL keeping the hook procedure
 * \param[in] proc_name		a name of the procedure
 * \return A handle to the hook procedure or NULL when not found.
 */
HOOKPROC get_hook_proc(HMODULE hook_library, const char* proc_name);

/*!
 * \brief Sets a hook on all running threads.
 *
 * \param[in] hook_library	a handle to the DLL keeping the hook procedure
 * \param[in] id_hook		an ID of the hook
 * \param[in] hook_proc		a handle to the hook from the above library
 * \return A handle to the hook object, needed to close it later.
 */
HHOOK set_hook(HMODULE hook_library, int id_hook, HOOKPROC hook_proc);

/*!
 * \brief Gets a memory view of the memory mapped file.
 *
 * This function will retrieve the address where the shared memory has been
 * allocated.
 *
 * \param[in] map_file a handle to the memory mapped file object
 * \return An address of the allocated shared memory.
 */
DWORD *get_memory_view(HANDLE map_file);

/*!
 * \brief A callback for handling console events.
 *
 * In order to shutdown the console app gracefully, we intercept CTRL+C
 * keystroke and send a WM_QUIT message to the thread.
 *
 * \param signal[in] a type of console event
 * \return
 */
BOOL WINAPI console_handler(DWORD signal);

/*!
 * \brief Returns a full path of the process EXE by its ID.
 *
 * \param[in]	proc_id a process ID
 * \param[out]	buff	a pointer to the buffer which will receive the EXE path
 * \return If the function succeeds, the return value is the number of
 * characters written to the buffer. In case of the error, the return value is
 * 0.
 */
DWORD get_proc_path(DWORD proc_id, wchar_t *buff);

/*!
 * \brief Converts a keystroke from WM_HOTKEY's lParam to human readable form.
 *
 * This function extracts the keystroke from the \c hotkey_lparam \c and,
 * converts it to the string form (like: ALT + A), and writes the result to
 * \c buf \c.
 *
 * @param[in]	hotkey_lparam	an lParam from WM_HOTKEY message
 * @param[out]	buf				a buffer to which the string representation will
 * 								be written, must be 32 bytes long or more
 */
void keystroke_to_str(LPARAM hotkey_lparam, char *buf);

/*!
 * \brief Displays the information about a found hook.
 *
 * @param[in] key_stroke	a string containing the hotkey sequence
 * @param[in] proc_path 	a path to the process owning the hotkey
 */
void display_info(char *key_stroke, wchar_t *proc_path);

#endif //HOTKEY_DETECTIVE_INCLUDE_CORE// _H_
