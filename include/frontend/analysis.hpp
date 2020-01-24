#pragma once
#include "frontend/output.hpp"
#include "frontend/source.hpp"
#include <filesystem>

namespace frontend {

auto analyze(const Source& mainSrc, const std::vector<std::filesystem::path>& searchPaths = {})
    -> Output;

} // namespace frontend
