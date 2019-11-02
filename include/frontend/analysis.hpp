#pragma once
#include "frontend/output.hpp"
#include "frontend/source.hpp"

namespace frontend {

auto analyze(const Source& src) -> Output;

} // namespace frontend
