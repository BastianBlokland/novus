#include "CLI/CLI.hpp"
#include "backend/generator.hpp"
#include "frontend/analysis.hpp"
#include "frontend/source.hpp"
#include "rang.hpp"
#include "vm/executor.hpp"
#include "vm/io/terminal_interface.hpp"

namespace eval {

template <typename InputItr>
auto run(const std::string& inputId, InputItr inputBegin, const InputItr inputEnd) {

  const auto src            = frontend::buildSource(inputId, inputBegin, inputEnd);
  const auto frontendOutput = frontend::analyze(src);
  if (frontendOutput.isSuccess()) {
    const auto vmProg = backend::generate(frontendOutput.getProg());
    auto interface    = vm::io::TerminalInterface{};
    vm::execute(vmProg, &interface);
  }

  if (!frontendOutput.isSuccess()) {
    for (auto diagItr = frontendOutput.beginDiags(); diagItr != frontendOutput.endDiags();
         ++diagItr) {
      std::cout << rang::style::bold << rang::bg::red << *diagItr << rang::bg::reset << '\n'
                << rang::style::reset;
    }
  }
}

} // namespace eval

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

  std::ifstream fs{input};
  if (fs.good()) {
    eval::run(input, std::istreambuf_iterator<char>{fs}, std::istreambuf_iterator<char>{});
  } else {
    eval::run("inline", input.begin(), input.end());
  }

  return 0;
}
