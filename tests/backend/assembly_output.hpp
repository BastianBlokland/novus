#pragma once
#include "vm/assembly.hpp"
#include <iostream>

namespace vm {

auto operator<<(std::ostream& out, const vm::Assembly& rhs) -> std::ostream&;

} // namespace vm
