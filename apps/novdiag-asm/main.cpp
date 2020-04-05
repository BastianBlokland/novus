#include "CLI/CLI.hpp"
#include "backend/generator.hpp"
#include "filesystem.hpp"
#include "frontend/analysis.hpp"
#include "frontend/output.hpp"
#include "frontend/source.hpp"
#include "input/char_escape.hpp"
#include "input/search_paths.hpp"
#include "novasm/assembly.hpp"
#include "novasm/disassembler.hpp"
#include "opt/opt.hpp"
#include "rang.hpp"
#include <chrono>
#include <optional>

using Clock    = std::chrono::high_resolution_clock;
using Duration = std::chrono::duration<double>;

auto operator<<(std::ostream& out, const Duration& rhs) -> std::ostream&;

auto printStringLiterals(const novasm::Assembly& assembly) -> void {
  const auto idColWidth = 5;

  std::cout << rang::style::bold << "StringLiterals:\n" << rang::style::reset;
  auto id = 0U;
  for (auto itr = assembly.beginLitStrings(); itr != assembly.endLitStrings(); ++itr, ++id) {
    std::cout << "  " << rang::style::bold << std::setw(idColWidth) << std::left << id
              << rang::style::reset << " \"" << input::escape(*itr) << '"' << '\n';
  }
}

auto printEntrypoint(const novasm::Assembly& assembly) -> void {
  std::cout << rang::style::bold << "Entrypoint: " << assembly.getEntrypoint() << rang::style::reset
            << '\n';
}

auto printInstructions(const novasm::Assembly& assembly, const backend::InstructionLabels& labels)
    -> void {
  const auto ipOffsetColWidth = 6;
  const auto opCodeColWidth   = 20;

  std::cout << rang::style::bold << "Instructions:\n" << rang::style::reset;
  auto instructions = novasm::disassembleInstructions(assembly, labels);
  for (const auto& instr : instructions) {
    // Print any labels associated with this instruction.
    for (const auto& label : instr.getLabels()) {
      std::cout << rang::style::bold << rang::fg::yellow << label << rang::style::reset << '\n';
    }

    // Print ip offset and op-code.
    std::cout << "  " << rang::style::dim << std::setw(ipOffsetColWidth) << std::left
              << instr.getIpOffset() << rang::style::reset << rang::style::bold
              << std::setw(opCodeColWidth) << std::left << instr.getOp() << rang::style::reset;

    // Print all the arguments.
    for (const auto& arg : instr.getArgs()) {
      // Print argument value.
      novasm::dasm::operator<<(std::cout, arg);
      std::cout << ' ';

      // Print argument labels.
      const auto& argLabels = arg.getLabels();
      if (!argLabels.empty()) {
        std::cout << '[';
        for (auto i = 0U; i != argLabels.size(); ++i) {
          if (i != 0U) {
            std::cout << ',';
          }
          std::cout << rang::fg::yellow << rang::style::bold << argLabels[i] << rang::style::reset;
        }
        std::cout << "]";
      }
      std::cout << ' ';
    }
    std::cout << '\n' << rang::style::reset;
  }
}

auto printAssembly(const novasm::Assembly& assembly, const backend::InstructionLabels& labels)
    -> void {
  printEntrypoint(assembly);
  std::cout << '\n';
  printStringLiterals(assembly);
  std::cout << '\n';
  printInstructions(assembly, labels);
}

template <typename InputItr>
auto run(
    const std::string& inputId,
    std::optional<filesystem::path> inputPath,
    const std::vector<filesystem::path>& searchPaths,
    InputItr inputBegin,
    const InputItr inputEnd,
    const bool outputProgram,
    const bool optimize) {
  const auto width = 80;

  // Generate an assembly file and time how long it takes.
  const auto t1  = Clock::now();
  const auto src = frontend::buildSource(inputId, std::move(inputPath), inputBegin, inputEnd);
  const auto frontendOutput = frontend::analyze(src, searchPaths);

  if (!frontendOutput.isSuccess()) {
    // Print diagnostics.
    for (auto diagItr = frontendOutput.beginDiags(); diagItr != frontendOutput.endDiags();
         ++diagItr) {
      std::cerr << rang::style::bold << rang::bg::red << *diagItr << rang::bg::reset << '\n'
                << rang::style::reset;
    }
    return 1;
  }

  const auto asmOutput = optimize ? backend::generate(opt::optimize(frontendOutput.getProg()))
                                  : backend::generate(opt::treeshake(frontendOutput.getProg()));
  const auto t2     = Clock::now();
  const auto genDur = std::chrono::duration_cast<Duration>(t2 - t1);

  std::cout << rang::style::dim << rang::style::italic << std::string(width, '-') << '\n'
            << "Generated assembly in " << genDur << '\n'
            << std::string(width, '-') << '\n'
            << rang::style::reset;

  if (outputProgram) {
    printAssembly(asmOutput.first, asmOutput.second);
    std::cout << rang::style::dim << std::string(width, '-') << '\n';
  }
  return 0;
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
  auto exitcode = 0;
  auto app      = CLI::App{"Novus assembly diagnostic tool"};
  app.require_subcommand(1);

  const auto searchPaths = input::getSearchPaths(argv);

  auto colorMode   = rang::control::Auto;
  auto printOutput = true;
  auto optimize    = false;

  app.add_flag(
      "--no-color{0},-c{2},--color{2},--auto-color{1}",
      colorMode,
      "Set the color output behaviour");

  // Analyze assembly for input characters.
  std::string input;
  auto analyzeCmd = app.add_subcommand("analyze", "Analyze assembly for the provided characters")
                        ->callback([&]() {
                          rang::setControlMode(colorMode);
                          exitcode =
                              run("inline",
                                  std::nullopt,
                                  searchPaths,
                                  input.begin(),
                                  input.end(),
                                  printOutput,
                                  optimize);
                        });
  analyzeCmd->add_option("input", input, "Input characters")->required();
  analyzeCmd->add_flag("!--no-output", printOutput, "Skip printing the program");
  analyzeCmd->add_flag("-o,--optimize", optimize, "Optimize program");

  // Analyze assembly for the input file.
  filesystem::path filePath;
  auto analyzeFileCmd =
      app.add_subcommand("analyzefile", "Analyze assembly for the provided file")->callback([&]() {
        rang::setControlMode(colorMode);

        auto absFilePath = filesystem::absolute(filePath);
        std::ifstream fs{filePath};
        exitcode =
            run(filePath.filename(),
                absFilePath,
                searchPaths,
                std::istreambuf_iterator<char>{fs},
                std::istreambuf_iterator<char>{},
                printOutput,
                optimize);
      });
  analyzeFileCmd->add_option("file", filePath, "Path to file")
      ->check(CLI::ExistingFile)
      ->required();
  analyzeFileCmd->add_flag("!--no-output", printOutput, "Skip printing the program");
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
