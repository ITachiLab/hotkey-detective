/*!
 * \file dllmain.c
 * \brief This is a DLL that keeps all hook procedures.
 * \author Krzysztof "Itachi" Bąk
 */

#include <stdio.h>
#include "dllmain.h"

HANDLE mapping_handle;
DWORD *listening_thread_id;

BOOL WINAPI DllMain(HINSTANCE h_inst, DWORD reason, LPVOID reserved) {
	switch(reason)
	{
		case DLL_PROCESS_ATTACH:
			mapping_handle = OpenFileMapping(
					FILE_MAP_READ, FALSE, "Local\\HotkeyDetectiveThreadId");
			if (!mapping_handle) {
				OutputDebugString("Couldn't open memory file mapping.");
				return FALSE;
			}

			listening_thread_id = MapViewOfFile(
					mapping_handle, FILE_MAP_READ, 0, 0, sizeof(DWORD));
			if (!listening_thread_id) {
				OutputDebugString("Couldn't create a view of file.");
				return FALSE;
			}
			break;

		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
			break;

		case DLL_PROCESS_DETACH:
			UnmapViewOfFile(listening_thread_id);
			CloseHandle(mapping_handle);
			break;
	}
	return TRUE;
}

LRESULT CALLBACK getmessage_hook(int n_code, WPARAM w_param, LPARAM l_param) {
	if (n_code == HC_ACTION) {
		MSG *msg = (MSG*)l_param;

		if (LOWORD(msg->message) == WM_HOTKEY) {
			PostThreadMessageW(*listening_thread_id, WM_NULL, (WPARAM)msg->hwnd,
					  msg->lParam);
		}
	}

	return CallNextHookEx(NULL, n_code, w_param, l_param);
}

LRESULT CALLBACK wndproc_hook(int n_code, WPARAM w_param, LPARAM l_param) {
	if (n_code == HC_ACTION) {
		CWPSTRUCT *cwp = (CWPSTRUCT*)l_param;

		if (LOWORD(cwp->message) == WM_HOTKEY) {
			PostThreadMessageW(*listening_thread_id, WM_NULL, (WPARAM)cwp->hwnd,
							   cwp->lParam);
		}
	}

	return CallNextHookEx(NULL, n_code, w_param, l_param);
}