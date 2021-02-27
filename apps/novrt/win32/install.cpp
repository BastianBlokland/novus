#if defined(_WIN32)
#include "install.hpp"
#include "reg.hpp"
#include "utilities.hpp"
#include <array>
#include <iostream>

namespace novrt::win32 {

constexpr std::string_view g_shellCommandRegKey =
    "SOFTWARE\\Classes\\Novus.Executable\\Shell\\open\\command";
constexpr std::string_view g_nxFileAssocRegKey = "SOFTWARE\\Classes\\.nx";

auto registerWin32FileAssoc() noexcept -> bool {

  std::cout << "-- Adding registry class for 'Novus.Executable'.\n";
  auto novExecClassKey = getUsrRegKey(g_shellCommandRegKey);
  if (!novExecClassKey) {
    std::cerr << "Failed to add registry key: \n" << novExecClassKey.getErrorMsg();
    return false;
  }
  if (novExecClassKey.isNew()) {
    std::cout << "-- Added reg key: '" << g_shellCommandRegKey << "'\n";
  }

  const auto rtPath = getExecutablePath();

  const size_t maxCmdValSize = MAX_PATH + 128u;
  char cmdValBuffer[maxCmdValSize];
  const int cmdValSize =
      ::snprintf(cmdValBuffer, maxCmdValSize, "\"%ls\" \"%%L\" %%*", rtPath.c_str());
  if (maxCmdValSize <= 0) {
    std::cerr << "Failed to create the shell command string";
    return false;
  }
  const auto cmdVal = std::string_view{cmdValBuffer, static_cast<size_t>(cmdValSize)};

  if (const OptWinErr err = setRegStrVal(novExecClassKey, cmdVal)) {
    std::cerr << "Failed to write to registry key: \n" << winErrToString(*err);
    return false;
  }

  std::cout << "-- Associating '.nx' file extension with 'Novus.Executable' class.\n";

  auto nxFileAssocKey = getUsrRegKey(g_nxFileAssocRegKey);
  if (!nxFileAssocKey) {
    std::cerr << "Failed to add registry key: \n" << novExecClassKey.getErrorMsg();
    return false;
  }
  if (novExecClassKey.isNew()) {
    std::cout << "-- Added reg key: '" << g_nxFileAssocRegKey << "'\n";
  }
  if (const OptWinErr err = setRegStrVal(nxFileAssocKey, "Novus.Executable")) {
    std::cerr << "Failed to write to registry key: \n" << winErrToString(*err);
    return false;
  }

  return true;
}

auto unregisterWin32FileAssoc() noexcept -> bool {

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

  return true;
}

} // namespace novrt::win32

#endif // _Win32
