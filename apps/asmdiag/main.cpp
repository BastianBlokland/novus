#include "CLI/CLI.hpp"
#include "backend/dasm/disassembler.hpp"
#include "backend/generator.hpp"
#include "frontend/analysis.hpp"
#include "frontend/output.hpp"
#include "frontend/source.hpp"
#include "input/char_escape.hpp"
#include "rang.hpp"
#include "vm/assembly.hpp"
#include <optional>

namespace asmdiag {

using high_resolution_clock = std::chrono::high_resolution_clock;
using duration              = std::chrono::duration<double>;

auto operator<<(std::ostream& out, const duration& rhs) -> std::ostream&;

static auto genAssembly(const frontend::Output& frontendOutput) -> std::optional<vm::Assembly> {
  if (!frontendOutput.isSuccess()) {
    return std::nullopt;
  }
  return backend::generate(frontendOutput.getProg());
}

auto printStringLiterals(const vm::Assembly& assembly) -> void {
  const auto idColWidth = 5;

  std::cout << rang::style::bold << "StringLiterals:\n" << rang::style::reset;
  auto id = 0U;
  for (auto itr = assembly.beginLitStrings(); itr != assembly.endLitStrings(); ++itr, ++id) {
    std::cout << "  " << rang::style::bold << std::setw(idColWidth) << std::left << id
              << rang::style::reset << " \"" << input::escape(*itr) << '"' << '\n';
  }
}

auto printEntryPoints(const vm::Assembly& assembly) -> void {
  std::cout << rang::style::bold << "EntryPoints:\n" << rang::style::reset;
  for (auto itr = assembly.beginEntryPoints(); itr != assembly.endEntryPoints(); ++itr) {
    std::cout << "  " << rang::style::bold << *itr << rang::style::reset << '\n';
  }
}

auto printInstructions(const vm::Assembly& assembly) -> void {
  const auto ipOffsetColWidth = 5;
  const auto opCodeColWidth   = 20;

  std::cout << rang::style::bold << "Instructions:\n" << rang::style::reset;
  auto instructions = backend::dasm::disassembleInstructions(assembly);
  for (const auto& instr : instructions) {

    std::cout << "  " << rang::style::bold << std::setw(ipOffsetColWidth) << std::left
              << instr.getIpOffset() << rang::style::reset << std::setw(opCodeColWidth) << std::left
              << instr.getOp();
    for (const auto& arg : instr.getArgs()) {
      backend::dasm::operator<<(std::cout, arg);
    }
    std::cout << '\n';
  }
}

auto printProgram(const vm::Assembly& assembly) -> void {
  printStringLiterals(assembly);
  std::cout << '\n';
  printEntryPoints(assembly);
  std::cout << '\n';
  printInstructions(assembly);
}

template <typename InputItr>
auto run(
    const std::string& inputId,
    InputItr inputBegin,
    const InputItr inputEnd,
    const bool outputProgram) {
  const auto width = 80;

  // Generate an assembly file and time how long it takes.
  const auto t1             = high_resolution_clock::now();
  const auto src            = frontend::buildSource(inputId, inputBegin, inputEnd);
  const auto frontendOutput = frontend::analyze(src);
  const auto assembly       = genAssembly(frontendOutput);
  const auto t2             = high_resolution_clock::now();
  const auto genDur         = std::chrono::duration_cast<duration>(t2 - t1);

  std::cout << rang::style::dim << rang::style::italic << std::string(width, '-') << '\n'
            << "Generated assembly in " << genDur << '\n'
            << std::string(width, '-') << '\n'
            << rang::style::reset;

  // Print diagnostics.
  for (auto diagItr = frontendOutput.beginDiags(); diagItr != frontendOutput.endDiags();
       ++diagItr) {
    std::cout << rang::style::bold << rang::bg::red << *diagItr << rang::bg::reset << '\n'
              << rang::style::reset;
  }

  if (assembly && outputProgram) {
    printProgram(*assembly);
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

} // namespace asmdiag

auto main(int argc, char** argv) -> int {
  auto app = CLI::App{"Assembly diagnostic tool"};
  app.require_subcommand(1);

  auto printOutput = true;
  app.add_flag("!--skip-output", printOutput, "Skip printing the assembly")->capture_default_str();

  // Generate assembly for input characters.
  std::string charsInput;
  auto genCmd =
      app.add_subcommand("gen", "Generate assembly for the provided characters")->callback([&]() {
        asmdiag::run("inline", charsInput.begin(), charsInput.end(), printOutput);
      });
  genCmd->add_option("input", charsInput, "Input characters to generate assembly for")->required();

  // Generate assembly  input file.
  std::string filePath;
  auto genFileCmd =
      app.add_subcommand("genfile", "Generate assembly for the provided file")->callback([&]() {
        std::ifstream fs{filePath};
        asmdiag::run(
            filePath,
            std::istreambuf_iterator<char>{fs},
            std::istreambuf_iterator<char>{},
            printOutput);
      });
  genFileCmd->add_option("file", filePath, "Path to file to generate assembly for")
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
