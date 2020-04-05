#pragma once
#include "filesystem.hpp"
#include <vector>

namespace input {

auto getSearchPaths(char** argv) noexcept -> std::vector<filesystem::path>;

} // namespace input
