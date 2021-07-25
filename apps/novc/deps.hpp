#pragma once
#include "filesystem.hpp"
#include <vector>

namespace novc {

struct DepsOptions {
  filesystem::path srcPath;
  const std::vector<filesystem::path>& searchPaths;
};

auto deps(const DepsOptions& options) -> bool;

} // namespace novc
