/*!
 * \file    HotkeyTable.cpp
 * \brief   Implementation of the main table.
 * \author  Itachi
 * \date    2020-12-01
 */

#include "HotkeyTable.h"

#include <KeySequence.h>
#include <commctrl.h>

#include <exception>
#include <utility>

#include "resource.h"

WCHAR* COLUMN_TITLES[] = {L"Hotkey", L"Process path"};
const int COLUMN_WIDTHS[] = {150, 400};

void HotkeyTable::addEntry(std::wstring keySequence, std::wstring processPath) {
  TableEntry entry(std::move(keySequence), std::move(processPath));

  LVITEM lvi;
  lvi.pszText = LPWSTR(entry.hotkey.c_str());
  lvi.mask = LVIF_TEXT | LVIF_STATE;
  lvi.stateMask = 0;
  lvi.iSubItem = 0;
  lvi.state = 0;
  lvi.iItem = entries.size();

  ListView_InsertItem(tableHwnd, &lvi);

  entries.push_back(std::move(entry));
}

void HotkeyTable::handleWmNotify(LPARAM lParam) {
  NMLVDISPINFOW* displayInfo;

  switch (((LPNMHDR)lParam)->code) {
    case LVN_GETDISPINFOW:
      displayInfo = (NMLVDISPINFOW*)lParam;

      switch (displayInfo->item.iSubItem) {
        case 0:
          displayInfo->item.pszText =
              LPWSTR(entries[displayInfo->item.iItem].hotkey.c_str());
          break;
        case 1:
          displayInfo->item.pszText =
              LPWSTR(entries[displayInfo->item.iItem].processPath.c_str());
          break;
        default:
          break;
      }
  }
}

void HotkeyTable::addToWindow(HWND parentWindow, HINSTANCE hInstance) {
  RECT rcClient;
  GetClientRect(parentWindow, &rcClient);

  tableHwnd = CreateWindowExW(0L,
                              WC_LISTVIEWW,
                              L"",
                              WS_VISIBLE | WS_CHILD | LVS_REPORT,
                              0,
                              0,
                              rcClient.right - rcClient.left,
                              rcClient.bottom - rcClient.top,
                              parentWindow,
                              (HMENU)IDC_MAIN_TABLE,
                              hInstance,
                              NULL);

  LVCOLUMNW lvc = {};

  for (int i = 0; i < TABLE_COLUMNS; i++) {
    lvc.iSubItem = i;
    lvc.pszText = COLUMN_TITLES[i];
    lvc.cx = COLUMN_WIDTHS[i];
    lvc.mask = LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;

    ListView_InsertColumn(tableHwnd, i, &lvc);
  }

  ListView_SetExtendedListViewStyle(tableHwnd,
                                    LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
}
