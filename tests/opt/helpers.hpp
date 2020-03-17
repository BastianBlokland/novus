#pragma once
#include "catch2/catch.hpp"
#include "frontend/analysis.hpp"
#include "frontend/source.hpp"

namespace opt {

#define SRC(INPUT)                                                                                 \
  frontend::buildSource("test", std::nullopt, std::string{INPUT}.begin(), std::string{INPUT}.end())

#define ANALYZE(INPUT) analyze(SRC(INPUT))

} // namespace opt
