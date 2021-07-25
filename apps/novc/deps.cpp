// -- Include rang before any os headers.
#include "../rang_include.hpp"
// --

#include "config.hpp"
#include "deps.hpp"
#include "frontend/analysis.hpp"
#include "frontend/source.hpp"
#include <fstream>

namespace novc {

auto deps(const DepsOptions& options) -> bool {

  // Load the source file.
  const auto absSrcPath = filesystem::canonical(filesystem::absolute(options.srcPath));
  auto srcFilestream    = std::ifstream{absSrcPath.string()};
  if (!srcFilestream.good()) {
    std::cerr << rang::style::bold << rang::bg::red << "Failed to read source file\n"
              << rang::style::reset;
    return false;
  }

  // Parse the source.
  const auto src = frontend::buildSource(
      absSrcPath.filename().string(),
      absSrcPath,
      std::istreambuf_iterator<char>{srcFilestream},
      std::istreambuf_iterator<char>{});

  // Gather dependencies.
  for (const auto& depSource : frontend::getDependencies(src, options.searchPaths)) {
    std::cout << *depSource.getPath() << '\n';
  }

  return true;
}

} // namespace novc
