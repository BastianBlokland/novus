#pragma once
#if defined(_WIN32)
#include "err.hpp"
#include "regkey.hpp"
#include "regval.hpp"
#include <optional>
#include <string>

namespace novrt::win32 {

enum class RegKeyStore {
  User,
  Machine,
};

enum class RegKeyAccessMode {
  Read,
  Write,
  Manage,
};

enum class RegKeyCreateMode {
  Open,
  Create,
};

auto getRegKey(
    std::string_view keyName,
    RegKeyStore store,
    RegKeyAccessMode accessMode,
    RegKeyCreateMode createMode) noexcept -> RegKey;

inline auto getUsrRegKey(std::string_view keyName) noexcept -> RegKey {
  return getRegKey(keyName, RegKeyStore::User, RegKeyAccessMode::Manage, RegKeyCreateMode::Create);
}

inline auto usrRegKeyExists(std::string_view keyName) noexcept -> bool {
  return getRegKey(keyName, RegKeyStore::User, RegKeyAccessMode::Read, RegKeyCreateMode::Open);
}

inline auto getSysRegKey(std::string_view keyName) noexcept -> RegKey {
  return getRegKey(keyName, RegKeyStore::Machine, RegKeyAccessMode::Read, RegKeyCreateMode::Create);
}

inline auto sysRegKeyExists(std::string_view keyName) noexcept -> bool {
  return getRegKey(keyName, RegKeyStore::Machine, RegKeyAccessMode::Read, RegKeyCreateMode::Open);
}

auto deleteRegKey(std::string_view keyName, RegKeyStore store) noexcept -> OptWinErr;

inline auto deleteUsrRegKey(std::string_view keyName) noexcept -> OptWinErr {
  return deleteRegKey(keyName, RegKeyStore::User);
}

auto regStrValExists(const RegKey& key, std::string_view valName = "") noexcept -> bool;

auto getRegStrVal(const RegKey& key, std::string_view valName = "") noexcept -> RegStrVal;

auto setRegStrVal(const RegKey& key, std::string_view value, std::string_view valName = "") noexcept
    -> OptWinErr;

} // namespace novrt::win32

#endif // _Win32
