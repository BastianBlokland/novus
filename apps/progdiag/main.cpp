#include "CLI/CLI.hpp"
#include "frontend/analysis.hpp"
#include "frontend/source.hpp"
#include "rang.hpp"

using high_resolution_clock = std::chrono::high_resolution_clock;
using duration              = std::chrono::duration<double>;

auto operator<<(std::ostream& out, const duration& rhs) -> std::ostream&;

auto printTypeDecls(const prog::Program& prog) -> void {
  const auto idColWidth = 5;

  std::cout << rang::style::bold << "Types:\n" << rang::style::reset;
  for (auto typeItr = prog.beginTypeDecls(); typeItr != prog.endTypeDecls(); ++typeItr) {
    const auto& typeDecl = *typeItr;
    std::stringstream idStr;
    idStr << typeDecl.getId();

    std::cout << " * " << std::setw(idColWidth) << std::left << idStr.str();

    switch (typeDecl.getKind()) {
    case prog::sym::TypeKind::Intrinsic:
      std::cout << rang::fg::yellow;
      break;
    }
    std::cout << "  " << rang::style::bold << typeDecl.getName() << rang::style::reset << '\n';
  }
}

auto printFuncDecls(const prog::Program& prog) -> void {
  const auto idColWidth    = 5;
  const auto nameColWidth  = 20;
  const auto inputColWidth = 20;

  std::cout << rang::style::bold << "Functions:\n" << rang::style::reset;
  for (auto funcItr = prog.beginFuncDecls(); funcItr != prog.endFuncDecls(); ++funcItr) {
    const auto& funcDecl = *funcItr;
    std::stringstream idStr;
    idStr << funcDecl.getId();

    std::cout << " * " << std::setw(idColWidth) << std::left << idStr.str();

    switch (funcDecl.getKind()) {
    case prog::sym::FuncKind::Intrinsic:
      std::cout << rang::fg::yellow;
      break;
    case prog::sym::FuncKind::User:
      std::cout << rang::fg::green;
      break;
    }
    std::cout << "  " << rang::style::bold << std::setw(nameColWidth) << std::left
              << funcDecl.getName() << rang::style::reset << "  ";

    std::stringstream inputStr;
    inputStr << "(" << funcDecl.getSig().getInput() << ")";
    std::cout << " " << std::setw(inputColWidth) << std::left << inputStr.str() << " -> "
              << funcDecl.getSig().getOutput() << '\n';
  }
}

auto printProgram(const prog::Program& prog) -> void {
  printTypeDecls(prog);
  std::cout << '\n';
  printFuncDecls(prog);
}

template <typename InputItr>
auto run(
    const std::string& inputId,
    InputItr inputBegin,
    const InputItr inputEnd,
    const bool outputProgram) {
  const auto width = 80;

  // Analyze the input and time how long it takes.
  const auto t1       = high_resolution_clock::now();
  const auto src      = frontend::buildSource(inputId, inputBegin, inputEnd);
  const auto output   = frontend::analyze(src);
  const auto t2       = high_resolution_clock::now();
  const auto parseDur = std::chrono::duration_cast<duration>(t2 - t1);

  std::cout << rang::style::dim << rang::style::italic << std::string(width, '-') << '\n'
            << "Analyzed " << src.getCharCount() << " chars in " << parseDur << '\n'
            << std::string(width, '-') << '\n'
            << rang::style::reset;

  for (auto diagItr = output.beginDiags(); diagItr != output.endDiags(); ++diagItr) {
    const auto& diag = *diagItr;
    switch (diag.getSeverity()) {
    case frontend::DiagSeverity::Warning:
      std::cout << rang::style::bold << rang::bg::yellow << diag << rang::bg::reset << '\n'
                << rang::style::reset;
      break;
    case frontend::DiagSeverity::Error:
      std::cout << rang::style::bold << rang::bg::red << diag << rang::bg::reset << '\n'
                << rang::style::reset;
      break;
    }
  }

  if (output.isSuccess() && outputProgram) {
    printProgram(output.getProg());
    std::cout << rang::style::dim << std::string(width, '-') << '\n';
  }
}

auto main(int argc, char** argv) -> int {
  auto app = CLI::App{"Program diagnostic tool"};
  app.require_subcommand(1);

  auto printOutput = true;
  app.add_flag("!--skip-output", printOutput, "Skip printing the program")->capture_default_str();

  // Analyze input characters.
  std::string charsInput;
  auto lexCmd = app.add_subcommand("analyze", "Analyze the provided characters")->callback([&]() {
    run("inline", charsInput.begin(), charsInput.end(), printOutput);
  });
  lexCmd->add_option("input", charsInput, "Input characters to analyze")->required();

  // Analyze input file.
  std::string filePath;
  auto lexFileCmd = app.add_subcommand("analyzefile", "Analyze the provided file")->callback([&]() {
    std::ifstream fs{filePath};
    run(filePath,
        std::istreambuf_iterator<char>{fs},
        std::istreambuf_iterator<char>{},
        printOutput);
  });
  lexFileCmd->add_option("file", filePath, "Path to file to analyze")
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
