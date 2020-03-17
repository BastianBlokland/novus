#pragma once
#include "prog/program.hpp"

namespace opt {

auto optimize(const prog::Program& prog) -> prog::Program;

auto treeshake(const prog::Program& prog) -> prog::Program;

auto inlineCalls(const prog::Program& prog) -> prog::Program;

} // namespace opt
