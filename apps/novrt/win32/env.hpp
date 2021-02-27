#pragma once
#if defined(_WIN32)
#include "regval.hpp"
#include <string>

namespace novrt::win32 {

enum class EnvVarStore {
  User,
  Machine,
};

[[nodiscard]] auto getEnvVar(std::string_view name, EnvVarStore store) noexcept -> RegStrVal;

[[nodiscard]] auto inline getUsrEnvVar(std::string_view name) noexcept -> RegStrVal {
  return getEnvVar(name, EnvVarStore::User);
}

[[nodiscard]] auto inline getSysEnvVar(std::string_view name) noexcept -> RegStrVal {
  return getEnvVar(name, EnvVarStore::Machine);
}

[[nodiscard]] auto inline getUsrOrSysEnvVar(std::string_view name) noexcept -> RegStrVal {
  RegStrVal res = getEnvVar(name, EnvVarStore::User);
  if (res) {
    return res;
  }
  return getEnvVar(name, EnvVarStore::Machine);
}

auto setEnvVar(std::string_view name, std::string_view value, EnvVarStore store) noexcept
    -> OptWinErr;

auto inline setUsrEnvVar(std::string_view name, std::string_view value) noexcept -> OptWinErr {
  return setEnvVar(name, value, EnvVarStore::User);
}

auto addToDelimEnvVar(std::string_view name, std::string_view value) -> OptWinErr;

auto removeFromDelimEnvVar(std::string_view name, std::string_view value) -> OptWinErr;

} // namespace novrt::win32

#endif // _Win32
