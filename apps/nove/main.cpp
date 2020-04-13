#include "backend/generator.hpp"
#include "filesystem.hpp"
#include "frontend/analysis.hpp"
#include "frontend/source.hpp"
#include "input/search_paths.hpp"
#include "opt/opt.hpp"
#include "rang.hpp"
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

  const auto src = frontend::buildSource(inputId, std::move(inputPath), inputBegin, inputEnd);
  const auto frontendOutput = frontend::analyze(src, searchPaths);
  if (frontendOutput.isSuccess()) {
    const auto optProg   = opt::optimize(frontendOutput.getProg());
    const auto asmOutput = backend::generate(optProg);
    auto iface           = vm::PlatformInterface{vmEnvArgsCount, vmEnvArgs, stdin, stdout, stderr};
    auto res             = vm::run(&asmOutput.first, &iface);
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

auto main(int argc, char** argv) noexcept -> int {
  if (argc <= 1) {
    std::cerr << rang::fg::red
              << "Novus evaluator - Please provide input characters or input file\n"
              << rang::style::reset;
    return 1;
  }

  const auto vmEnvArgsCount = argc - 2; // 1 for program path and 1 for source.
  auto** const vnEnvArgs    = argv + 2;
  const auto searchPaths    = input::getSearchPaths(argv);

  auto path = filesystem::path{argv[1]};
  auto fs   = std::ifstream{path};
  if (fs.good()) {
    auto absInputPath = filesystem::absolute(path);
    return run(
        path.filename(),
        filesystem::canonical(absInputPath),
        searchPaths,
        std::istreambuf_iterator<char>{fs},
        std::istreambuf_iterator<char>{},
        vmEnvArgsCount,
        vnEnvArgs);
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
      vnEnvArgs);
}
