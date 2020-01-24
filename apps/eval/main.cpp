#include "CLI/CLI.hpp"
#include "backend/generator.hpp"
#include "frontend/analysis.hpp"
#include "frontend/source.hpp"
#include "rang.hpp"
#include "vm/executor.hpp"
#include "vm/platform/terminal_interface.hpp"
#include <filesystem>
#include <system_error>

namespace eval {

template <typename InputItr>
auto run(
    const std::string& inputId,
    std::optional<std::filesystem::path> inputPath,
    const std::vector<std::filesystem::path>& searchPaths,
    InputItr inputBegin,
    const InputItr inputEnd) {

  const auto src = frontend::buildSource(inputId, std::move(inputPath), inputBegin, inputEnd);
  const auto frontendOutput = frontend::analyze(src, searchPaths);
  if (frontendOutput.isSuccess()) {
    const auto assembly = backend::generate(frontendOutput.getProg());
    auto iface          = vm::platform::TerminalInterface{};
    try {
      vm::execute(assembly, iface);
      return 0;
    } catch (const std::exception& e) {
      std::cout << rang::bg::red << "Runtime error: " << e.what() << '\n';
      return 1;
    }
  }

  if (!frontendOutput.isSuccess()) {
    for (auto diagItr = frontendOutput.beginDiags(); diagItr != frontendOutput.endDiags();
         ++diagItr) {
      std::cout << rang::style::bold << rang::bg::red << *diagItr << rang::bg::reset << '\n'
                << rang::style::reset;
    }
  }
  return 1;
}

} // namespace eval

auto getSearchPaths(char** argv) {
  auto result = std::vector<std::filesystem::path>{};

  // Add the path to the binary.
  result.push_back(std::filesystem::absolute(argv[0]).parent_path());

  return result;
}

auto main(int argc, char** argv) -> int {
  auto app = CLI::App{"Evaluator"};

  std::string input;
  app.add_option("input", input, "Input characters or input file")->required();

  // Parse arguments.
  std::atexit([]() { std::cout << rang::style::reset; });
  try {
    app.parse(argc, argv);
  } catch (const CLI::ParseError& e) {
    std::cout << (e.get_exit_code() == 0 ? rang::fg::green : rang::fg::red);
    return app.exit(e);
  }

  auto path = std::filesystem::path{input};
  auto fs   = std::ifstream{path};
  if (fs.good()) {
    std::error_code getAbsErr;
    auto absInputPath = std::filesystem::absolute(path, getAbsErr);
    return eval::run(
        path.filename(),
        absInputPath,
        getSearchPaths(argv),
        std::istreambuf_iterator<char>{fs},
        std::istreambuf_iterator<char>{});
  }
  return eval::run("inline", std::nullopt, getSearchPaths(argv), input.begin(), input.end());
}
