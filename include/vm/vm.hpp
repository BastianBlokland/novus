#pragma once
#include "novasm/assembly.hpp"
#include "vm/exec_state.hpp"

namespace vm {

template <typename PlatformInterface>
auto run(const novasm::Assembly* assembly, PlatformInterface* iface) noexcept -> ExecState;

} // namespace vm
