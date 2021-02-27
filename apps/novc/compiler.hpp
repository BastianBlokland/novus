#pragma once
#include "filesystem.hpp"

namespace novc {

struct Options {
  filesystem::path srcPath;
  filesystem::path destPath;
  const std::vector<filesystem::path>& searchPaths;
  bool optimize;
};

auto compile(Options opts) -> bool;

} // namespace novc
