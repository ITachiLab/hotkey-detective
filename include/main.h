//
// Created by Itachi on 2020-12-01.
//

#ifndef HOTKEY_DETECTIVE_INCLUDE_MAIN_H_
#define HOTKEY_DETECTIVE_INCLUDE_MAIN_H_

#include <windows.h>

int WINAPI wWinMain(HINSTANCE h_instance, HINSTANCE h_prev_instance,
                    PWSTR cmd_line, int cmd_show);

LRESULT CALLBACK window_proc(HWND hwnd, UINT uMsg, WPARAM wParam,
                             LPARAM lParam);

#endif //HOTKEY_DETECTIVE_INCLUDE_MAIN_H_
