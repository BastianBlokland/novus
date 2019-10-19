#include "CLI/CLI.hpp"
#include "lex/lexer.hpp"
#include "parse/node.hpp"
#include "parse/node_type.hpp"
#include "parse/parser.hpp"
#include "rang.hpp"
#include <ios>
#include <iostream>
#include <iterator>

using high_resolution_clock = std::chrono::high_resolution_clock;
using duration              = std::chrono::duration<double>;

auto getFgColor(const parse::Node& node) -> rang::fg;
auto getBgColor(const parse::Node& node) -> rang::bg;

auto operator<<(std::ostream& out, const duration& rhs) -> std::ostream&;

auto printNode(const parse::Node& n, std::string prefix = "", bool isLastSibling = false) -> void {
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

  // Print node.
  std::cout << s::bold << getFgColor(n) << getBgColor(n) << n << fg::reset << bg::reset << s::reset
            << s::dim << s::italic << ' ' << n.getType() << '\n'
            << s::reset;

  const auto childCount = n.getChildCount();
  for (auto i = 0; i < childCount; ++i) {
    printNode(n[i], prefix, i == (childCount - 1));
  }
}

template <typename InputItr>
auto run(InputItr inputBegin, const InputItr inputEnd, const bool printNodes) {
  const auto width  = 80;
  const auto tokens = lex::lexAll(inputBegin, inputEnd);

  // Parse all the tokens and time how long it takes.
  const auto t1       = high_resolution_clock::now();
  const auto nodes    = parse::parseAll(tokens.begin(), tokens.end());
  const auto t2       = high_resolution_clock::now();
  const auto parseDur = std::chrono::duration_cast<duration>(t2 - t1);

  std::cout << rang::style::dim << rang::style::italic << std::string(width, '-') << '\n'
            << "Parsed " << nodes.size() << " nodes from " << tokens.size() << " tokens in "
            << parseDur << '\n'
            << std::string(width, '-') << '\n'
            << rang::style::reset;

  if (printNodes) {
    for (const auto& node : nodes) {
      printNode(*node);
      std::cout << rang::style::dim << " │\n" << rang::style::reset;
    }
    std::cout << rang::style::dim << std::string(width, '-') << '\n';
  }
}

auto main(int argc, char** argv) -> int {
  auto app = CLI::App{"Parser diagnostic tool"};
  app.require_subcommand(1);

  auto printNodes = true;
  app.add_flag("!--skip-nodes", printNodes, "Skip printing the nodes")->capture_default_str();

  // Parse input characters.
  std::string charsInput;
  auto lexCmd = app.add_subcommand("parse", "Parse the provided characters")->callback([&]() {
    run(charsInput.begin(), charsInput.end(), printNodes);
  });
  lexCmd->add_option("input", charsInput, "Input characters to parse")->required();

  // Parse input file.
  std::string filePath;
  auto lexFileCmd =
      app.add_subcommand("parsefile", "Parse all characters in a file")->callback([&]() {
        std::ifstream fs{filePath};
        run(std::istreambuf_iterator<char>{fs}, std::istreambuf_iterator<char>{}, printNodes);
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

auto getFgColor(const parse::Node& node) -> rang::fg {
  using nt = parse::NodeType;

  switch (node.getType()) {
  case nt::ExprBinaryOp:
  case nt::ExprUnaryOp:
  case nt::ExprSwitch:
  case nt::ExprSwitchIf:
  case nt::ExprGroup:
  case nt::ExprParen:
    return rang::fg::green;
  case nt::StmtFunDecl:
  case nt::StmtPrint:
    return rang::fg::blue;
  case nt::ExprCall:
  case nt::ExprConst:
  case nt::ExprConstDecl:
    return rang::fg::magenta;
  case nt::ExprLit:
    return rang::fg::cyan;
  case nt::Error:
    return rang::fg::reset;
  }
  return rang::fg::reset;
}

auto getBgColor(const parse::Node& node) -> rang::bg {
  switch (node.getType()) {
  case parse::NodeType::Error:
    return rang::bg::red;
  default:
    return rang::bg::reset;
  }
  return rang::bg::reset;
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
