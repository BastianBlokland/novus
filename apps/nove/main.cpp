// -- Include rang before any os headers.
#include "../rang_include.hpp"
// --

#include "backend/generator.hpp"
#include "config.hpp"
#include "filesystem.hpp"
#include "frontend/analysis.hpp"
#include "frontend/source.hpp"
#include "input/search_paths.hpp"
#include "opt/opt.hpp"
#include "vm/exec_state.hpp"
#include "vm/platform_interface.hpp"
#include "vm/vm.hpp"
#include <cstdio>
#include <fstream>
#include <system_error>

template <typename InputItr>
auto run(
    const std::string& inputId,
    std::optional<filesystem::path> inputPath,
    const std::vector<filesystem::path>& searchPaths,
    InputItr inputBegin,
    const InputItr inputEnd,
    int vmEnvArgsCount,
    char* const* vmEnvArgs) noexcept {

  const auto src            = frontend::buildSource(inputId, inputPath, inputBegin, inputEnd);
  const auto frontendOutput = frontend::analyze(src, searchPaths);

  if (frontendOutput.isSuccess()) {

    const auto optProg   = opt::optimize(frontendOutput.getProg());
    const auto asmOutput = backend::generate(optProg);

    const auto progPath = inputPath ? inputPath->c_str() : nullptr;
    auto iface = vm::PlatformInterface{progPath, vmEnvArgsCount, vmEnvArgs, stdin, stdout, stderr};
    auto res   = vm::run(&asmOutput.first, &iface);

    if (res > vm::ExecState::Failed) {
      std::cerr << rang::style::bold << rang::bg::red << "runtime error: " << res << '\n'
                << rang::style::reset;
    }
    return static_cast<int>(res);
  }

  if (!frontendOutput.isSuccess()) {
    std::cerr << rang::style::bold << rang::bg::red;
    for (auto diagItr = frontendOutput.beginDiags(); diagItr != frontendOutput.endDiags();
         ++diagItr) {
      std::cerr << *diagItr << '\n';
    }
    std::cerr << rang::style::reset;
  }
  return 1;
}

auto printUsage() {
  std::cerr << rang::fg::red
            << "Novus evaluator [" PROJECT_VER "] - Please provide input characters or input file\n"
            << rang::style::reset;
}

auto main(int argc, char** argv) noexcept -> int {
  if (argc <= 1) {
    printUsage();
    return 1;
  }

  const auto vmEnvArgsCount = argc - 2; // 1 for program path and 1 for source.
  auto** const vmEnvArgs    = argv + 2;
  const auto searchPaths    = input::getSearchPaths(argv);

  auto path = filesystem::path{argv[1]};
  if (filesystem::is_directory(path)) {
    printUsage();
    return 1;
  }

  auto fs = std::ifstream{path.string()};
  if (fs.good()) {
    return run(
        path.filename().string(),
        filesystem::canonical(path),
        searchPaths,
        std::istreambuf_iterator<char>{fs},
        std::istreambuf_iterator<char>{},
        vmEnvArgsCount,
        vmEnvArgs);
  }

  auto progTxt = std::string{argv[1]};
  progTxt.append(" import \"std.nov\"");
  return run(
      "inline",
      std::nullopt,
      searchPaths,
      progTxt.begin(),
      progTxt.end(),
      vmEnvArgsCount,
      vmEnvArgs);
}
