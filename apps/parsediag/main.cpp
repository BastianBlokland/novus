#include "CLI/CLI.hpp"
#include "lex/lexer.hpp"
#include "parse/parser.hpp"
#include "rang.hpp"
#include <ios>
#include <iostream>
#include <iterator>

template <typename InputItr>
auto run(InputItr inputBegin, const InputItr inputEnd) {
  auto tokens = lex::lexAll(inputBegin, inputEnd);
  auto nodes  = parse::parseAll(tokens.begin(), tokens.end());

  std::cout << "Parsed " << nodes.size() << " nodes from " << tokens.size() << " tokens\n";
  for (const auto& node : nodes) {
    std::cout << "* Node: " << *node << '\n';
  }
}

auto main(int argc, char** argv) -> int {
  auto app = CLI::App{"Parser diagnostic tool"};
  app.require_subcommand(1);

  // Parse input characters.
  std::string charsInput;
  auto lexCmd = app.add_subcommand("parse", "Parse the provided characters.")->callback([&]() {
    run(charsInput.begin(), charsInput.end());
  });
  lexCmd->add_option("input", charsInput, "Input characters to parse.")->required();

  // Parse input file.
  std::string filePath;
  auto lexFileCmd =
      app.add_subcommand("parsefile", "Parse all characters in a file.")->callback([&]() {
        std::ifstream fs{filePath};
        run(std::istreambuf_iterator<char>{fs}, std::istreambuf_iterator<char>{});
      });
  lexFileCmd->add_option("file", filePath, "Path to file to parse.")
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
