#ifndef HOTKEY_DETECTIVE_INCLUDE_DLLMAIN_H_
#define HOTKEY_DETECTIVE_INCLUDE_DLLMAIN_H_

#include <Windows.h>

#define DLL_MESSAGE_ID 0xDEAD

static const wchar_t MMF_NAME[] = L"Local\\HotkeyDetectiveThreadId";

#ifdef __cplusplus
extern "C" {
#endif

#if DLL_COMPILE
#define LIB_EXPORT __declspec(dllexport)
#else
#define LIB_EXPORT __declspec(dllimport)
#endif

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
 * \param[in] id_hook   an ID of the hook
 * \return A windowHandle to the hook object, needed to close it later, or NULL when
 * the specified id_hook is not supported.
 */
LIB_EXPORT HHOOK set_hook(int id_hook);

#ifdef __cplusplus
}
#endif

#endif //HOTKEY_DETECTIVE_INCLUDE_DLLMAIN_H_