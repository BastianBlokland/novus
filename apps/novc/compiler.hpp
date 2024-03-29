#pragma once
#include "filesystem.hpp"
#include <vector>

namespace novc {

struct CompileOptions {
  filesystem::path srcPath;
  filesystem::path destPath;
  const std::vector<filesystem::path>& searchPaths;
  bool optimize;
};

auto compile(const CompileOptions& options) -> bool;

} // namespace novc
