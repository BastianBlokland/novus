#pragma once
#include "vm/assembly.hpp"

namespace vm {

template <typename PlatformInterface>
auto execute(const Assembly& assembly, PlatformInterface& iface) -> void;

} // namespace vm
