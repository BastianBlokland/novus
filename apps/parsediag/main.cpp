#include "CLI/CLI.hpp"
#include "input/info.hpp"
#include "lex/lexer.hpp"
#include "parse/parser.hpp"
#include "rang.hpp"
#include "visitors/get_color.hpp"

namespace parsediag {

using high_resolution_clock = std::chrono::high_resolution_clock;
using duration              = std::chrono::duration<double>;

auto operator<<(std::ostream& out, const duration& rhs) -> std::ostream&;

auto printNode(
    const parse::Node& n,
    const input::Info& inputInfo,
    std::string prefix = "",
    bool isLastSibling = false) -> void {

  const static auto cross  = " ├─";
  const static auto corner = " └─";
  const static auto vert   = " │ ";
  const static auto indent = "   ";

  using s  = rang::style;
  using fg = rang::style;
  using bg = rang::style;

  // Print prefix.
  std::cout << s::dim << prefix;
  if (isLastSibling) {
    std::cout << corner;
    prefix += indent;
  } else {
    std::cout << cross;
    prefix += vert;
  }
  std::cout << s::reset;

  auto nodeCol = visitors::GetColor{};
  n.accept(&nodeCol);

  // Print node.
  auto span  = n.getSpan();
  auto start = inputInfo.getTextPos(span.getStart());
  std::cout << s::bold << nodeCol.getFgColor() << nodeCol.getBgColor() << n << fg::reset
            << bg::reset << s::reset << s::dim << s::italic << " " << start << '\n'
            << s::reset;

  const auto childCount = n.getChildCount();
  for (auto i = 0U; i < childCount; ++i) {
    printNode(n[i], inputInfo, prefix, i == (childCount - 1));
  }
}

template <typename InputItr>
auto run(InputItr inputBegin, const InputItr inputEnd, const bool outputNodes) {
  const auto width = 80;

  // Parse the input and time how long it takes.
  const auto t1       = high_resolution_clock::now();
  auto inputInfo      = input::Info{};
  auto lexer          = lex::Lexer{input::InfoItr{inputBegin, &inputInfo}, inputEnd};
  const auto nodes    = parse::parseAll(lexer.begin(), lexer.end());
  const auto t2       = high_resolution_clock::now();
  const auto parseDur = std::chrono::duration_cast<duration>(t2 - t1);

  std::cout << rang::style::dim << rang::style::italic << std::string(width, '-') << '\n'
            << "Parsed " << nodes.size() << " nodes from " << inputInfo.getCharCount()
            << " chars in " << parseDur << '\n'
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

auto operator<<(std::ostream& out, const duration& rhs) -> std::ostream& {
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

} // namespace parsediag

auto main(int argc, char** argv) -> int {
  auto app = CLI::App{"Parser diagnostic tool"};
  app.require_subcommand(1);

  auto printOutput = true;
  app.add_flag("!--skip-output", printOutput, "Skip printing the nodes")->capture_default_str();

  // Parse input characters.
  std::string charsInput;
  auto lexCmd = app.add_subcommand("parse", "Parse the provided characters")->callback([&]() {
    parsediag::run(charsInput.begin(), charsInput.end(), printOutput);
  });
  lexCmd->add_option("input", charsInput, "Input characters to parse")->required();

  // Parse input file.
  std::string filePath;
  auto lexFileCmd =
      app.add_subcommand("parsefile", "Parse all characters in a file")->callback([&]() {
        std::ifstream fs{filePath};
        parsediag::run(
            std::istreambuf_iterator<char>{fs}, std::istreambuf_iterator<char>{}, printOutput);
      });
  lexFileCmd->add_option("file", filePath, "Path to file to parse")
      ->check(CLI::ExistingFile)
      ->required();

  // Parse arguments and run subcommands.
  std::atexit([]() { std::cout << rang::style::reset; });
  try {
    app.parse(argc, argv);
  } catch (const CLI::ParseError& e) {
    std::cout << (e.get_exit_code() == 0 ? rang::fg::green : rang::fg::red);
    return app.exit(e);
  }
  return 0;
}
