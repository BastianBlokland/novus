#include "CLI/CLI.hpp"
#include "filesystem.hpp"
#include "frontend/analysis.hpp"
#include "frontend/source.hpp"
#include "get_expr_color.hpp"
#include "input/char_escape.hpp"
#include "opt/opt.hpp"
#include "rang.hpp"
#include <chrono>

namespace progdiag {

using Clock    = std::chrono::high_resolution_clock;
using Duration = std::chrono::duration<double>;

auto operator<<(std::ostream& out, const Duration& rhs) -> std::ostream&;

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
  const auto idColWidth = 10;

  std::cout << rang::style::bold << "Type declarations:\n" << rang::style::reset;
  for (auto typeItr = prog.beginTypeDecls(); typeItr != prog.endTypeDecls(); ++typeItr) {
    const auto& typeDecl = typeItr->second;
    std::stringstream idStr;
    idStr << typeDecl.getId();

    std::cout << " * " << std::setw(idColWidth) << std::left << idStr.str();
    std::cout << "  " << rang::fg::yellow << rang::style::bold << typeDecl.getName()
              << rang::style::reset << '\n';
  }
}

auto printFuncDecls(const prog::Program& prog) -> void {
  const auto idColWidth    = 10;
  const auto nameColWidth  = 30;
  const auto inputColWidth = 25;

  std::cout << rang::style::bold << "Function declarations:\n" << rang::style::reset;
  for (auto funcItr = prog.beginFuncDecls(); funcItr != prog.endFuncDecls(); ++funcItr) {
    const auto& funcDecl = funcItr->second;
    std::stringstream idStr;
    idStr << funcDecl.getId();

    std::cout << " * " << std::setw(idColWidth) << std::left << idStr.str();

    switch (funcDecl.getKind()) {
    case prog::sym::FuncKind::User:
      std::cout << (funcDecl.isAction() ? rang::fg::blue : rang::fg::green);
      break;
    default:
      std::cout << (funcDecl.isAction() ? rang::fg::magenta : rang::fg::yellow);
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

    // Print type content.
    if (std::holds_alternative<prog::sym::StructDef>(typeDef)) {
      const auto& structDef = std::get<prog::sym::StructDef>(typeDef);
      for (const auto& field : structDef.getFields()) {
        const auto& typeName = prog.getTypeDecl(field.getType()).getName();
        std::cout << "  " << rang::fg::yellow << rang::style::bold << std::setw(nameColWidth)
                  << std::left << typeName << rang::style::reset << field.getName() << '\n';
      }
    } else if (std::holds_alternative<prog::sym::UnionDef>(typeDef)) {
      const auto& unionDef = std::get<prog::sym::UnionDef>(typeDef);
      for (const auto& type : unionDef.getTypes()) {
        const auto& typeName = prog.getTypeDecl(type).getName();
        std::cout << "  " << rang::fg::yellow << rang::style::bold << std::setw(nameColWidth)
                  << std::left << typeName << rang::style::reset << '\n';
      }
    } else if (std::holds_alternative<prog::sym::EnumDef>(typeDef)) {
      const auto& enumDef = std::get<prog::sym::EnumDef>(typeDef);
      for (const auto& entry : enumDef) {
        std::cout << "  " << rang::fg::yellow << rang::style::bold << std::setw(nameColWidth)
                  << std::left << entry.first << rang::style::reset << entry.second << '\n';
      }
    } else if (std::holds_alternative<prog::sym::DelegateDef>(typeDef)) {
      const auto& delegateDef = std::get<prog::sym::DelegateDef>(typeDef);
      std::stringstream inputStr;
      inputStr << "(" << delegateDef.getInput() << ")";
      std::cout << "  " << rang::fg::yellow << rang::style::bold << std::setw(nameColWidth)
                << std::left << inputStr.str() << " -> " << delegateDef.getOutput()
                << rang::style::reset << '\n';
    } else if (std::holds_alternative<prog::sym::FutureDef>(typeDef)) {
      const auto& futureDef = std::get<prog::sym::FutureDef>(typeDef);
      std::cout << "  " << rang::fg::yellow << rang::style::bold << std::setw(nameColWidth)
                << std::left << "future"
                << " -> " << futureDef.getResult() << rang::style::reset << '\n';
    }
  }
}

auto printConsts(const prog::sym::ConstDeclTable& consts) -> void {
  const auto colWidth = 10;
  for (const auto& c : consts) {
    switch (c.getKind()) {
    case prog::sym::ConstKind::Bound:
      std::cout << rang::fg::magenta;
      break;
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

    const auto funcId    = funcItr->first;
    const auto& funcDef  = funcItr->second;
    const auto& funcDecl = prog.getFuncDecl(funcId);
    const auto& funcName = funcDecl.getName();
    std::cout << (funcDecl.isAction() ? " action " : " ");
    std::cout << rang::style::bold << funcName << rang::style::dim << " " << funcId
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
    const auto& consts = execItr->getConsts();
    if (consts.begin() != consts.end()) {
      std::cout << rang::style::italic << "  Consts:\n" << rang::style::reset;
      printConsts(consts);
    }

    std::cout << rang::style::italic << "  Body:\n" << rang::style::reset;
    printExpr(execItr->getExpr(), "   ");
    std::cout << '\n';
  }
}

auto printProgram(const prog::Program& prog) -> void {
  printTypeDecls(prog);
  std::cout << '\n';
  printFuncDecls(prog);
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
    std::optional<filesystem::path> inputPath,
    const std::vector<filesystem::path>& searchPaths,
    InputItr inputBegin,
    const InputItr inputEnd,
    const bool outputProgram,
    const bool treeshake,
    const bool optimize) {
  const auto width = 80;

  // Analyze the input and time how long it takes.
  const auto t1       = Clock::now();
  const auto src      = frontend::buildSource(inputId, std::move(inputPath), inputBegin, inputEnd);
  const auto output   = frontend::analyze(src, searchPaths);
  const auto t2       = Clock::now();
  const auto parseDur = std::chrono::duration_cast<Duration>(t2 - t1);

  std::cout << rang::style::dim << rang::style::italic << std::string(width, '-') << '\n'
            << "Analyzed in " << parseDur << '\n'
            << std::string(width, '-') << '\n'
            << rang::style::reset;

  for (auto diagItr = output.beginDiags(); diagItr != output.endDiags(); ++diagItr) {
    const auto& diag = *diagItr;
    switch (diag.getSeverity()) {
    case frontend::DiagSeverity::Warning:
      std::cerr << rang::style::bold << rang::bg::yellow << diag << rang::bg::reset << '\n'
                << rang::style::reset;
      break;
    case frontend::DiagSeverity::Error:
      std::cerr << rang::style::bold << rang::bg::red << diag << rang::bg::reset << '\n'
                << rang::style::reset;
      break;
    }
  }

  if (output.isSuccess() && outputProgram) {
    if (optimize) {
      printProgram(opt::optimize(output.getProg()));
    } else if (treeshake) {
      printProgram(opt::treeshake(output.getProg()));
    } else {
      printProgram(output.getProg());
    }
    std::cout << rang::style::dim << std::string(width, '-') << '\n';
  }
  return output.isSuccess() ? 0 : 1;
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

} // namespace progdiag

auto getSearchPaths(char** argv) {
  auto result = std::vector<filesystem::path>{};

  // Add the path to the binary.
  result.push_back(filesystem::absolute(argv[0]).parent_path());

  return result;
}

auto main(int argc, char** argv) -> int {
  auto exitcode = 0;
  auto app      = CLI::App{"Program diagnostic tool"};
  app.require_subcommand(1);

  auto colorMode   = rang::control::Auto;
  auto printOutput = true;
  auto treeshake   = true;
  auto optimize    = false;

  app.add_flag(
      "--no-color{0},-c{2},--color{2},--auto-color{1}",
      colorMode,
      "Set the color output behaviour");

  // Analyze input characters.
  std::string input;
  auto analyzeCmd =
      app.add_subcommand("analyze", "Analyze the provided characters")->callback([&]() {
        rang::setControlMode(colorMode);

        exitcode = progdiag::run(
            "inline",
            std::nullopt,
            getSearchPaths(argv),
            input.begin(),
            input.end(),
            printOutput,
            treeshake,
            optimize);
      });
  analyzeCmd->add_option("input", input, "Input characters to analyze")->required();
  analyzeCmd->add_flag("!--no-output", printOutput, "Skip printing the program");
  analyzeCmd->add_flag("!--no-treeshake", treeshake, "Don't remove unused types and functions");
  analyzeCmd->add_flag("-o,--optimize", optimize, "Optimize program");

  // Analyze input file.
  filesystem::path filePath;
  auto analyzeFileCmd =
      app.add_subcommand("analyzefile", "Analyze the provided file")->callback([&]() {
        rang::setControlMode(colorMode);

        auto absFilePath = filesystem::absolute(filePath);
        std::ifstream fs{filePath};
        exitcode = progdiag::run(
            filePath.filename(),
            absFilePath,
            getSearchPaths(argv),
            std::istreambuf_iterator<char>{fs},
            std::istreambuf_iterator<char>{},
            printOutput,
            treeshake,
            optimize);
      });
  analyzeFileCmd->add_option("file", filePath, "Path to file to analyze")
      ->check(CLI::ExistingFile)
      ->required();
  analyzeFileCmd->add_flag("!--no-output", printOutput, "Skip printing the program");
  analyzeFileCmd->add_flag("!--no-treeshake", treeshake, "Don't remove unused types and functions");
  analyzeFileCmd->add_flag("-o,--optimize", optimize, "Optimize program");

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
  return exitcode;
}
