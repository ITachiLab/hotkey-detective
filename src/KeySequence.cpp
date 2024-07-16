#include "KeySequence.h"

bool Key::isModifier(unsigned int virtualKeyCode) {
  // No need to check for VK_L* and VK_R* because they are used only by
  // GeyKeyState() and GetAsyncKeyState() functions.
  return virtualKeyCode == VK_SHIFT || virtualKeyCode == VK_CONTROL ||
         virtualKeyCode == VK_MENU;
}

Key Key::fromWhKeyboard(LPARAM lParam) {
  Key key;
  key.name.reserve(nameMaxLen);

  const auto keyboardData = reinterpret_cast<WhKeyboardData*>(&lParam);
  const long keyCode = keyboardData->scanCode << 16 | keyboardData->extendedKey
                                                          << 24;

  GetKeyNameText(keyCode, LPWSTR(key.name.c_str()), nameMaxLen);

  key.virtualKeyCode = MapVirtualKey(keyboardData->scanCode, MAPVK_VSC_TO_VK);
  key.pressed = !keyboardData->released;
  key.modifier = isModifier(key.virtualKeyCode);

  return key;
}

void KeySequence::addKeyStroke(Key& key) {
  if (key.isModifier()) {
    if (key.isPressed()) {
      modifiers.insert(std::move(key));
    } else {
      modifiers.erase(key);
    }
  } else {
    if (key.isPressed()) {
      normalKey = std::move(key);
    } else {
      normalKey = emptyKey;
    }
  }
}

bool KeySequence::isCombination() const {
  return !modifiers.empty() && normalKey != emptyKey;
}
