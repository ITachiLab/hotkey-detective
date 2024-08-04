#ifndef HOTKEY_DETECTIVE_INCLUDE_DLLMAIN_H_
#define HOTKEY_DETECTIVE_INCLUDE_DLLMAIN_H_

#include <Windows.h>

#include <cstdint>

#define DLL_MESSAGE_ID 0xDEAD

static const wchar_t MMF_NAME[] = L"Local\\HkdSharedData";
static const wchar_t TERMINATE_EVENT_NAME[] = L"Local\\HkdTerminateEvent";

#pragma pack(4)
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

#ifdef __cplusplus
extern "C" {
#endif

#if DLL_COMPILE
#define LIB_EXPORT __declspec(dllexport)
#else
#define LIB_EXPORT __declspec(dllimport)
#endif

/*!
 * \brief Sets a hook on all running threads.
 *
 * \param[in] hookId   an ID of the hook
 * \return A windowHandle to the hook object, needed to close it later, or NULL
 * when the specified id_hook is not supported.
 */
LIB_EXPORT HHOOK setupHook(int hookId);

#ifdef __cplusplus
}
#endif

#endif //HOTKEY_DETECTIVE_INCLUDE_DLLMAIN_H_