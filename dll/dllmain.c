/*!
 * \file dllmain.c
 * \brief This is a DLL that keeps all hook procedures.
 * \author Krzysztof "Itachi" Bąk
 */

#include <stdio.h>
#include "dllmain.h"

HANDLE mapping_handle;
HANDLE timer_queue;
HANDLE timer;
HANDLE shutdown_event;
SharedDataPtr shared_data;
HINSTANCE dll_hinst;
HANDLE callback_mutex;


VOID CALLBACK timer_callback(_In_ PVOID lpParameter, _In_ BOOLEAN TimerOrWaitFired) {
    if (WaitForSingleObject(callback_mutex, 1000) == WAIT_OBJECT_0) {
        if (dll_hinst && WaitForSingleObject(shutdown_event, 0) == WAIT_OBJECT_0) {
            HINSTANCE instance = dll_hinst;
            dll_hinst = NULL;

            FreeLibrary(instance);
        }

        ReleaseMutex(callback_mutex);
    }
}

BOOL WINAPI DllMain(HINSTANCE h_inst, DWORD reason, LPVOID reserved) {
    switch (reason) {
        case DLL_PROCESS_ATTACH:
            dll_hinst = h_inst;

            shutdown_event = CreateEventW(NULL, TRUE, FALSE, EVENT_NAME);
            if (GetLastError() != ERROR_ALREADY_EXISTS) {
                OutputDebugString("Couldn't find the named event.");

                if (shutdown_event) {
                    goto event_created;
                }

                return FALSE;
            }

            mapping_handle = OpenFileMappingW(FILE_MAP_ALL_ACCESS, FALSE, MMF_NAME);
            if (!mapping_handle) {
                OutputDebugString("Couldn't open memory file mapping.");
                goto event_created;
            }

            shared_data = MapViewOfFile(
                    mapping_handle,
                    FILE_MAP_ALL_ACCESS,
                    0,
                    0,
                    sizeof(SharedData));
            if (!shared_data) {
                OutputDebugString("Couldn't create a view of file.");
                goto mapping_created;
            }

            timer_queue = CreateTimerQueue();
            if (!timer_queue) {
                OutputDebugString("Couldn't create a timer queue.");
                goto view_created;
            }

            if(!CreateTimerQueueTimer(&timer, timer_queue, timer_callback, NULL, 3000, 3000, 0)) {
                OutputDebugString("Couldn't create a timer.");
                goto timer_queue_created;
            }

            InterlockedIncrement(&shared_data->dllRefCount);
            return TRUE;

        timer_queue_created:
            DeleteTimerQueue(timer_queue);
        view_created:
            UnmapViewOfFile(shared_data);
        mapping_created:
            CloseHandle(mapping_handle);
        event_created:
            CloseHandle(shutdown_event);

            return FALSE;
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
            break;

        case DLL_PROCESS_DETACH:
            InterlockedDecrement(&shared_data->dllRefCount);

            DeleteTimerQueueTimer(timer_queue, timer, INVALID_HANDLE_VALUE);
            DeleteTimerQueue(timer_queue);
            UnmapViewOfFile(shared_data);
            CloseHandle(mapping_handle);
            CloseHandle(shutdown_event);

            break;
    }
    return TRUE;
}

LRESULT CALLBACK getmessage_hook(int n_code, WPARAM w_param, LPARAM l_param) {
    if (n_code == HC_ACTION) {
        MSG *msg = (MSG *)l_param;

        if (LOWORD(msg->message) == WM_HOTKEY) {
            OutputDebugString("WH_GETMESSAGE");
            PostMessageW(shared_data->windowHandle, WM_NULL,
                         (WPARAM)msg->hwnd, msg->lParam);
        }
    }

    return CallNextHookEx(NULL, n_code, w_param, l_param);
}

LRESULT CALLBACK wndproc_hook(int n_code, WPARAM w_param, LPARAM l_param) {
    if (n_code == HC_ACTION) {
        CWPSTRUCT *cwp = (CWPSTRUCT *)l_param;

        if (LOWORD(cwp->message) == WM_HOTKEY) {
            OutputDebugString("WNDPROC");
            PostMessageW(shared_data->windowHandle, WM_NULL,
                         (WPARAM)cwp->hwnd, cwp->lParam);
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