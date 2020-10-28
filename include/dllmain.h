#ifndef HOTKEY_DETECTIVE_INCLUDE_DLLMAIN_H_
#define HOTKEY_DETECTIVE_INCLUDE_DLLMAIN_H_

#include <windows.h>

__declspec(dllexport)
LRESULT CALLBACK getmessage_hook(int n_code, WPARAM w_param, LPARAM l_param);

__declspec(dllexport)
LRESULT CALLBACK wndproc_hook(int n_code, WPARAM w_param, LPARAM l_param);

#endif //HOTKEY_DETECTIVE_INCLUDE_DLLMAIN_H_