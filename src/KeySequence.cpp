#include "KeySequence.h"

bool Key::isModifier(unsigned int virtualKeyCode) {
  // No need to check for VK_L* and VK_R* because they are used only by
  // GeyKeyState() and GetAsyncKeyState() functions.
  return virtualKeyCode == VK_SHIFT || virtualKeyCode == VK_CONTROL ||
         virtualKeyCode == VK_MENU;
}

Key Key::fromWindowMessage(LPARAM lParam) {
  Key key;
  wchar_t buffer[nameMaxLen] = {};

  const auto strokeData = reinterpret_cast<KeyStrokeData*>(&lParam);
  const long keyCode = strokeData->scanCode << 16 | strokeData->extendedKey
                                                        << 24;

  // ORing with (1 << 25) to make GetKeyNameText don't care about whether this
  // is right or left modifier key.
  GetKeyNameText(keyCode | 1 << 25, buffer, nameMaxLen);

  key.virtualKeyCode = MapVirtualKey(strokeData->scanCode, MAPVK_VSC_TO_VK);
  key.pressed = !strokeData->released;
  key.modifier = isModifier(key.virtualKeyCode);
  key.name = std::wstring(buffer);

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
    }
    normalKeyPressed = key.isPressed();
  }
}

bool KeySequence::isCombination() const {
  return !modifiers.empty() && normalKeyPressed;
}

std::wstring KeySequence::getCombinationString() const {
  std::wstring output;

  for (const auto& key : modifiers) {
    output += key.getName() + L" + ";
  }
  output += normalKey.getName();

  return output;
}

void KeySequence::clear() {
  modifiers.clear();
  normalKeyPressed = false;
}
