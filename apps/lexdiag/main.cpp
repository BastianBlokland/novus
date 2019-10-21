#include "CLI/CLI.hpp"
#include "lex/lexer.hpp"
#include "rang.hpp"

using high_resolution_clock = std::chrono::high_resolution_clock;
using duration              = std::chrono::duration<double>;

auto getFgColor(const lex::Token& token) -> rang::fg;
auto getBgColor(const lex::Token& token) -> rang::bg;

auto operator<<(std::ostream& out, const duration& rhs) -> std::ostream&;

template <typename InputItr>
auto run(InputItr inputBegin, const InputItr inputEnd, const bool printTokens) {
  const auto columnWidth = 20;
  const auto width       = 80;

  using s  = rang::style;
  using fg = rang::style;
  using bg = rang::style;

  // Lex all the tokens and time how long it takes.
  const auto t1     = high_resolution_clock::now();
  const auto tokens = lex::lexAll(inputBegin, inputEnd);
  const auto t2     = high_resolution_clock::now();
  const auto lexDur = std::chrono::duration_cast<duration>(t2 - t1);

  std::cout << s::dim << s::italic << std::string(width, '-') << '\n'
            << "Lexed " << tokens.size() << " tokens in " << lexDur << '\n'
            << std::string(width, '-') << '\n'
            << s::reset;

  if (printTokens) {
    for (const auto& token : tokens) {
      std::cout << s::bold << "* " << getFgColor(token) << getBgColor(token)
                << std::setw(columnWidth) << std::left << token.getType();

      std::stringstream spanStr;
      spanStr << '(' << token.getSpan().getStart() << " - " << token.getSpan().getEnd() << ')';

      std::cout << s::dim << fg::reset << std::setw(columnWidth) << std::right << spanStr.str()
                << s::reset;

      const auto payload = token.getPayload();
      if (payload) {
        std::cout << " > " << *payload;
      }

      std::cout << bg::reset << fg::reset << '\n';
    }
    std::cout << s::dim << std::string(width, '-') << '\n';
  }
}

auto main(int argc, char** argv) -> int {
  auto app = CLI::App{"Lexer diagnostic tool"};
  app.require_subcommand(1);

  auto printTokens = true;
  app.add_flag("!--skip-tokens", printTokens, "Skip printing the tokens")->capture_default_str();

  // Lex input characters.
  std::string charsInput;
  auto lexCmd = app.add_subcommand("lex", "Lex the provided characters")->callback([&]() {
    run(charsInput.begin(), charsInput.end(), printTokens);
  });
  lexCmd->add_option("input", charsInput, "Input characters to lex")->required();

  // Lex input file.
  std::string filePath;
  auto lexFileCmd = app.add_subcommand("lexfile", "Lex all characters in a file")->callback([&]() {
    std::ifstream fs{filePath};
    run(std::istreambuf_iterator<char>{fs}, std::istreambuf_iterator<char>{}, printTokens);
  });
  lexFileCmd->add_option("file", filePath, "Path to file to lex")
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
  using tc = lex::TokenCat;

  switch (token.getCat()) {
  case tc::Operator:
    return rang::fg::cyan;
  case tc::Seperator:
    return rang::fg::magenta;
  case tc::Literal:
    return rang::fg::yellow;
  case tc::Keyword:
    return rang::fg::blue;
  case tc::Identifier:
    return rang::fg::green;
  case tc::Error:
  case tc::Unknown:
    return rang::fg::reset;
  }
  return rang::fg::reset;
}

auto getBgColor(const lex::Token& token) -> rang::bg {
  using tc = lex::TokenCat;

  switch (token.getCat()) {
  case tc::Operator:
  case tc::Seperator:
  case tc::Literal:
  case tc::Keyword:
  case tc::Identifier:
  case tc::Unknown:
    return rang::bg::reset;
  case tc::Error:
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
