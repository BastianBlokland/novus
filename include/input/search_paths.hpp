#pragma once
#include "filesystem.hpp"
#include <vector>

namespace input {

auto getSearchPaths(const char** argv) noexcept -> std::vector<filesystem::path>;

auto getExecutablePath() noexcept -> filesystem::path;

} // namespace input
