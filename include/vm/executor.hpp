#pragma once
#include "vm/assembly.hpp"

namespace vm {

auto execute(const Assembly& assembly) -> void;

} // namespace vm
