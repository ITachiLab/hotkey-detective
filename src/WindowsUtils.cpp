#include "WindowsUtils.h"

#include <securitybaseapi.h>

BOOL WindowsUtils::isUserAdmin() {
    SID_IDENTIFIER_AUTHORITY authority = SECURITY_NT_AUTHORITY;
    PSID administratorsGroup;

    BOOL result = AllocateAndInitializeSid(&authority, 2,
                                           SECURITY_BUILTIN_DOMAIN_RID,
                                           DOMAIN_ALIAS_RID_ADMINS,
                                           0, 0, 0, 0, 0, 0,
                                           &administratorsGroup);

    if (result) {
        // If this call fails, that means an error, so we can as well assume
        // the user has no admin rights.
        if (!CheckTokenMembership(nullptr, administratorsGroup, &result)) {
            result = FALSE;
        }

        FreeSid(administratorsGroup);
    }

    return result;
}

const wchar_t *WindowsUtils::resStr(int strResource) {
    HINSTANCE hInstance = GetModuleHandleW(nullptr);

    const wchar_t* strPtr = nullptr;
    LoadStringW(hInstance, strResource, (LPWSTR) &strPtr, 0);

    return strPtr;
}