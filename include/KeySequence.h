#ifndef SEQUENCEDETECTOR_H
#define SEQUENCEDETECTOR_H

#include <windows.h>

#include <set>
#include <string>

#pragma pack(1)
/*!
 * \brief This structure represents data passed to the program from the
 *        WH_KEYBOARD hook.
 *
 * It's treated like a classical, C-style struct to which binary data can be
 * casted directly. In order to do so, the structure needs to be packed.
 */
struct WhKeyboardData final {
  uint16_t repeatCount;
  uint8_t scanCode;
  bool extendedKey : 1;
  uint8_t reserved : 4;
  bool altDown : 1;
  bool previousState : 1;
  bool released : 1;
};

/*!
 * \brief A representation of a key press with a human-friendly name.
 *
 * Each time a key is pressed it's going to be represented by instances of this
 * class. It contains all information needed to identify and display details of
 * individual key presses.
 */
class Key final {
  static constexpr size_t nameMaxLen = 32;

  std::wstring name;
  unsigned int virtualKeyCode = 0;
  bool pressed = false;
  bool modifier = false;

 public:
  /*!
   * \brief Create a new Key instance from data received by KeyboardProc, as a
   *        result of WH_KEYBOARD hook.
   *
   * @param lParam the lParam parameter received in a KeyboardProc
   * @return A new Key object created from details contained in lParam.
   */
  static Key fromWhKeyboard(LPARAM lParam);

  /*!
   * \brief Check if the virtual key code represents a modifier (ALT, SHIFT,
   *        CTRL).
   *
   * @param virtualKeyCode the virtual key code to check
   * @return "true" when the virtual key code belongs to ALT, SHIFT or CTRL
   *         keys, "false" when it's a normal key.
   */
  static bool isModifier(unsigned int virtualKeyCode);

  /*!
   * \brief Check if this key was pressed or released.
   *
   * @return "true" if this key was pressed, "false" when it's been released.
   */
  [[nodiscard]] bool isPressed() const { return pressed; }

  /*!
   * \brief Check if this key is a modifier.
   *
   * @return "true" if this key is a modifier (ALT, SHIFT or CTRL).
   */
  [[nodiscard]] bool isModifier() const { return modifier; }

  /*!
   * \brief Get a human-friendly name of the keystroke.
   *
   * @return A string with a human-friendly name of the keystroke.
   */
  [[nodiscard]] const std::wstring& getName() const { return name; }

  /*!
   * \brief Compare two Key instances by comparing their virtual key codes.
   *
   * This method is mandatory to let Key instances be part of std::set.
   *
   * @param other other Key instance to compare
   * @return "true" if this and other instances are equal in a sense that their
   *         virtual key codes are equal; "false" otherwise.
   */
  bool operator==(const Key& other) const {
    return virtualKeyCode == other.virtualKeyCode;
  }

  /*!
   * \brief Compare inequality of two Key instances by comparing their virtual
   *        key codes.
   *
   * @param other other Key instance to compare
   * @return "true" if this and other instances are equal in a sense that their
   *         virtual key codes are equal; "false" otherwise.
   */
  bool operator!=(const Key& other) const {
    return virtualKeyCode != other.virtualKeyCode;
  }

  /*!
   * \brief Compare two Key instances and determine if virtual key code of this
   *        instance is less than other's.
   *
   * This method is mandatory to let Key instances be part of std::set.
   *
   * @param other other Key instance to compare
   * @return "true" if this virtual key code is less than other's virtual key
   *         code.
   */
  bool operator<(const Key& other) const {
    return virtualKeyCode < other.virtualKeyCode;
  }
};

/*!
 * \brief A container keeping sequences of keystrokes.
 */
class KeySequence final {
  const Key emptyKey;

  std::set<Key> modifiers;
  Key normalKey = emptyKey;

 public:
  /*!
   * \brief Add a new Key stroke to the sequence.
   *
   * This method behaves differently depending whether the Key is a modifier or
   * a normal key.
   *
   * For modifiers, when the Key represents a "press" action, the
   * Key will be added to the set of modifiers, from which it's going to be
   * removed on a subsequent "release" action. For normal keys, the method
   * simply stores the most recent key.
   *
   * @param key the Key to add to the sequnce
   */
  void addKeyStroke(Key& key);

  /*!
   * \brief Check if this KeySequence represents a combination of at least one
   *        modifier with a normal key.
   *
   * @return "true" if this KeySequence represents a combination of at least one
   *         modifier with a normal key.
   */
  [[nodiscard]] bool isCombination() const;
};

#endif  // SEQUENCEDETECTOR_H
