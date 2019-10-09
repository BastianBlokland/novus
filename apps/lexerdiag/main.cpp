#include "CLI/CLI.hpp"
#include "lex/lexer.hpp"
#include "lex/utilities.hpp"
#include "rang.hpp"
#include <chrono>
#include <fstream>
#include <iomanip>
#include <ios>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

using high_resolution_clock = std::chrono::high_resolution_clock;
using duration = std::chrono::duration<double>;

auto getFgColor(const lex::Token& token) -> rang::fg;
auto getBgColor(const lex::Token& token) -> rang::bg;

auto operator<<(std::ostream& out, const duration& rhs) -> std::ostream&;

template <typename InputItr> auto printTokens(InputItr inputBegin, const InputItr inputEnd) {
  const auto colWidth = 20;
  const auto totalWidth = 80;

  // Lex all the tokens and time how long it takes.
  auto t1 = high_resolution_clock::now();
  auto tokens = lex::lexAll(inputBegin, inputEnd);
  auto t2 = high_resolution_clock::now();
  auto lexDur = std::chrono::duration_cast<duration>(t2 - t1);

  std::cout << rang::style::dim << rang::style::italic;
  std::cout << std::string(totalWidth, '-') << '\n';
  std::cout << "Lexed " << tokens.size() << " tokens in " << lexDur << '\n';
  std::cout << std::string(totalWidth, '-') << '\n';
  std::cout << rang::style::reset;

  for (const auto& token : tokens) {
    std::cout << rang::style::bold << "* ";
    std::cout << getFgColor(token) << getBgColor(token);
    std::cout << std::setw(colWidth) << std::left << token.getType();

    std::stringstream spanStr;
    spanStr << '(' << token.getSpan().getStart() << " - " << token.getSpan().getEnd() << ')';

    std::cout << rang::style::dim << rang::fg::reset;
    std::cout << std::setw(colWidth) << std::right << spanStr.str();
    std::cout << rang::style::reset;

    const auto payload = token.getPayload();
    if (payload) {
      std::cout << " = " << *payload;
    }

    std::cout << rang::bg::reset << rang::fg::reset << '\n';
  }

  std::cout << rang::style::dim << std::string(totalWidth, '-') << '\n';
}

auto main(int argc, char** argv) -> int {
  auto app = CLI::App{"Lexer diagnostic tool"};
  app.require_subcommand(1);

  // Lex input characters.
  std::string lexInput;
  auto lexCmd = app.add_subcommand("lex", "Lex the provided characters.")->callback([&]() {
    printTokens(lexInput.begin(), lexInput.end());
  });
  lexCmd->add_option("input", lexInput, "Input characters to lex.")->required();

  // Lex input file.
  std::string lexFilePath;
  auto lexFileCmd = app.add_subcommand("lexfile", "Lex all characters in a file.")->callback([&]() {
    std::ifstream fs{lexFilePath};
    printTokens(std::istreambuf_iterator<char>{fs}, std::istreambuf_iterator<char>{});
  });
  lexFileCmd->add_option("file", lexFilePath, "Path to file to lex.")
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

auto getFgColor(const lex::Token& token) -> rang::fg {
  switch (token.getCat()) {
  case lex::TokenCat::Operator:
    return rang::fg::cyan;
  case lex::TokenCat::Seperator:
    return rang::fg::magenta;
  case lex::TokenCat::Literal:
    return rang::fg::yellow;
  case lex::TokenCat::Keyword:
    return rang::fg::blue;
  case lex::TokenCat::Identifier:
    return rang::fg::green;
  case lex::TokenCat::Error:
  case lex::TokenCat::Unknown:
    return rang::fg::reset;
  }
  return rang::fg::reset;
}

auto getBgColor(const lex::Token& token) -> rang::bg {
  switch (token.getCat()) {
  case lex::TokenCat::Operator:
  case lex::TokenCat::Seperator:
  case lex::TokenCat::Literal:
  case lex::TokenCat::Keyword:
  case lex::TokenCat::Identifier:
  case lex::TokenCat::Unknown:
    return rang::bg::reset;
  case lex::TokenCat::Error:
    return rang::bg::red;
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
