//
// Created by Itachi on 2020-12-01.
//

#ifndef HOTKEY_DETECTIVE__HOTKEYTABLE_H_
#define HOTKEY_DETECTIVE__HOTKEYTABLE_H_

#include <windows.h>
#include <vector>

struct TableEntry {
  wchar_t hotkey[32];
  wchar_t processPath[MAX_PATH];
};

class HotkeyTable {
 private:
  HWND tableHwnd;
  int idHeader;
  std::vector<TableEntry> items;

  void addColumn(int id, WCHAR *columnTitle, int width);
 protected:
 public:
  HotkeyTable(HWND parentWindow, HINSTANCE hInstance);
  void addItem(char *hotkey, wchar_t *processPath);
  void handleWmNotify(LPARAM lParam);
};

#endif //HOTKEY_DETECTIVE__HOTKEYTABLE_H_
