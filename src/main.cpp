/*!
 * \file main.c
 * \brief A main file of the Hotkey Detective.
 * \author Krzysztof "Itachi" Bąk
 */

#include <stdio.h>
#include <HotkeyTable.h>

#include "main.h"
#include "core.h"
#include "dllmain.h"

DWORD *thread_id;
HINSTANCE hInstance;
HotkeyTable *table;

int WINAPI wWinMain(HINSTANCE h_instance, HINSTANCE h_prev_instance,
                    PWSTR cmd_line, int cmd_show) {
    const wchar_t CLASS_NAME[] = L"Hotkey Detective";

    hInstance = h_instance;

    WNDCLASSW wc = { };
    wc.lpfnWndProc = window_proc;
    wc.hInstance = h_instance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClassW(&wc);

    HWND hwnd = CreateWindowExW(0,
                                CLASS_NAME,
                                L"Hotkey Detective",
                                WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
                                CW_USEDEFAULT,
                                CW_USEDEFAULT,
                                400,
                                300,
                                NULL,
                                NULL,
                                h_instance,
                                NULL);

    if (!hwnd) {
        return 0;
    }

    ShowWindow(hwnd, cmd_show);

    MSG msg = { };
    while (GetMessageW(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    return 0;
}

LRESULT CALLBACK window_proc(HWND hwnd, UINT uMsg, WPARAM wParam,
                             LPARAM lParam) {
    switch (uMsg) {
        case WM_CREATE:
        {
            table = new HotkeyTable(hwnd, hInstance);
            table->addItem("Test", L"Test");
        }
        return 0;
        case WM_NOTIFY:
            table->handleWmNotify(lParam);
            return 0;
        case WM_DESTROY:
            delete table;

            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

//int main() {
//	int rv = 0;
//	MSG msg;
//
//	HANDLE map_file = create_map_file();
//	if (!map_file) {
//		rv = -1;
//		goto exit;
//	}
//
//	thread_id = get_memory_view(map_file);
//	if (!thread_id) {
//		rv = -1;
//		goto map_file_created;
//	}
//
//	*thread_id = GetCurrentThreadId();
//
//	// Force pre-creation of the message queue.
//	PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE);
//
//	SetConsoleCtrlHandler(console_handler, TRUE);
//
//	HHOOK getmessage_hook_handle = set_hook(WH_GETMESSAGE);
//	if (!getmessage_hook_handle) {
//		rv = -1;
//		goto map_view_created;
//	}
//
//	HHOOK wndproc_hook_handle = set_hook(WH_CALLWNDPROC);
//	if (!wndproc_hook_handle) {
//		rv = -1;
//		goto hooks_set;
//	}
//
//	wchar_t proc_buffer[MAX_PATH];
//	char key_buffer[32];
//	DWORD proc_id;
//
//	printf("        === HotKey Detective v0.1 ===\n\n");
//	printf("Waiting for hot-keys, press CTRL+C to exit...\n\n");
//
//	while (GetMessageW(&msg, NULL, 0, 0) != 0) {
//		if (msg.message == WM_NULL) {
//			GetWindowThreadProcessId((HWND)msg.wParam, &proc_id);
//
//			keystroke_to_str(msg.lParam, key_buffer);
//
//			if (get_proc_path(proc_id, proc_buffer)) {
//				display_info(key_buffer, proc_buffer);
//			} else {
//				display_info(key_buffer, L"Unknown process");
//			}
//		}
//	}
//
//hooks_set:
//	UnhookWindowsHookEx(wndproc_hook_handle);
//	UnhookWindowsHookEx(getmessage_hook_handle);
//map_view_created:
//	UnmapViewOfFile(thread_id);
//map_file_created:
//	CloseHandle(map_file);
//exit:
//	if (rv != 0) {
//		system("pause");
//	}
//
//	return rv;
//}
