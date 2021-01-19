/*!
 * \file    HotkeyTable.cpp
 * \brief   Implementation of the main table.
 * \author  Itachi
 * \date    2020-12-01
 */

#include "HotkeyTable.h"

#include "resource.h"

#include <exception>
#include <commctrl.h>

WCHAR *COLUMN_TITLES[] = {L"Hotkey", L"Process path"};
const int COLUMN_WIDTHS[] = {100, 200};

void HotkeyTable::addEntry(char *hotkey, wchar_t *processPath) {
    TableEntry entry = {};
    MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, hotkey, strlen(hotkey),
                        entry.hotkey, HOTKEY_LENGTH);
    wcscpy_s(entry.processPath, MAX_PATH, processPath);

    LVITEM lvi;

    lvi.pszText = entry.hotkey;
    lvi.mask = LVIF_TEXT | LVIF_STATE;
    lvi.stateMask = 0;
    lvi.iSubItem = 0;
    lvi.state = 0;
    lvi.iItem = entries.size();

    ListView_InsertItem(tableHwnd, &lvi);

    entries.push_back(entry);
}

void HotkeyTable::handleWmNotify(LPARAM lParam) {
    NMLVDISPINFOW* displayInfo;

    switch (((LPNMHDR) lParam)->code) {
        case LVN_GETDISPINFOW:
            displayInfo = (NMLVDISPINFOW*)lParam;

            switch (displayInfo->item.iSubItem) {
                case 0:
                    displayInfo->item.pszText =
                            entries[displayInfo->item.iItem].hotkey;
                    break;
                case 1:
                    displayInfo->item.pszText =
                            entries[displayInfo->item.iItem].processPath;
                    break;
                default:
                    break;
            }
    }
}

void HotkeyTable::addToWindow(HWND parentWindow, HINSTANCE hInstance) {
    RECT rcClient;
    GetClientRect(parentWindow, &rcClient);

    tableHwnd = CreateWindowExW(0L, WC_LISTVIEWW, L"",
                                WS_VISIBLE | WS_CHILD | LVS_REPORT,
                                0, 0,
                                rcClient.right - rcClient.left,
                                rcClient.bottom - rcClient.top,
                                parentWindow,
                                (HMENU) ID_TABLE,
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
