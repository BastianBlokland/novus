#include "backend/generator.hpp"
#include "frontend/analysis.hpp"
#include "frontend/source.hpp"
#include "rang.hpp"
#include "vm/exec_state.hpp"
#include "vm/platform/terminal_interface.hpp"
#include "vm/vm.hpp"
#include <filesystem>
#include <fstream>
#include <system_error>

namespace eval {

template <typename InputItr>
auto run(
    const std::string& inputId,
    std::optional<std::filesystem::path> inputPath,
    const std::vector<std::filesystem::path>& searchPaths,
    InputItr inputBegin,
    const InputItr inputEnd,
    int vmEnvArgsCount,
    char** vmEnvArgs) noexcept {

  const auto src = frontend::buildSource(inputId, std::move(inputPath), inputBegin, inputEnd);
  const auto frontendOutput = frontend::analyze(src, searchPaths);
  if (frontendOutput.isSuccess()) {
    const auto assembly = backend::generate(frontendOutput.getProg());
    auto iface          = vm::platform::TerminalInterface{vmEnvArgsCount, vmEnvArgs};
    auto res            = vm::execute(assembly, iface);
    if (res != vm::ExecState::Success) {
      std::cout << rang::style::bold << rang::bg::red << "Runtime error: " << res << '\n'
                << rang::style::reset;
    }
    return static_cast<int>(res);
  }

  if (!frontendOutput.isSuccess()) {
    for (auto diagItr = frontendOutput.beginDiags(); diagItr != frontendOutput.endDiags();
         ++diagItr) {
      std::cout << rang::style::bold << rang::bg::red << *diagItr << '\n' << rang::style::reset;
    }
  }
  return 1;
}

} // namespace eval

auto getSearchPaths(char** argv) noexcept {
  auto result = std::vector<std::filesystem::path>{};

  // Add the path to the binary.
  result.push_back(std::filesystem::absolute(argv[0]).parent_path());

  return result;
}

auto main(int argc, char** argv) noexcept -> int {
  if (argc <= 1) {
    std::cout << rang::style::bold << rang::bg::red
              << "Evaluator - Please provide input characters or input file\n"
              << rang::style::reset;
    return 1;
  }

  auto vmEnvArgsCount = argc - 2; // 1 for program path and 1 for source.
  auto vnEnvArgs      = argv + 2;

  auto path = std::filesystem::path{argv[1]};
  auto fs   = std::ifstream{path};
  if (fs.good()) {
    std::error_code getAbsErr;
    auto absInputPath = std::filesystem::absolute(path, getAbsErr);
    return eval::run(
        path.filename(),
        absInputPath,
        getSearchPaths(argv),
        std::istreambuf_iterator<char>{fs},
        std::istreambuf_iterator<char>{},
        vmEnvArgsCount,
        vnEnvArgs);
  }
  return eval::run<char*>(
      "inline", std::nullopt, getSearchPaths(argv), argv[1], nullptr, vmEnvArgsCount, vnEnvArgs);
}
