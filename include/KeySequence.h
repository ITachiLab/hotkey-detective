#ifndef SEQUENCEDETECTOR_H
#define SEQUENCEDETECTOR_H

#include <windows.h>

#include <set>
#include <string>

#pragma pack(push, 1)
/*!
 * \brief This structure represents keystroke data. This is how WinApi usually
 *        transfers details of keystrokes between functions.
 */
struct KeyStrokeData final {
  uint16_t repeatCount;
  uint8_t scanCode;
  bool extendedKey : 1;
  uint8_t reserved : 4;
  bool altDown : 1;
  bool previousState : 1;
  bool released : 1;
};

/*!
 * \brief This structure represents key combination data received with WM_HOTKEY
 *        message.
 */
struct GlobalHotKeyData final {
  uint32_t altDown : 1;
  uint32_t ctrlDown : 1;
  uint32_t shiftDown : 1;
  uint32_t winDown : 1;
  uint32_t empty: 12;
  uint32_t virtualKeyCode: 16;
};
#pragma pack(pop)

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
  unsigned virtualKeyCode = 0;
  bool pressed = false;
  bool modifier = false;

  Key(const wchar_t *name, const unsigned virtualKeyCode, const bool pressed)
    : name(std::wstring(name)),
      virtualKeyCode(virtualKeyCode),
      pressed(pressed),
      modifier(isModifier(virtualKeyCode)) {}

public:
  explicit Key() = default;

  /*!
   * \brief Create a new Key instance from data received by a window procedure.
   *
   * This method can also be used for other procedures carrying keystroke data,
   * for example: KeyboardProc. WinApi is fairly consistent when it comes to
   * how the keystrokes are transferred, usually it's the same structure as
   * KeyStrokeData.
   *
   * @param lParam the lParam parameter received in the window procedure for
   *               WM_KEYDOWN and WM_KEYUP messages
   * @return A new Key object created from details contained in lParam.
   */
  static Key fromWindowMessage(LPARAM lParam);

  /*!
   * \brief Create a new Key instance for the given virtual key code.
   *
   * The created Key is assumed to be pressed because there is no point in
   * creating new instances for released keys.
   *
   * @param virtualKeyCode the virtual key code of the pressed key
   * @return A new Key instance for the given virtual key code.
   */
  static Key fromVirtualKeyCode(unsigned virtualKeyCode);

  /*!
   * \brief Check if the virtual key code represents a modifier (ALT, SHIFT,
   *        CTRL).
   *
   * @param virtualKeyCode the virtual key code to check
   * @return "true" when the virtual key code belongs to ALT, SHIFT or CTRL
   *         keys, "false" when it's a normal key.
   */
  [[nodiscard]] static bool isModifier(unsigned int virtualKeyCode);

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
  std::set<Key> modifiers;
  Key normalKey;
  bool normalKeyPressed = false;

 public:
  explicit KeySequence() = default;

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
   * \brief Clear the sequence, effectively making it like a pristine instance.
   */
  void clear();

  /*!
   * \brief Check if this KeySequence represents a combination of at least one
   *        modifier with a normal key.
   *
   * @return "true" if this KeySequence represents a combination of at least one
   *         modifier with a normal key.
   */
  [[nodiscard]] bool isCombination() const;

  /*!
   * \brief Get a string describing all keys being a part of this key sequence
   *        delimited with the "+" character.
   *
   *
   *
   * @return A string consisting of concatenation of all modifiers with the
   *         normal key.
   */
  [[nodiscard]] std::wstring getCombinationString() const;

  /*!
   * \brief Create a sequence from data provided to WM_HOTKEY message.
   *
   * WM_HOTKEY messages are shipped with the full key combination encoded on a
   * single LPARAM parameter, so the whole sequence can be initialized at once.
   *
   * @param lParam the LPARAM parameter of the WM_HOTKEY message
   * @return A new KeySequnce initialized from the WM_HOTKEY data. The sequence
   * is always a combination.
   */
  static KeySequence fromGlobalHotKey(LPARAM lParam);
};

#endif  // SEQUENCEDETECTOR_H
