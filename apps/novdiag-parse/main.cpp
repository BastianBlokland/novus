// -- Include rang before any os headers.
#include "../rang_include.hpp"
// --

#include "CLI/CLI.hpp"
#include "config.hpp"
#include "filesystem.hpp"
#include "get_color.hpp"
#include "input/info.hpp"
#include "lex/lexer.hpp"
#include "parse/parser.hpp"
#include <chrono>

using Clock    = std::chrono::high_resolution_clock;
using Duration = std::chrono::duration<double>;

auto operator<<(std::ostream& out, const Duration& rhs) -> std::ostream&;

auto printNode(
    const parse::Node& n,
    const input::Info& inputInfo,
    std::string prefix = "",
    bool isLastSibling = false) -> void {

  const static auto cross  = " ├─";
  const static auto corner = " └─";
  const static auto vert   = " │ ";
  const static auto indent = "   ";

  using S = rang::style;

  // Print prefix.
  std::cout << S::dim << prefix;
  if (isLastSibling) {
    std::cout << corner;
    prefix += indent;
  } else {
    std::cout << cross;
    prefix += vert;
  }
  std::cout << S::reset;

  auto nodeCol = GetExprColor{};
  n.accept(&nodeCol);

  // Print node.
  auto span  = n.getSpan();
  auto start = inputInfo.getTextPos(span.getStart());
  std::cout << S::bold << nodeCol.getFgColor() << nodeCol.getBgColor() << n << S::reset << S::reset
            << S::dim << S::italic << " " << start << '\n'
            << S::reset;

  const auto childCount = n.getChildCount();
  for (auto i = 0U; i < childCount; ++i) {
    printNode(n[i], inputInfo, prefix, i == (childCount - 1));
  }
}

template <typename InputItr>
auto run(InputItr inputBegin, const InputItr inputEnd, const bool outputNodes) {
  const auto width = 80;

  // Parse the input and time how long it takes.
  const auto t1       = Clock::now();
  auto inputInfo      = input::Info{};
  auto lexer          = lex::Lexer{input::InfoItr{inputBegin, &inputInfo}, inputEnd};
  const auto nodes    = parse::parseAll(lexer.begin(), lexer.end());
  const auto t2       = Clock::now();
  const auto parseDur = std::chrono::duration_cast<Duration>(t2 - t1);

  std::cout << rang::style::dim << rang::style::italic << std::string(width, '-') << '\n'
            << "Parsed " << nodes.size() << " nodes in " << parseDur << '\n'
            << std::string(width, '-') << '\n'
            << rang::style::reset;

  if (outputNodes) {
    for (const auto& node : nodes) {
      printNode(*node, inputInfo);
      std::cout << rang::style::dim << " │\n" << rang::style::reset;
    }
    std::cout << rang::style::dim << std::string(width, '-') << '\n';
  }
}

auto operator<<(std::ostream& out, const Duration& rhs) -> std::ostream& {
  auto s = rhs.count();
  if (s < .000001) {                // NOLINT: Magic numbers
    out << s * 1000000000 << " ns"; // NOLINT: Magic numbers
  } else if (s < .001) {            // NOLINT: Magic numbers
    out << s * 1000000 << " us";    // NOLINT: Magic numbers
  } else if (s < 1) {               // NOLINT: Magic numbers
    out << s * 1000 << " ms";       // NOLINT: Magic numbers
  } else {
    out << s << " s";
  }
  return out;
}

auto main(int argc, char** argv) -> int {
  auto app = CLI::App{"Novus parser diagnostic tool [" PROJECT_VER "]", "novdiag-parse"};
  app.require_subcommand(1);

  auto colorMode   = rang::control::Auto;
  auto printOutput = true;

  app.add_flag(
      "--no-color{0},-c{2},--color{2},--auto-color{1}",
      colorMode,
      "Set the color output behaviour");

  // Parse input characters.
  std::string charsInput;
  auto analyzeCmd = app.add_subcommand("analyze", "Parse the provided characters")->callback([&]() {
    rang::setControlMode(colorMode);

    run(charsInput.begin(), charsInput.end(), printOutput);
  });
  analyzeCmd->add_option("input", charsInput, "Input characters")->required();
  analyzeCmd->add_flag("!--no-output", printOutput, "Skip printing the nodes");

  // Parse input file.
  filesystem::path filePath;
  auto analyzeFileCmd =
      app.add_subcommand("analyzefile", "Parse all characters in a file")->callback([&]() {
        rang::setControlMode(colorMode);

        auto absFilePath = filesystem::absolute(filePath);
        std::ifstream fs{filePath.string()};
        run(std::istreambuf_iterator<char>{fs}, std::istreambuf_iterator<char>{}, printOutput);
      });
  analyzeFileCmd->add_option("file", filePath, "Path to file")
      ->check(CLI::ExistingFile)
      ->required();
  analyzeFileCmd->add_flag("!--no-output", printOutput, "Skip printing the nodes");

  // Parse arguments and run subcommands.
  std::atexit([]() {
    std::cout << rang::style::reset;
    std::cerr << rang::style::reset;
  });
  try {
    app.parse(argc, argv);
  } catch (const CLI::ParseError& e) {
    if (e.get_exit_code() != 0) {
      std::cerr << rang::fg::red;
    }
    return app.exit(e);
  }
  return 0;
}
