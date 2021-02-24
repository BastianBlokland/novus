#include "config.hpp"
#include "filesystem.hpp"
#include "novasm/serialization.hpp"
#include "vm/exec_state.hpp"
#include "vm/platform_interface.hpp"
#include "vm/vm.hpp"
#include <cstdio>
#include <fstream>

auto main(int argc, const char** argv) noexcept -> int {

  if (argc < 2) {
    std::cerr << "Novus runtime [" PROJECT_VER
                 "] - Please provide path to a 'nx' executable file\n";
    return 1;
  }
  auto relProgPath = filesystem::path(std::string(argv[1]));

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

  const auto consumedArgs   = 1; // Consume the path to the runtime executable itself.
  const auto vmEnvArgsCount = argc - consumedArgs;
  const char** vmEnvArgs    = argv + consumedArgs;
  auto absProgPath          = filesystem::canonical(relProgPath).string();

  auto iface = vm::PlatformInterface{
      std::move(absProgPath),
      vmEnvArgsCount,
      vmEnvArgs,
      vm::fileStdIn(),
      vm::fileStdOut(),
      vm::fileStdErr()};
  auto res = vm::run(&asmOutput.value(), &iface);
  if (res > vm::ExecState::Failed) {
    std::cerr << "runtime error: " << res << '\n';
  }
  return static_cast<int>(res);
}
