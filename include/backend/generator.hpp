#pragma once
#include "novasm/assembly.hpp"
#include "prog/program.hpp"

namespace backend {

auto generate(const prog::Program& program) -> novasm::Assembly;

} // namespace backend
