#pragma once
#include "novasm/assembly.hpp"
#include "vm/exec_state.hpp"
#include "vm/platform_interface.hpp"

namespace vm {

auto run(const novasm::Assembly* assembly, PlatformInterface* iface) noexcept -> ExecState;

} // namespace vm
