#include "CLI/CLI.hpp"
#include "frontend/analysis.hpp"
#include "frontend/source.hpp"
#include "get_expr_color.hpp"
#include "input/char_escape.hpp"
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

  std::cout << rang::style::bold << exprCol.getFgColor() << input::escape(n.toString()) << ' '
            << rang::fg::reset << rang::style::dim << n.getType() << rang::style::reset << '\n';

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
    std::cout << "  " << rang::fg::yellow << rang::style::bold << typeDecl.getName()
              << rang::style::reset << '\n';
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
    case prog::sym::FuncKind::User:
      std::cout << rang::fg::green;
      break;
    default:
      std::cout << rang::fg::yellow;
      break;
    }
    std::cout << "  " << rang::style::bold << std::setw(nameColWidth) << std::left
              << funcDecl.getName() << rang::style::reset;

    std::stringstream inputStr;
    inputStr << "(" << funcDecl.getInput() << ")";
    std::cout << std::setw(inputColWidth) << std::left << inputStr.str() << " -> "
              << funcDecl.getOutput() << '\n';
  }
}

auto printActionDecls(const prog::Program& prog) -> void {
  const auto idColWidth   = 5;
  const auto nameColWidth = 20;

  std::cout << rang::style::bold << "Action declarations:\n" << rang::style::reset;
  for (auto actionItr = prog.beginActionDecls(); actionItr != prog.endActionDecls(); ++actionItr) {
    const auto& actionDecl = *actionItr;
    std::stringstream idStr;
    idStr << actionDecl.getId();

    std::cout << " * " << std::setw(idColWidth) << std::left << idStr.str();
    std::cout << "  " << rang::fg::yellow << rang::style::bold << std::setw(nameColWidth)
              << std::left << actionDecl.getName() << rang::style::reset << '('
              << actionDecl.getInput() << ")\n";
  }
}

auto printTypeDefs(const prog::Program& prog) -> void {
  const auto nameColWidth = 15;

  std::cout << rang::style::bold << "Type definitions:\n" << rang::style::reset;
  for (auto typeItr = prog.beginTypeDefs(); typeItr != prog.endTypeDefs(); ++typeItr) {
    if (typeItr != prog.beginTypeDefs()) {
      std::cout << "\n";
    }

    const auto typeId    = typeItr->first;
    const auto& typeDecl = prog.getTypeDecl(typeId);
    const auto& typeDef  = typeItr->second;
    std::cout << " " << rang::style::bold << typeDecl.getName() << rang::style::dim << " ("
              << typeDecl.getKind() << ") " << typeId << rang::style::reset << "\n";

    // Print struct fields.
    if (std::holds_alternative<prog::sym::StructDef>(typeDef)) {
      const auto& structDef = std::get<prog::sym::StructDef>(typeDef);
      for (const auto& field : structDef.getFields()) {
        const auto& typeName = prog.getTypeDecl(field.getType()).getName();
        std::cout << "  " << rang::fg::yellow << rang::style::bold << std::setw(nameColWidth)
                  << std::left << typeName << rang::style::reset << field.getName() << '\n';
      }
    }
  }
}

auto printConsts(const prog::sym::ConstDeclTable& consts) -> void {
  const auto colWidth = 10;
  for (const auto& c : consts) {
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
    std::cout << rang::style::bold << "   * " << std::setw(colWidth) << std::left << idStr.str()
              << "  " << c.getName() << ' ' << rang::style::reset << rang::style::dim << c.getType()
              << rang::style::reset << '\n';
  }
}

auto printFuncDefs(const prog::Program& prog) -> void {
  std::cout << rang::style::bold << "Function definitions:\n" << rang::style::reset;
  for (auto funcItr = prog.beginFuncDefs(); funcItr != prog.endFuncDefs(); ++funcItr) {
    if (funcItr != prog.beginFuncDefs()) {
      std::cout << "\n";
    }

    const auto funcId   = funcItr->first;
    const auto& funcDef = funcItr->second;
    const auto funcName = prog.getFuncDecl(funcId).getName();
    std::cout << " " << rang::style::bold << funcName << rang::style::dim << " " << funcId
              << rang::style::reset << "\n";

    const auto& consts = funcDef.getConsts();
    if (consts.begin() != consts.end()) {
      std::cout << rang::style::italic << "  Consts:\n" << rang::style::reset;
      printConsts(consts);
    }

    std::cout << rang::style::italic << "  Body:\n" << rang::style::reset;
    printExpr(funcDef.getExpr(), "   ");
  }
}

auto printExecStmts(const prog::Program& prog) -> void {
  std::cout << rang::style::bold << "Execute statements:\n" << rang::style::reset;
  for (auto execItr = prog.beginExecStmts(); execItr != prog.endExecStmts(); ++execItr) {
    const auto actionId   = execItr->getActionId();
    const auto actionName = prog.getActionDecl(actionId).getName();
    std::cout << " " << rang::style::bold << actionName << rang::style::dim << " " << actionId
              << rang::style::reset << "\n";

    const auto& consts = execItr->getConsts();
    if (consts.begin() != consts.end()) {
      std::cout << rang::style::italic << "  Consts:\n" << rang::style::reset;
      printConsts(consts);
    }

    const auto& args = execItr->getArgs();
    if (args.begin() != args.end()) {
      std::cout << rang::style::italic << "  Args:\n" << rang::style::reset;
      for (const auto& arg : args) {
        printExpr(*arg, "   ");
      }
    }
    std::cout << '\n';
  }
}

auto printProgram(const prog::Program& prog) -> void {
  printTypeDecls(prog);
  std::cout << '\n';
  printFuncDecls(prog);
  std::cout << '\n';
  printActionDecls(prog);
  if (prog.beginTypeDefs() != prog.endTypeDefs()) {
    std::cout << '\n';
    printTypeDefs(prog);
  }
  if (prog.beginFuncDefs() != prog.endFuncDefs()) {
    std::cout << '\n';
    printFuncDefs(prog);
  }
  if (prog.beginExecStmts() != prog.endExecStmts()) {
    std::cout << '\n';
    printExecStmts(prog);
  }
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
  auto analyzeCmd =
      app.add_subcommand("analyze", "Analyze the provided characters")->callback([&]() {
        progdiag::run("inline", charsInput.begin(), charsInput.end(), printOutput);
      });
  analyzeCmd->add_option("input", charsInput, "Input characters to analyze")->required();

  // Analyze input file.
  std::string filePath;
  auto analyzeFileCmd =
      app.add_subcommand("analyzefile", "Analyze the provided file")->callback([&]() {
        std::ifstream fs{filePath};
        progdiag::run(
            filePath,
            std::istreambuf_iterator<char>{fs},
            std::istreambuf_iterator<char>{},
            printOutput);
      });
  analyzeFileCmd->add_option("file", filePath, "Path to file to analyze")
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
