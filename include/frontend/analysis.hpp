#pragma once
#include "filesystem.hpp"
#include "frontend/output.hpp"
#include "frontend/source.hpp"

namespace frontend {

auto analyze(const Source& mainSrc, const std::vector<filesystem::path>& searchPaths = {})
    -> Output;

// Gather the dependencies (all imports) of the given source file.
auto getDependencies(const Source& mainSrc, const std::vector<filesystem::path>& searchPaths)
    -> std::forward_list<Source>;

} // namespace frontend
