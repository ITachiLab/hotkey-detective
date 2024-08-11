#ifndef HOTKEY_DETECTIVE_INCLUDE_CLOSEDIALOG_HPP_
#define HOTKEY_DETECTIVE_INCLUDE_CLOSEDIALOG_HPP_

#include <Core.h>
#include <commctrl.h>
#include <resource.h>

namespace CloseDialog {
static constexpr unsigned POLL_INTERVAL_MS = 500;
static constexpr unsigned PROGRESS_STEP = 1;
static constexpr unsigned PROGRESS_MIN = 0;
static constexpr unsigned PROGRESS_MAX = 10;

/*!
 * \struct ProcData
 * \brief This is a structure with data shared between the main window and the
 *        modal dialog that shows shutting down progress.
 */
struct ProcData {
  Core *core;            /*!< A pointer to the Core class */
  unsigned timerCounter; /*!< A counter of timer firing events */
};

/*!
 * \brief A dialog procedure of the pre-shutdown dialog.
 *
 * @param hwnd   a handle to the dialog windows
 * @param msg    a message sent to the dialog
 * @param wParam unused WPARAM
 * @param lParam for WM_INITDIALOG messages this parameter contains a pointer to
 *               ProcData object
 * @return The result of processing the message.
 */
static INT_PTR dialogProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
  const HWND progressBar = GetDlgItem(hwnd, IDC_CLOSE_PROGRESS);

  if (msg == WM_INITDIALOG) {
    SetWindowLongPtr(hwnd, GWLP_USERDATA, lParam);

    SendMessage(
        progressBar, PBM_SETRANGE, 0, MAKELPARAM(PROGRESS_MIN, PROGRESS_MAX));
    SendMessage(progressBar, PBM_SETSTEP, 1, PROGRESS_MIN);
    SetTimer(hwnd, IDT_CLOSE_TIMER, POLL_INTERVAL_MS, nullptr);

    return true;
  }

  if (msg == WM_TIMER) {
    const auto procData =
        reinterpret_cast<ProcData *>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    SendMessage(progressBar, PBM_STEPIT, 0, 0);

    if (++procData->timerCounter == PROGRESS_MAX ||
        procData->core->getInjectCount() == 0) {
      KillTimer(hwnd, IDT_CLOSE_TIMER);
      EndDialog(hwnd, 0);
    }
  }

  return false;
}
}  // namespace CloseDialog
#endif