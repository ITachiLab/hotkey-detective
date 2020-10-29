/*!
 * \file main.c
 * \brief A main file of the Hotkey Detective.
 * \author Krzysztof "Itachi" Bąk
 */

#include <stdio.h>
#include "core.h"
#include "dllmain.h"

DWORD *thread_id;

int main() {
	int rv = 0;
	MSG msg;

	HANDLE map_file = create_map_file();
	if (!map_file) {
		rv = -1;
		goto exit;
	}

	thread_id = get_memory_view(map_file);
	if (!thread_id) {
		rv = -1;
		goto map_file_created;
	}

	*thread_id = GetCurrentThreadId();

	// Force pre-creation of the message queue.
	PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE);

	SetConsoleCtrlHandler(console_handler, TRUE);

	HHOOK getmessage_hook_handle = set_hook(WH_GETMESSAGE);
	if (!getmessage_hook_handle) {
		rv = -1;
		goto map_view_created;
	}

	HHOOK wndproc_hook_handle = set_hook(WH_CALLWNDPROC);
	if (!wndproc_hook_handle) {
		rv = -1;
		goto hooks_set;
	}

	wchar_t proc_buffer[MAX_PATH];
	char key_buffer[32];
	DWORD proc_id;

	printf("        === HotKey Detective v0.1 ===\n\n");
	printf("Waiting for hot-keys, press CTRL+C to exit...\n\n");

	while (GetMessageW(&msg, NULL, 0, 0) != 0) {
		if (msg.message == WM_NULL) {
			GetWindowThreadProcessId((HWND)msg.wParam, &proc_id);

			keystroke_to_str(msg.lParam, key_buffer);

			if (get_proc_path(proc_id, proc_buffer)) {
				display_info(key_buffer, proc_buffer);
			} else {
				display_info(key_buffer, L"Unknown process");
			}
		}
	}

hooks_set:
	UnhookWindowsHookEx(wndproc_hook_handle);
	UnhookWindowsHookEx(getmessage_hook_handle);
map_view_created:
	UnmapViewOfFile(thread_id);
map_file_created:
	CloseHandle(map_file);
exit:
	if (rv != 0) {
		system("pause");
	}

	return rv;
}
