#include "WindowsUtils.h"

#include <securitybaseapi.h>

#include <stdexcept>

#include "debug.h"
#include "resource.h"

BOOL WindowsUtils::isUserAdmin() {
  SID_IDENTIFIER_AUTHORITY authority = SECURITY_NT_AUTHORITY;
  PSID administratorsGroup;

  BOOL result = AllocateAndInitializeSid(&authority,
                                         2,
                                         SECURITY_BUILTIN_DOMAIN_RID,
                                         DOMAIN_ALIAS_RID_ADMINS,
                                         0,
                                         0,
                                         0,
                                         0,
                                         0,
                                         0,
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

std::wstring WindowsUtils::resStr(int strResource, HMODULE moduleHandle) {
  if (!moduleHandle) {
    moduleHandle = GetModuleHandleW(nullptr);
  }
  const wchar_t* strPtr = nullptr;
  const int size = LoadStringW(moduleHandle, strResource, (LPWSTR)&strPtr, 0);
  if (size == 0) {
    debugPrint("String resource with ID %d does not exist, error code: %d",
               strResource,
               GetLastError());
    return std::wstring{};
  }
  return std::wstring(strPtr, size);
}

std::unique_ptr<std::remove_pointer_t<HMODULE>, decltype(&FreeLibrary)>
WindowsUtils::loadModule(std::wstring_view libraryPath, DWORD flags) {
  HMODULE resContainer = LoadLibraryExW(libraryPath.data(), NULL, flags);
  if (!resContainer) {
    constexpr int size = 512;
    char displayBuffer[size];
    sprintf_s(displayBuffer,
              size,
              "FAILURE: Unable to load module (%ls), last error = %d.",
              libraryPath.data(),
              GetLastError());
    throw std::runtime_error(displayBuffer);
  }
  return {resContainer, FreeLibrary};
}

std::wstring WindowsUtils::toWString(std::string_view string) {
  const char* strPtr{string.data()};
  std::mbstate_t state = std::mbstate_t{};
  std::size_t len = 1 + std::mbsrtowcs(nullptr, &strPtr, 0, &state);
  std::wstring msg(len, L'#');
  std::mbsrtowcs(msg.data(), &strPtr, len, &state);
  return msg;
}
