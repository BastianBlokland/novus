// -- Include rang before any os headers.
#include "../rang_include.hpp"
// --

#include "CLI/CLI.hpp"
#include "compiler.hpp"
#include "config.hpp"
#include "deps.hpp"
#include "filesystem.hpp"
#include "input/search_paths.hpp"
#include <algorithm>

auto main(int argc, const char** argv) noexcept -> int {
  auto app = CLI::App{"Novus compiler [" PROJECT_VER "]", "novc"};

  auto additionalSearchPaths = std::vector<filesystem::path>{};
  auto colorMode             = rang::control::Auto;
  auto optimize              = true;
  filesystem::path srcPath;
  filesystem::path outPath;

  app.add_flag(
      "--no-color{0},-c{2},--color{2},--auto-color{1}",
      colorMode,
      "Set the color output behaviour");
  app.add_option("source-file", srcPath, "Path to the source file to compile")
      ->check(CLI::ExistingFile)
      ->required();
  app.add_option("-o,--out", outPath, "Path to output the program to");
  app.add_flag("--optimize,!--no-optimize", optimize, "Optimize the program");
  app.add_option(
         "-s,--searchpaths", additionalSearchPaths, "Additional paths to search for imports")
      ->check(CLI::ExistingDirectory);
  app.validate_positionals();

  // Sub-commands.
  auto validateCmd =
      app.add_subcommand("validate", "Validate the given sourcefile without compiling it");
  auto depsCmd = app.add_subcommand("deps", "Output the dependencies of the given sourcefile");

  // Parse arguments.
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

  rang::setControlMode(colorMode);

  const bool outputProgram = !app.got_subcommand(validateCmd) && !app.got_subcommand(depsCmd);
  if (outputProgram) {
    // If no output path is given generate one based on the source file path.
    if (outPath.empty()) {
      outPath = srcPath;
    }
    outPath.replace_extension("nx");
  } else {
    outPath = filesystem::path{};
  }

  // Gather the default search-paths and add any manually provided ones.
  auto searchPaths = input::getSearchPaths(argv);
  std::copy(
      additionalSearchPaths.begin(), additionalSearchPaths.end(), std::back_inserter(searchPaths));

  if (app.got_subcommand(depsCmd)) {
    return novc::deps({srcPath, searchPaths}) ? 0 : 1;
  }

  return novc::compile({srcPath, outPath, searchPaths, optimize}) ? 0 : 1;
}
