#if defined(_WIN32)
#include "reg.hpp"
#include "Windows.h"
#include <optional>

namespace novrt::win32 {

namespace {

[[noreturn]] auto errorExit(const char* msg) noexcept {
  std::fprintf(stderr, "%s\n", msg);
  std::fflush(stderr);
  std::exit(EXIT_FAILURE);
}

auto accessFromMode(RegKeyAccessMode mode) noexcept -> REGSAM {
  switch (mode) {
  case RegKeyAccessMode::Read:
    return KEY_READ;
  case RegKeyAccessMode::Write:
    return KEY_WRITE;
  case RegKeyAccessMode::Manage:
    return KEY_ALL_ACCESS;
  }
  errorExit("Unsupported key mode");
}

auto rootKeyFromStore(RegKeyStore store) noexcept -> HKEY {
  switch (store) {
  case RegKeyStore::User:
    return HKEY_CURRENT_USER;
  case RegKeyStore::Machine:
    return HKEY_LOCAL_MACHINE;
  }
  errorExit("Unsupported key store");
}

} // namespace

auto getRegKey(
    std::string_view keyName,
    RegKeyStore store,
    RegKeyAccessMode accessMode,
    RegKeyCreateMode createMode) noexcept -> RegKey {
  auto keyNameCopy            = std::string{keyName}; // Copy the keyName to null-term it.
  const char* keyNameNullTerm = keyNameCopy.c_str();

  HKEY rootKey  = rootKeyFromStore(store);
  REGSAM access = accessFromMode(accessMode);

  HKEY key          = nullptr;
  DWORD disposition = REG_OPENED_EXISTING_KEY;
  WinErr err;
  if (createMode == RegKeyCreateMode::Create) {
    err = ::RegCreateKeyExA(
        rootKey, keyNameNullTerm, 0, nullptr, 0, access, nullptr, &key, &disposition);
  } else {
    err = ::RegOpenKeyExA(rootKey, keyNameNullTerm, 0, access, &key);
  }

  if (err != ERROR_SUCCESS) {
    return RegKey(err);
  }
  return RegKey(
      key,
      disposition == REG_OPENED_EXISTING_KEY ? RegKey::Disposition::Existing
                                             : RegKey::Disposition::New);
}

auto deleteRegKey(std::string_view keyName, RegKeyStore store) noexcept -> OptWinErr {
  auto keyNameCopy            = std::string{keyName}; // Copy the keyName to null-term it.
  const char* keyNameNullTerm = keyNameCopy.c_str();
  const HKEY rootKey          = rootKeyFromStore(store);

  const WinErr err = ::RegDeleteKeyA(rootKey, keyNameNullTerm);
  if (err != ERROR_SUCCESS) {
    return err;
  }
  return winSuccess();
}

auto regStrValExists(const RegKey& key, std::string_view valName) noexcept -> bool {
  if (!key) {
    errorExit("Invalid key");
  }

  auto valNameCopy            = std::string{valName}; // Copy the keyName to null-term it.
  const char* valNameNullTerm = valNameCopy.c_str();

  const WinErr err =
      ::RegGetValueA(*key, nullptr, valNameNullTerm, RRF_RT_REG_SZ, nullptr, nullptr, nullptr);
  return err == ERROR_SUCCESS;
}

auto getRegStrVal(const RegKey& key, std::string_view valName) noexcept -> RegStrVal {
  if (!key) {
    errorExit("Invalid key");
  }

  auto valNameCopy            = std::string{valName}; // Copy the keyName to null-term it.
  const char* valNameNullTerm = valNameCopy.c_str();

  auto result      = std::string{};
  DWORD resultSize = 128;
  WinErr err;
  do {
    result.resize(resultSize);
    err = ::RegGetValueA(
        *key, nullptr, valNameNullTerm, RRF_RT_REG_SZ, nullptr, result.data(), &resultSize);
  } while (err == ERROR_MORE_DATA);

  if (err != ERROR_SUCCESS) {
    return RegStrVal{err};
  }
  result.resize(resultSize - 1);
  return RegStrVal{std::move(result)};
}

auto setRegStrVal(const RegKey& key, std::string_view value, std::string_view valName) noexcept
    -> OptWinErr {
  if (!key) {
    errorExit("Invalid key");
  }

  const auto valCopy      = std::string{value};
  const char* valNullTerm = valCopy.c_str();

  auto valNameCopy            = std::string{valName}; // Copy the keyName to null-term it.
  const char* valNameNullTerm = valNameCopy.c_str();

  const WinErr err = ::RegSetValueExA(
      *key,
      valNameNullTerm,
      0,
      REG_SZ,
      reinterpret_cast<const BYTE*>(valNullTerm),
      ::strlen(valNullTerm) + 1); // NOTE: Using strlen in case of any null-terms in the middle.

  if (err != ERROR_SUCCESS) {
    return err;
  }
  return winSuccess();
}

} // namespace novrt::win32

#endif // _Win32
