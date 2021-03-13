#include "config.hpp"
#include "filesystem.hpp"
#include "metacmd.hpp"
#include "novasm/serialization.hpp"
#include "vm/exec_state.hpp"
#include "vm/platform_interface.hpp"
#include "vm/vm.hpp"
#include <cstdio>
#include <fstream>

auto main(int argc, const char** argv) noexcept -> int {
  // Drop the first arg (path to this executable).
  if (argc > 0) {
    argc -= 1;
    argv += 1;
  }

  // If a meta command was invoked (like --install) then execute it.
  if (argc && strncmp(argv[0], "--", 2) == 0) {
    return novrt::execMetaCommand(argc, argv);
  }

  // Otherwise execute a nx executable.

  if (!argc) {
    std::cerr << "Novus runtime [" PROJECT_VER
                 "] - Please provide path to a 'nx' executable file\n";
    return 1;
  }
  auto relProgPath = filesystem::path(std::string(argv[0]));
  // Drop the first arg (path to the novus executable).
  if (argc > 0) {
    argc -= 1;
    argv += 1;
  }

  // Open a handle to the program executable file.
  auto fs = std::ifstream{relProgPath.string(), std::ios::binary};
  if (!fs.good()) {
    std::cerr << "Novus runtime [" PROJECT_VER "] - Failed to open file: " << relProgPath << '\n';
    return 1;
  }

  const auto asmOutput =
      novasm::deserialize(std::istreambuf_iterator<char>{fs}, std::istreambuf_iterator<char>{});
  if (!asmOutput) {
    std::cerr << "Novus runtime [" PROJECT_VER "] - Corrupt or incompatible 'nx' executable file\n";
    return 1;
  }

  // Close the handle to the program executable file.
  fs.close();

  auto iface = vm::PlatformInterface{
      filesystem::canonical(relProgPath).string(),
      argc,
      argv,
      vm::fileStdIn(),
      vm::fileStdOut(),
      vm::fileStdErr()};

  auto res = vm::run(&asmOutput.value(), &iface);
  if (res > vm::ExecState::Failed) {
    std::cerr << "runtime error: " << res << '\n';
  }
  return static_cast<int>(res);
}
