#include "CLI/CLI.hpp"
#include "lex/lexer.hpp"
#include "rang.hpp"
#include <chrono>

using Clock    = std::chrono::high_resolution_clock;
using Duration = std::chrono::duration<double>;

auto getFgColor(const lex::Token& token) -> rang::fg;
auto getBgColor(const lex::Token& token) -> rang::bg;

auto operator<<(std::ostream& out, const Duration& rhs) -> std::ostream&;

template <typename InputItr>
auto run(InputItr inputBegin, const InputItr inputEnd, const bool outputTokens) {
  const auto columnWidth = 20;
  const auto width       = 80;

  using S = rang::style;

  // Lex all the tokens and time how long it takes.
  const auto t1     = Clock::now();
  const auto tokens = lex::lexAll(inputBegin, inputEnd);
  const auto t2     = Clock::now();
  const auto lexDur = std::chrono::duration_cast<Duration>(t2 - t1);

  std::cout << S::dim << S::italic << std::string(width, '-') << '\n'
            << "Lexed " << tokens.size() << " tokens in " << lexDur << '\n'
            << std::string(width, '-') << '\n'
            << S::reset;

  if (outputTokens) {
    for (const auto& token : tokens) {
      std::cout << S::bold << "* " << getFgColor(token) << getBgColor(token)
                << std::setw(columnWidth) << std::left << token.getKind();

      std::stringstream spanStr;
      spanStr << '(' << token.getSpan().getStart() << " - " << token.getSpan().getEnd() << ')';

      std::cout << S::dim << S::reset << std::setw(columnWidth) << std::right << spanStr.str()
                << S::reset;

      const auto payload = token.getPayload();
      if (payload) {
        std::cout << " > " << *payload;
      }

      std::cout << S::reset << '\n';
    }
    std::cout << S::dim << std::string(width, '-') << '\n';
  }
}

auto getFgColor(const lex::Token& token) -> rang::fg {
  using Tc = lex::TokenCat;

  switch (token.getCat()) {
  case Tc::Operator:
    return rang::fg::cyan;
  case Tc::Seperator:
    return rang::fg::magenta;
  case Tc::Literal:
    return rang::fg::yellow;
  case Tc::Keyword:
    return rang::fg::blue;
  case Tc::Identifier:
  case Tc::Comment:
    return rang::fg::green;
  case Tc::Error:
  case Tc::Unknown:
    return rang::fg::reset;
  }
  return rang::fg::reset;
}

auto getBgColor(const lex::Token& token) -> rang::bg {
  using Tc = lex::TokenCat;

  switch (token.getCat()) {
  case Tc::Operator:
  case Tc::Seperator:
  case Tc::Literal:
  case Tc::Keyword:
  case Tc::Identifier:
  case Tc::Comment:
  case Tc::Unknown:
    return rang::bg::reset;
  case Tc::Error:
    return rang::bg::red;
  }
  return rang::bg::reset;
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
  auto app = CLI::App{"Novus lexer diagnostic tool"};
  app.require_subcommand(1);

  auto colorMode   = rang::control::Auto;
  auto printOutput = true;

  app.add_flag(
      "--no-color{0},-c{2},--color{2},--auto-color{1}",
      colorMode,
      "Set the color output behaviour");

  // Lex input characters.
  std::string charsInput;
  auto analyzeCmd = app.add_subcommand("analyze", "Lex the provided characters")->callback([&]() {
    rang::setControlMode(colorMode);

    run(charsInput.begin(), charsInput.end(), printOutput);
  });
  analyzeCmd->add_option("input", charsInput, "Input characters")->required();
  analyzeCmd->add_flag("!--no-output", printOutput, "Skip printing the tokens");

  // Lex input file.
  std::string filePath;
  auto analyzeFileCmd =
      app.add_subcommand("analyzefile", "Lex all characters in a file")->callback([&]() {
        rang::setControlMode(colorMode);

        std::ifstream fs{filePath};
        run(std::istreambuf_iterator<char>{fs}, std::istreambuf_iterator<char>{}, printOutput);
      });
  analyzeFileCmd->add_option("file", filePath, "Path to file")
      ->check(CLI::ExistingFile)
      ->required();
  analyzeFileCmd->add_flag("!--no-output", printOutput, "Skip printing the tokens");

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
