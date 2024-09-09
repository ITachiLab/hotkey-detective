#ifndef HOTKEY_DETECTIVE_INCLUDE_DLLMAIN_H_
#define HOTKEY_DETECTIVE_INCLUDE_DLLMAIN_H_

#include <Windows.h>

#include <cstdint>

static constexpr wchar_t dllMessage[] = L"HKD_DLL_NOTIFY";
static const wchar_t MMF_NAME[] = L"Local\\HkdSharedData";
static const wchar_t TERMINATE_EVENT_NAME[] = L"Local\\HkdTerminateEvent";

#pragma pack(push, 4)
/*!
 * \struct HkdHookData
 * \brief The structure with various data shared between Hotkey Detective main
 *        application and the DLL with hooks.
 */
struct HkdHookData {
  /*!
   * \brief The counter tracking the number of times the library was loaded and
   *        freed.
   *
   * Each time the library is attached to a process, this counter is incremented
   * atomically. Alike, when the library is detached, the counter is decremented
   * atomically. This allows to check how many processes keep a reference to the
   * DLL when Hotkey Detective is about to be closed. It's not super reliable
   * but it's reliable enough just to display a small dialog informing the user
   * that Hotkey Detective is gracefully shutting down.
   */
  uint32_t injectCounter;

  /*!
   * \brief A handle to the Hotkey Detective's main application window.
   */
  HWND hkdWindowHandle;
};
#pragma pack(pop)

#ifdef __cplusplus
extern "C" {
#endif

#if DLL_COMPILE
#define LIB_EXPORT __declspec(dllexport)
#else
#define LIB_EXPORT __declspec(dllimport)
#endif

/**
 * Set up a global hook in all running processes.
 *
 * This method is called from Hotkey Detective application when it starts.
 * Depending on the supplied `hookType`, a hook of the given type will be set on
 * every running process to which the user running Hotkey Detective has
 * permissions, hence it's better to run Hotkey Detective as administrator.
 *
 * Once Win32::SetWindowsHookEx is invoked, the DLL will be injected to all
 * eligible processes, and the chosen hook will be installed.
 *
 * @param[in] hookType the type of the hook, valid choices are:
 *                     `WH_CALLWNDPROC`, `WH_GETMESSAGE` and `WH_SYSMSGFILTER`
 * @return A handle to the hook object, needed to close it later, or `NULL`
 *         when the specified hook type is not supported.
 */
LIB_EXPORT HHOOK setupHook(int hookType);

#ifdef __cplusplus
}
#endif

#endif //HOTKEY_DETECTIVE_INCLUDE_DLLMAIN_H_