#if defined(_WIN32)
#include "env.hpp"
#include "Windows.h"
#include "reg.hpp"

namespace novrt::win32 {

namespace {

[[noreturn]] auto errorExit(const char* msg) noexcept {
  std::fprintf(stderr, "%s\n", msg);
  std::fflush(stderr);
  std::exit(EXIT_FAILURE);
}

auto getKeyStore(EnvVarStore store) noexcept -> RegKeyStore {
  switch (store) {
  case EnvVarStore::User:
    return RegKeyStore::User;
  case EnvVarStore::Machine:
    return RegKeyStore::Machine;
  }
  errorExit("Unsupported environment variable store");
}

auto getRegKeyName(EnvVarStore store) noexcept -> std::string_view {
  switch (store) {
  case EnvVarStore::User:
    return "Environment";
  case EnvVarStore::Machine:
    return "SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment";
  }
  errorExit("Unsupported environment variable store");
}

} // namespace

auto getEnvVar(std::string_view name, EnvVarStore store) noexcept -> RegStrVal {
  const RegKey regKey = getRegKey(
      getRegKeyName(store), getKeyStore(store), RegKeyAccessMode::Read, RegKeyCreateMode::Open);
  if (!regKey) {
    return RegStrVal{regKey.getErrorCode()};
  }
  return getRegStrVal(regKey, name);
}

auto setEnvVar(std::string_view name, std::string_view value, EnvVarStore store) noexcept
    -> OptWinErr {
  const RegKey regKey = getRegKey(
      getRegKeyName(store), getKeyStore(store), RegKeyAccessMode::Write, RegKeyCreateMode::Open);
  if (!regKey) {
    return regKey.getErrorCode();
  }
  return setRegStrVal(regKey, value, name);
}

auto addToDelimEnvVar(std::string_view name, std::string_view value) -> OptWinErr {
  RegStrVal regStrVal = getUsrOrSysEnvVar(name);
  if (!regStrVal) {
    return regStrVal.getErrorCode();
  }
  const std::string_view delimVal = regStrVal.getVal();

  // Check if the value is already present.
  for (size_t i = 0, end;; i = end + 1) {
    end                  = delimVal.find(';', i);
    std::string_view ext = delimVal.substr(i, end - i);
    if (ext == value) {
      return winSuccess(); // Value is allready present.
    }
    if (end == delimVal.npos || end == delimVal.size()) {
      break; // Reached the end of the collection.
    }
  }

  // Value is not present, add it.
  std::string newDelimVal = std::string{delimVal};
  if (!newDelimVal.empty() && newDelimVal.back() != ';') {
    newDelimVal += ';';
  }
  newDelimVal += value;

  return setEnvVar(name, newDelimVal, EnvVarStore::User);
}

auto removeFromDelimEnvVar(std::string_view name, std::string_view value) -> OptWinErr {
  RegStrVal regStrVal = getUsrEnvVar(name);
  if (!regStrVal) {
    return regStrVal.getErrorCode();
  }
  const std::string_view delimVal = regStrVal.getVal();

  std::string newDelimVal;
  newDelimVal.reserve(delimVal.size());

  // Construct a new delimited collection without 'value'.
  for (size_t i = 0, end;; i = end + 1) {
    end                  = delimVal.find(';', i);
    std::string_view ext = delimVal.substr(i, end - i);
    if (ext != value) {
      if (!newDelimVal.empty()) {
        newDelimVal += ';';
      }
      newDelimVal += ext;
    }
    if (end == delimVal.npos || end == delimVal.size()) {
      break; // Reached the end of the collection.
    }
  }

  return setEnvVar(name, newDelimVal, EnvVarStore::User);
}

} // namespace novrt::win32

#endif // _Win32
