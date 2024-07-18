#include "KeySequence.h"

bool Key::isModifier(unsigned int virtualKeyCode) {
  // No need to check for VK_L* and VK_R* because they are used only by
  // GeyKeyState() and GetAsyncKeyState() functions.
  return virtualKeyCode == VK_SHIFT || virtualKeyCode == VK_CONTROL ||
         virtualKeyCode == VK_MENU;
}

Key Key::fromWindowMessage(LPARAM lParam) {
  wchar_t buffer[nameMaxLen] = {};

  const auto strokeData = reinterpret_cast<KeyStrokeData*>(&lParam);
  const long keyCode = strokeData->scanCode << 16 | strokeData->extendedKey
                                                        << 24;

  // ORing with (1 << 25) to make GetKeyNameText don't care about whether this
  // is right or left modifier key.
  GetKeyNameText(keyCode | 1 << 25, buffer, nameMaxLen);

  return {
    buffer, MapVirtualKey(strokeData->scanCode, MAPVK_VSC_TO_VK),
        !strokeData->released
  };
}

Key Key::fromVirtualKeyCode(const unsigned virtualKeyCode) {
  wchar_t buffer[nameMaxLen] = {};

  const long scanCode = MapVirtualKey(virtualKeyCode, MAPVK_VK_TO_VSC);
  GetKeyNameText(scanCode << 16, buffer, nameMaxLen);

  return {buffer, virtualKeyCode, true};
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

KeySequence KeySequence::fromGlobalHotKey(LPARAM lParam) {
  KeySequence sequence;
  const auto hotKeyData = reinterpret_cast<GlobalHotKeyData*>(&lParam);

  if (hotKeyData->altDown) {
    sequence.modifiers.insert(Key::fromVirtualKeyCode(VK_MENU));
  }

  if (hotKeyData->ctrlDown) {
    sequence.modifiers.insert(Key::fromVirtualKeyCode(VK_CONTROL));
  }

  if (hotKeyData->shiftDown) {
    sequence.modifiers.insert(Key::fromVirtualKeyCode(VK_SHIFT));
  }

  if (hotKeyData->winDown) {
    sequence.modifiers.insert(Key::fromVirtualKeyCode(VK_LWIN));
  }

  sequence.normalKey = Key::fromVirtualKeyCode(hotKeyData->virtualKeyCode);
  return sequence;
}

void KeySequence::clear() {
  modifiers.clear();
  normalKeyPressed = false;
}
