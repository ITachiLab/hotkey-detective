/*!
 * \file    main.h
 * \brief   A header of the main.cpp file.
 * \author  Itachi
 * \date    2020-12-01
 */

#ifndef HOTKEY_DETECTIVE_INCLUDE_MAIN_H_
#define HOTKEY_DETECTIVE_INCLUDE_MAIN_H_

#include <windows.h>

/*!
 * \brief Just a WinMain function.
 *
 * @param hinstance[in]     a handle to the current instance of the application
 * @param hPrevInstance[in] a handle to the previous instance of the application
 * @param cmdLine[in]       the command line for the application
 * @param cmdShow[in]       specify how an application is to be displayed when
 *                          it is opened
 * @return If the function succeeds, terminating when it receives a WM_QUIT
 * message, it should return the exit value contained in that message's wParam
 * parameter. If the function terminates before entering the message loop, it
 * should return zero.
 */
int WINAPI wWinMain(HINSTANCE hinstance, HINSTANCE hPrevInstance,
                    PWSTR cmdLine, int cmdShow);

#endif //HOTKEY_DETECTIVE_INCLUDE_MAIN_H_
