#include "CLI/CLI.hpp"
#include "frontend/analysis.hpp"
#include "frontend/source.hpp"
#include "get_expr_color.hpp"
#include "rang.hpp"

namespace progdiag {

using high_resolution_clock = std::chrono::high_resolution_clock;
using duration              = std::chrono::duration<double>;

auto operator<<(std::ostream& out, const duration& rhs) -> std::ostream&;

auto printExpr(
    const prog::expr::Node& n,
    std::string prefix = "",
    bool isRoot        = true,
    bool isLastSibling = false) -> void {

  const static auto cross  = " ├─";
  const static auto corner = " └─";
  const static auto vert   = " │ ";
  const static auto indent = "   ";

  if (isRoot) {
    std::cout << prefix << rang::style::italic << "* ";
    prefix += "  ";
  } else {
    std::cout << rang::style::dim << prefix;
    if (isLastSibling) {
      std::cout << corner;
      prefix += indent;
    } else {
      std::cout << cross;
      prefix += vert;
    }
    std::cout << rang::style::reset;
  }

  auto exprCol = GetExprColor{};
  n.accept(&exprCol);

  std::cout << rang::style::bold << exprCol.getFgColor() << n << ' ' << rang::fg::reset
            << rang::style::dim << n.getType() << rang::style::reset << '\n';

  const auto childCount = n.getChildCount();
  for (auto i = 0U; i < childCount; ++i) {
    printExpr(n[i], prefix, false, i == (childCount - 1));
  }
}

auto printTypeDecls(const prog::Program& prog) -> void {
  const auto idColWidth = 5;

  std::cout << rang::style::bold << "Type declarations:\n" << rang::style::reset;
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

  std::cout << rang::style::bold << "Function declarations:\n" << rang::style::reset;
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

auto printFuncDefs(const prog::Program& prog) -> void {
  const auto constIdColWidth = 10;

  std::cout << rang::style::bold << "Function definitions:\n" << rang::style::reset;
  for (auto funcItr = prog.beginFuncDefs(); funcItr != prog.endFuncDefs(); ++funcItr) {
    const auto funcId   = funcItr->getId();
    const auto funcName = prog.getFuncDecl(funcId).getName();
    std::cout << " " << rang::style::bold << funcName << rang::style::dim << " " << funcId
              << rang::style::reset << "\n";

    std::cout << rang::style::italic << "  Consts:\n" << rang::style::reset;
    for (const auto& c : funcItr->getConsts()) {
      switch (c.getKind()) {
      case prog::sym::ConstKind::Input:
        std::cout << rang::fg::blue;
        break;
      case prog::sym::ConstKind::Local:
        std::cout << rang::fg::green;
        break;
      }
      std::stringstream idStr;
      idStr << c.getId() << ": " << c.getKind();
      std::cout << rang::style::bold << "   * " << std::setw(constIdColWidth) << std::left
                << idStr.str() << "  " << c.getName() << ' ' << rang::style::reset
                << rang::style::dim << c.getType() << rang::style::reset << '\n';
    }

    // Print expression.
    std::cout << rang::style::italic << "  Body:\n" << rang::style::reset;
    printExpr(funcItr->getExpr(), "   ");
    std::cout << '\n';
  }
}

auto printProgram(const prog::Program& prog) -> void {
  printTypeDecls(prog);
  std::cout << '\n';
  printFuncDecls(prog);
  std::cout << '\n';
  printFuncDefs(prog);
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

} // namespace progdiag

auto main(int argc, char** argv) -> int {
  auto app = CLI::App{"Program diagnostic tool"};
  app.require_subcommand(1);

  auto printOutput = true;
  app.add_flag("!--skip-output", printOutput, "Skip printing the program")->capture_default_str();

  // Analyze input characters.
  std::string charsInput;
  auto lexCmd = app.add_subcommand("analyze", "Analyze the provided characters")->callback([&]() {
    progdiag::run("inline", charsInput.begin(), charsInput.end(), printOutput);
  });
  lexCmd->add_option("input", charsInput, "Input characters to analyze")->required();

  // Analyze input file.
  std::string filePath;
  auto lexFileCmd = app.add_subcommand("analyzefile", "Analyze the provided file")->callback([&]() {
    std::ifstream fs{filePath};
    progdiag::run(
        filePath,
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
