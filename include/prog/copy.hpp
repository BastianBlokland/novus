#pragma once
#include "prog/program.hpp"

namespace prog {

auto copyType(const Program& from, Program* to, sym::TypeId func) -> void;

auto copyUserFunc(const Program& from, Program* to, sym::FuncId func) -> void;

} // namespace prog
