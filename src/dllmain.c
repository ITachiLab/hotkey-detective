/*!
 * \file dllmain.c
 * \brief This is a DLL that keeps all hook procedures.
 * \author Krzysztof "Itachi" Bąk
 */

#include <stdio.h>
#include "dllmain.h"

HANDLE mapping_handle;
DWORD *listening_thread_id;
HINSTANCE dll_hinst;

BOOL WINAPI DllMain(HINSTANCE h_inst, DWORD reason, LPVOID reserved) {
    switch (reason) {
        case DLL_PROCESS_ATTACH:
            dll_hinst = h_inst;

            mapping_handle = OpenFileMappingW(FILE_MAP_READ, FALSE, MMF_NAME);
            if (!mapping_handle) {
                OutputDebugString("Couldn't open memory file mapping.");
                break;
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
        MSG *msg = (MSG *)l_param;

        if (LOWORD(msg->message) == WM_HOTKEY) {
            PostThreadMessageW(*listening_thread_id, WM_NULL, (WPARAM)msg->hwnd,
                               msg->lParam);
        }
    }

    return CallNextHookEx(NULL, n_code, w_param, l_param);
}

LRESULT CALLBACK wndproc_hook(int n_code, WPARAM w_param, LPARAM l_param) {
    if (n_code == HC_ACTION) {
        CWPSTRUCT *cwp = (CWPSTRUCT *)l_param;

        if (LOWORD(cwp->message) == WM_HOTKEY) {
            PostThreadMessageW(*listening_thread_id, WM_NULL, (WPARAM)cwp->hwnd,
                               cwp->lParam);
        }
    }

    return CallNextHookEx(NULL, n_code, w_param, l_param);
}

HHOOK set_hook(int id_hook) {
    HOOKPROC hook_proc;

    if (id_hook == WH_GETMESSAGE) {
        hook_proc = getmessage_hook;
    } else {
        hook_proc = wndproc_hook;
    }

    HHOOK hook_handle = SetWindowsHookExW(id_hook, hook_proc, dll_hinst, 0);
    if (!hook_handle) {
        printf("Couldn't apply hook: %lu\n", GetLastError());
    }

    return hook_handle;
}