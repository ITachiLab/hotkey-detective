#ifndef HOTKEY_DETECTIVE_INCLUDE_WINDOWSUTILS_H_
#define HOTKEY_DETECTIVE_INCLUDE_WINDOWSUTILS_H_

#include <windows.h>

#include <memory>
#include <string>
#include <string_view>
#include <type_traits>

namespace WindowsUtils {
/*!
 * \brief Check if the running user has administrator privileges.
 *
 * @return If the running user has administrator privileges, the function
 * returns true. Otherwise, when the user doesn't belong to the "Administrators"
 * group, or there are errors while checking the affiliation, the result is
 * false.
 */
BOOL isUserAdmin();

/*!
 * \brief Return a string resource from given module.
 *
 * This function can be used to retrieve a string resource stored in a given
 * module. If moduleHandle is nullptr then the current executable is used.
 *
 * @param[in] strResource an ID of the string resource
 * @return If the function succeeds, the return value is a std::wstring
 * containing the desired string resource. Otherwise, the return value is an
 * empty std::wstring.
 */
[[nodiscard]] std::wstring resStr(int strResource,
                                  HMODULE moduleHandle = nullptr);

/*!
 * \brief Loads the specified module into the address space of the calling process.
 *
 * This function can be used to load executables/DLLs in order to retrieve resources.
 *
 * @param[in] modulePath path to the executable/DLL
 * @param[in] flags controls behavior of LoadLibraryExW syscall
 * @return If the function succeeds it returns handle to the loaded module wrapped in 
 * std::unique_ptr to provide RAII for the module handle. 
 * Otherwise, it throws std::runtime_error
 */
[[nodiscard]] std::unique_ptr<std::remove_pointer_t<HMODULE>,
                              decltype(&FreeLibrary)>
loadModule(std::wstring_view modulePath, DWORD flags);


/*!
 * \brief Converts any of regular strings to wide string representation.
 *
 * This function can be used to convert exception msg to wide string required in MessageBox
 *
 * @param[in] string: will be converted to wide string representation
 * @return Returns string in a wide string representation (std::wstring)
 */
[[nodiscard]] std::wstring toWString(std::string_view string);
}  // namespace WindowsUtils

#endif  // HOTKEY_DETECTIVE_INCLUDE_WINDOWSUTILS_H_
