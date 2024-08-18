#include <windows.h>

#include <exception>

#include "TesterWindow.h"

/**
 * Main function of the window.
 *
 * @param hInstance[in]     a handle to the current instance of the application
 * @param hPrevInstance[in] a handle to the previous instance of the application
 * @param lpCmdLine[in]     the command line for the application
 * @param nShowCmd[in]      specify how an application is to be displayed when
 *                          it is opened
 * @return If the function succeeds, terminating when it receives a WM_QUIT
 * message, it should return the exit value contained in that message's wParam
 * parameter. If the function terminates before entering the message loop, it
 * should return zero.
 */
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    PWSTR lpCmdLine, int nShowCmd) {
  const auto window = TesterWindow(hInstance);
  return static_cast<int>(window.show());
}