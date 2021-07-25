// -- Include rang before any os headers.
#include "../rang_include.hpp"
// --

#include "backend/generator.hpp"
#include "compiler.hpp"
#include "config.hpp"
#include "frontend/analysis.hpp"
#include "frontend/source.hpp"
#include "novasm/serialization.hpp"
#include "opt/opt.hpp"
#include "utilities.hpp"
#include <algorithm>
#include <chrono>
#include <fstream>

namespace novc {

using Clock    = std::chrono::high_resolution_clock;
using Duration = std::chrono::duration<double>;

static auto operator<<(std::ostream& out, const Duration& rhs) -> std::ostream&;
static auto msgHeader(std::ostream& out) -> std::ostream&;
static auto infHeader(std::ostream& out) -> std::ostream&;

auto compile(const CompileOptions& options) -> bool {

  std::cout << rang::style::bold << "--- Novus compiler [" PROJECT_VER "] ---\n"
            << rang::style::reset;

  auto validateOnly = options.destPath.empty();
  if (validateOnly) {
    msgHeader(std::cout) << "Validate: " << options.srcPath << '\n';
  } else {
    msgHeader(std::cout) << "Compile: " << options.srcPath << '\n';
  }

  // Load the source file.
  const auto absSrcPath = filesystem::canonical(filesystem::absolute(options.srcPath));
  auto srcFilestream    = std::ifstream{absSrcPath.string()};
  if (!srcFilestream.good()) {
    msgHeader(std::cerr) << rang::style::bold << rang::bg::red << "Failed to read source file\n"
                         << rang::style::reset;
    return false;
  }

  const auto compileStartTime = Clock::now();

  // Parse the source.
  const auto src = frontend::buildSource(
      absSrcPath.filename().string(),
      absSrcPath,
      std::istreambuf_iterator<char>{srcFilestream},
      std::istreambuf_iterator<char>{});

  // Build a program out of the source.
  const auto frontendOut = frontend::analyze(src, options.searchPaths);

  const auto compileEndTime = Clock::now();
  const auto compileDur = std::chrono::duration_cast<Duration>(compileEndTime - compileStartTime);

  if (!frontendOut.isSuccess()) {
    msgHeader(std::cout) << rang::style::bold << rang::bg::red << "Compilation failed, errors:\n";
    for (auto diagItr = frontendOut.beginDiags(); diagItr != frontendOut.endDiags(); ++diagItr) {
      diagItr->print(std::cerr, frontendOut.getSourceTable());
      std::cerr << '\n';
    }
    std::cerr << rang::style::reset;
    return false;
  }

  const auto numFiles = 1u +
      std::distance(frontendOut.getImportedSources().begin(),
                    frontendOut.getImportedSources().end());

  msgHeader(std::cout) << "Analyzed program in: " << compileDur << '\n';
  infHeader(std::cout) << "Files: " << numFiles << '\n';
  infHeader(std::cout) << "Types: " << frontendOut.getProg().getTypeCount() << '\n';
  infHeader(std::cout) << "Functions: " << frontendOut.getProg().getFuncCount() << '\n';
  infHeader(std::cout) << "Execute statements: " << frontendOut.getProg().getExecStmtCount()
                       << '\n';

  if (validateOnly) {
    msgHeader(std::cout) << "Successfully validated program\n";
    return true;
  }

  auto optProg = prog::Program{};
  if (options.optimize) {

    msgHeader(std::cout) << "Optimize program\n";

    const auto optStartTime = Clock::now();
    optProg                 = opt::optimize(frontendOut.getProg());
    const auto optEndTime   = Clock::now();
    const auto optDur       = std::chrono::duration_cast<Duration>(optEndTime - optStartTime);

    msgHeader(std::cout) << "Finished optimizing program in: " << optDur << '\n';
    infHeader(std::cout) << "Types: " << optProg.getTypeCount() << '\n';
    infHeader(std::cout) << "Functions: " << optProg.getFuncCount() << '\n';
    infHeader(std::cout) << "Execute statements: " << optProg.getExecStmtCount() << '\n';
  }

  const auto& prog = options.optimize ? optProg : frontendOut.getProg();

  // Generate novus assembly representation.
  msgHeader(std::cout) << "Generate novus assembly\n";

  const auto asmStartTime = Clock::now();
  const auto asmOutput    = backend::generate(prog);
  const auto asmEndTime   = Clock::now();
  const auto asmDur       = std::chrono::duration_cast<Duration>(asmEndTime - asmStartTime);

  msgHeader(std::cout) << "Finished generating novus assembly in: " << asmDur << '\n';
  infHeader(std::cout) << "Instructions: " << asmOutput.first.getInstructions().size() << '\n';

  // Write executable to disk.
  auto destFilestream = std::ofstream{options.destPath.string(), std::ios::binary};
  if (!destFilestream.good()) {
    msgHeader(std::cerr) << rang::style::bold << rang::bg::red << "Failed to write to output path\n"
                         << rang::style::reset;
    return false;
  }
  novasm::serialize(asmOutput.first, std::ostreambuf_iterator<char>{destFilestream});

  if (!setOutputFilePermissions(options.destPath)) {
    msgHeader(std::cerr) << rang::style::bold << rang::bg::red
                         << "Failed to set output file permissions\n"
                         << rang::style::reset;
    return false;
  }

  msgHeader(std::cout) << "Successfully compiled executable to: " << options.destPath << '\n';
  return true;
}

static auto msgHeader(std::ostream& out) -> std::ostream& {
  return out << rang::style::bold << rang::fg::green << "* " << rang::style::reset
             << rang::fg::reset;
}

static auto infHeader(std::ostream& out) -> std::ostream& {
  return out << rang::style::dim << rang::fg::gray << "  * " << rang::style::reset
             << rang::fg::reset;
}

static auto operator<<(std::ostream& out, const Duration& rhs) -> std::ostream& {
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

} // namespace novc
