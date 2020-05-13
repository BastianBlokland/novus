#include "filesystem.hpp"
#include "novasm/serialization.hpp"
#include "vm/exec_state.hpp"
#include "vm/platform_interface.hpp"
#include "vm/vm.hpp"
#include <cstdio>
#include <fstream>

auto main(int argc, char** argv) noexcept -> int {

  /* Note: Supports either reading a 'nova' assembly file as argment 1 or looking for a 'prog.nova'
   * in current working directory. */

  auto implicitPath = true;
  auto progPath     = filesystem::path{"prog.nova"};
  if (argc >= 2) {
    auto arg1Path = filesystem::path(std::string(argv[1]));
    if (arg1Path.extension() == ".nova") {
      progPath     = std::move(arg1Path);
      implicitPath = false;
    }
  }

  auto fs = std::ifstream{progPath.string(), std::ios::binary};
  if (!fs.good()) {
    if (implicitPath) {
      std::cerr << "Novus runtime - Please provide path to a 'nova' file\n";
    } else {
      std::cerr << "Novus runtime - Failed to open file: " << progPath << '\n';
    }
    return 1;
  }

  const auto asmOutput =
      novasm::deserialize(std::istreambuf_iterator<char>{fs}, std::istreambuf_iterator<char>{});
  if (!asmOutput) {
    std::cerr << "Novus runtime - Corrupt 'nova' file\n";
    return 1;
  }

  const auto consumedArgs   = implicitPath ? 1 : 2; // 1 for executable path and 1 for nova file.
  const auto vmEnvArgsCount = argc - consumedArgs;
  auto** const vmEnvArgs    = argv + consumedArgs;

  auto iface = vm::PlatformInterface{vmEnvArgsCount, vmEnvArgs, stdin, stdout, stderr};
  auto res   = vm::run(&asmOutput.value(), &iface);
  if (res > vm::ExecState::Failed) {
    std::cerr << "runtime error: " << res << '\n';
  }
  return static_cast<int>(res);
}
