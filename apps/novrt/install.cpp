#include "install.hpp"
#include "config.hpp"
#include "win32/install.hpp"
#include <iostream>

namespace novrt {

auto install(int argc, const char** /*unused*/) noexcept -> int {
  std::cout << "Novus runtime [" PROJECT_VER "]\nInstalling...\n";

  if (argc) {
    std::cerr << "Unexpected arguments to install command\n";
    return 1;
  }

  bool succeeded = true;
#if defined(_WIN32)
  succeeded = win32::registerWin32FileAssoc();
#endif // _WIN32

  // NOTE: Only on windows we have an install procedure defined at the moment, for other platforms
  // this is just a no-op.

  if (succeeded) {
    std::cout << "Installed successfully\n";
    return 0;
  } else {
    std::cerr << "Installation failed\n";
    return 1;
  }
}

auto uninstall(int argc, const char** /*unused*/) noexcept -> int {
  std::cout << "Novus runtime [" PROJECT_VER "]\nUninstalling...\n";

  if (argc) {
    std::cerr << "Unexpected arguments to uninstall command\n";
    return 1;
  }

  bool succeeded = true;
#if defined(_WIN32)
  succeeded = win32::unregisterWin32FileAssoc();
#endif // _WIN32

  // NOTE: Only on windows we have an uninstall procedure defined at the moment, for other platforms
  // this is just a no-op.

  if (succeeded) {
    std::cout << "Uninstalled successfully\n";
    return 0;
  } else {
    std::cerr << "Uninstall failed\n";
    return 1;
  }
}

} // namespace novrt
