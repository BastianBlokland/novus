#pragma once
#include "prog/program.hpp"
#include "vm/assembly.hpp"

namespace backend {

auto generate(const prog::Program& program) -> vm::Assembly;

} // namespace backend
