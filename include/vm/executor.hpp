#pragma once
#include "vm/assembly.hpp"
#include "vm/io/interface.hpp"

namespace vm {

auto execute(const Assembly& assembly, io::Interface* interface) -> void;

} // namespace vm
