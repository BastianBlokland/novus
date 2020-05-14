// -- Include rang before any os headers.
#include "../rang_include.hpp"
// --

#include "CLI/CLI.hpp"
#include "compiler.hpp"
#include "filesystem.hpp"
#include "input/search_paths.hpp"
#include <algorithm>

auto main(int argc, char** argv) noexcept -> int {
  auto app = CLI::App{"Novus compiler"};

  auto additionalSearchPaths = std::vector<filesystem::path>{};
  auto colorMode             = rang::control::Auto;
  auto optimize              = true;
  auto validateOnly          = false;
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
  app.add_flag("-v,--validate-only", validateOnly, "Only validate but do not output the program");
  app.add_option(
         "-s,--searchpaths", additionalSearchPaths, "Additional paths to search for imports")
      ->check(CLI::ExistingDirectory);
  app.validate_positionals();

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

  // If no output path is given generate one based on the source file path.
  if (outPath.empty()) {
    outPath = srcPath;
    outPath.replace_extension("nova");
  }

  // In validate-only mode don't specify an output path.
  if (validateOnly) {
    outPath = filesystem::path{};
  }

  // Gather the default search-paths and add any manually provided ones.
  auto searchPaths = input::getSearchPaths(argv);
  std::copy(
      additionalSearchPaths.begin(), additionalSearchPaths.end(), std::back_inserter(searchPaths));

  auto options = novc::Options{srcPath, outPath, searchPaths, optimize};
  auto success = novc::compile(options);

  return success ? 0 : 1;
}
