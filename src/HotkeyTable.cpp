//
// Created by Itachi on 2020-12-01.
//

#include "HotkeyTable.h"
#include "resource.h"

#include <exception>
#include <commctrl.h>

const int COLUMNS = 2;
WCHAR *COLUMN_TITLES[] = {L"Hotkey", L"Process path"};
const int COLUMN_WIDTHS[] = {100, 200};

HotkeyTable::HotkeyTable(HWND parentWindow, HINSTANCE hInstance) {
    INITCOMMONCONTROLSEX icex = {};
    icex.dwICC = ICC_LISTVIEW_CLASSES;

    InitCommonControlsEx(&icex);

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

    for (int i = 0; i < COLUMNS; i++) {
        addColumn(i, COLUMN_TITLES[i], COLUMN_WIDTHS[i]);
    }

    ListView_SetExtendedListViewStyle(tableHwnd, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
}

void HotkeyTable::addColumn(int id, WCHAR *columnTitle, int width) {
    LVCOLUMNW lvc;
    lvc.iSubItem = id;
    lvc.pszText = columnTitle;
    lvc.cx = width;
    lvc.mask = LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;

    ListView_InsertColumn(tableHwnd, id, &lvc);
}

void HotkeyTable::addItem(char *hotkey, wchar_t *processPath) {
    TableEntry entry = {};
    MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, hotkey, strlen(hotkey),
                        entry.hotkey, 32);
    wcscpy_s(entry.processPath, sizeof(entry.processPath), processPath);

    LVITEM lvi;

    lvi.pszText = entry.hotkey;
    lvi.mask = LVIF_TEXT | LVIF_STATE;
    lvi.stateMask = 0;
    lvi.iSubItem = 0;
    lvi.state = LVIS_CUT;
    lvi.iItem = items.size();

    ListView_InsertItem(tableHwnd, &lvi);

    items.push_back(entry);
}

void HotkeyTable::handleWmNotify(LPARAM lParam) {
    NMLVDISPINFOW* displayInfo;

    switch (((LPNMHDR) lParam)->code) {
        case LVN_GETDISPINFOW:
            displayInfo = (NMLVDISPINFOW*)lParam;

            switch (displayInfo->item.iSubItem) {
                case 0:
                    displayInfo->item.pszText = items[displayInfo->item.iItem].hotkey;
                    break;
                case 1:
                    displayInfo->item.pszText = items[displayInfo->item.iItem].processPath;
                    break;
                default:
                    break;
            }

            displayInfo->item.mask = LVIF_TEXT | LVIF_STATE;
            displayInfo->item.state = LVIS_CUT;
    }
}
