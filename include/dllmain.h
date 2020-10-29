#ifndef HOTKEY_DETECTIVE_INCLUDE_DLLMAIN_H_
#define HOTKEY_DETECTIVE_INCLUDE_DLLMAIN_H_

#include <windows.h>

static const wchar_t MMF_NAME[] = L"Local\\HotkeyDetectiveThreadId";

/*!
 * \brief This is s hook procedure for WH_GETMESSAGE hooks.
 * \sa https://docs.microsoft.com/en-us/previous-versions/windows/desktop/legacy/ms644981(v=vs.85)
 *
 * @param n_code
 * @param w_param
 * @param l_param
 * @return
 */
LRESULT CALLBACK getmessage_hook(int n_code, WPARAM w_param, LPARAM l_param);

/*!
 * \brief This is a hook procedure for WH_CALLWNDPROC hooks.
 * \sa https://docs.microsoft.com/en-us/previous-versions/windows/desktop/legacy/ms644975(v=vs.85)
 *
 * @param n_code
 * @param w_param
 * @param l_param
 * @return
 */
LRESULT CALLBACK wndproc_hook(int n_code, WPARAM w_param, LPARAM l_param);

/*!
 * \brief Sets a hook on all running threads.
 *
 * \param[in] id_hook	an ID of the hook
 * \return A handle to the hook object, needed to close it later, or NULL when
 * the specified id_hook is not supported.
 */
__declspec(dllexport) HHOOK set_hook(int id_hook);

#endif //HOTKEY_DETECTIVE_INCLUDE_DLLMAIN_H_