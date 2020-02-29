#pragma once
#include "prog/program.hpp"

namespace opt {

auto treeshake(const prog::Program& prog) -> prog::Program;

} // namespace opt
