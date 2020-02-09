#pragma once
#include "filesystem.hpp"
#include "frontend/output.hpp"
#include "frontend/source.hpp"

namespace frontend {

auto analyze(const Source& mainSrc, const std::vector<filesystem::path>& searchPaths = {})
    -> Output;

} // namespace frontend
