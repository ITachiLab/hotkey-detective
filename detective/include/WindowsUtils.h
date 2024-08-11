#ifndef HOTKEY_DETECTIVE_INCLUDE_WINDOWSUTILS_H_
#define HOTKEY_DETECTIVE_INCLUDE_WINDOWSUTILS_H_

#include <windows.h>

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
 * \brief Return a read-only pointer to a string resource.
 *
 * This function can be used to retrieve a read-only pointer to any string
 * resource defined in this application.
 *
 * @param[in] strResource an ID of the string resource
 * @return If the function succeeds, the return value is a read-only pointer to
 * the desired string resource. Otherwise, the return value is a nullptr.
 */
const wchar_t *resStr(int strResource);
}

#endif //HOTKEY_DETECTIVE_INCLUDE_WINDOWSUTILS_H_
