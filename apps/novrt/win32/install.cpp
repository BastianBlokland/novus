#if defined(_WIN32)
#include "install.hpp"
#include "env.hpp"
#include "reg.hpp"
#include "utilities.hpp"
#include <array>
#include <iostream>

namespace novrt::win32 {

auto registerWin32FileAssoc() noexcept -> bool {

  const auto rtPath   = getExecutablePath();
  const auto homePath = rtPath.parent_path();

  std::cout << "-- Novus home path: '" << homePath << "'\n";
  std::cout << "-- Setting 'NOVUS_HOME' environment variable.\n";
  setUsrEnvVar("NOVUS_HOME", homePath.string());

  std::cout << "-- Adding registry class for 'Novus.Executable'.\n";

  constexpr std::string_view novExecKeyName = "SOFTWARE\\Classes\\Novus.Executable";
  auto novExecKey                           = getUsrRegKey(novExecKeyName);
  if (!novExecKey) {
    std::cerr << "Failed to add registry key: \n" << novExecKey.getErrorMsg();
    return false;
  }
  if (novExecKey.isNew()) {
    std::cout << "-- Added reg key: '" << novExecKeyName << "'\n";
  }
  if (const OptWinErr err = setRegStrVal(novExecKey, "Novus Executable")) {
    std::cerr << "Failed to write to registry key: \n" << winErrToString(*err);
    return false;
  }

  std::cout << "-- Adding shell open command for 'Novus.Executable'.\n";

  constexpr std::string_view novExecCmdKeyName =
      "SOFTWARE\\Classes\\Novus.Executable\\Shell\\open\\command";

  auto novExecCmdKey = getUsrRegKey(novExecCmdKeyName);
  if (!novExecCmdKey) {
    std::cerr << "Failed to add registry key: \n" << novExecCmdKey.getErrorMsg();
    return false;
  }
  if (novExecCmdKey.isNew()) {
    std::cout << "-- Added reg key: '" << novExecCmdKeyName << "'\n";
  }

  const size_t maxCmdValSize = MAX_PATH + 128u;
  char cmdValBuffer[maxCmdValSize];
  const int cmdValSize =
      ::snprintf(cmdValBuffer, maxCmdValSize, "\"%ls\" \"%%L\" %%*", rtPath.c_str());
  if (maxCmdValSize <= 0) {
    std::cerr << "Failed to create the shell command string";
    return false;
  }
  const auto cmdVal = std::string_view{cmdValBuffer, static_cast<size_t>(cmdValSize)};

  if (const OptWinErr err = setRegStrVal(novExecCmdKey, cmdVal)) {
    std::cerr << "Failed to write to registry key: \n" << winErrToString(*err);
    return false;
  }

  std::cout << "-- Associating '.nx' file extension with 'Novus.Executable' class.\n";

  constexpr std::string_view nxFileAssocKeyName = "SOFTWARE\\Classes\\.nx";
  auto nxFileAssocKey                           = getUsrRegKey(nxFileAssocKeyName);
  if (!nxFileAssocKey) {
    std::cerr << "Failed to add registry key: \n" << nxFileAssocKey.getErrorMsg();
    return false;
  }
  if (nxFileAssocKey.isNew()) {
    std::cout << "-- Added reg key: '" << nxFileAssocKeyName << "'\n";
  }
  if (const OptWinErr err = setRegStrVal(nxFileAssocKey, "Novus.Executable")) {
    std::cerr << "Failed to write to registry key: \n" << winErrToString(*err);
    return false;
  }

  std::cout << "-- Adding '.NX' to 'PATHEXT' environment variable.\n";
  if (const OptWinErr err = addToDelimEnvVar("PATHEXT", ".NX")) {
    std::cerr << "Failed to add to 'PATHEXT' environment variable: \n" << winErrToString(*err);
    return false;
  }

  std::cout << "-- Adding '%NOVUS_HOME%' to 'Path' environment variable.\n";
  if (const OptWinErr err = addToDelimEnvVar("Path", "%NOVUS_HOME%")) {
    std::cerr << "Failed to add to 'Path' environment variable: \n" << winErrToString(*err);
    return false;
  }

  std::cout << "<< NOTE: Please restart your shell for changes to take affect >>\n";

  return true;
}

auto unregisterWin32FileAssoc() noexcept -> bool {

  if (usrEnvVarExists("NOVUS_HOME")) {
    std::cout << "-- Removing 'NOVUS_HOME' environment variable.'\n";
    if (const OptWinErr err = deleteUsrEnvVar("NOVUS_HOME")) {
      std::cerr << "Failed to remove 'NOVUS_HOME' environment variable: \n" << winErrToString(*err);
      return false;
    }
  }

  // TODO: Create a util to delete a key with subkeys to avoid having to manually delete each.
  std::array<std::string_view, 5> keysToDelete = {
      "SOFTWARE\\Classes\\Novus.Executable\\Shell\\open\\command",
      "SOFTWARE\\Classes\\Novus.Executable\\Shell\\open",
      "SOFTWARE\\Classes\\Novus.Executable\\Shell",
      "SOFTWARE\\Classes\\Novus.Executable",
      "SOFTWARE\\Classes\\.nx",
  };

  for (const std::string_view key : keysToDelete) {
    if (usrRegKeyExists(key)) {
      std::cout << "-- Deleting registry key: '" << key << "'\n";
      if (const OptWinErr err = deleteUsrRegKey(key)) {
        std::cerr << "Failed to delete registry key: \n" << winErrToString(*err);
        return false;
      }
    }
  }

  std::cout << "-- Removing '.NX' from 'PATHEXT' environment variable.\n";
  if (const OptWinErr err = removeFromDelimEnvVar("PATHEXT", ".NX")) {
    std::cerr << "Failed to remove from 'PATHEXT' environment variable: \n" << winErrToString(*err);
    return false;
  }

  std::cout << "-- Removing '%NOVUS_HOME%' from 'Path' environment variable.\n";
  if (const OptWinErr err = removeFromDelimEnvVar("Path", "%NOVUS_HOME%")) {
    std::cerr << "Failed to remove from 'Path' environment variable: \n" << winErrToString(*err);
    return false;
  }

  return true;
}

} // namespace novrt::win32

#endif // _Win32
