#pragma once
#include "vm/assembly.hpp"
#include "vm/platform/interface.hpp"

namespace vm {

auto execute(const Assembly& assembly, platform::Interface* iface) -> void;

} // namespace vm
