#pragma once
#include "vm/assembly.hpp"
#include "vm/exec_state.hpp"

namespace vm {

template <typename PlatformInterface>
auto run(const Assembly* assembly, PlatformInterface* iface) noexcept -> ExecState;

} // namespace vm
