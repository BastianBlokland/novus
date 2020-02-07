#pragma once
#include "vm/assembly.hpp"
#include "vm/exec_state.hpp"

namespace vm {

template <typename PlatformInterface>
auto execute(const Assembly& assembly, PlatformInterface& iface) noexcept -> ExecState;

} // namespace vm
