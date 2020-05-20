#pragma once
#include "novasm/assembly.hpp"
#include "vm/exec_state.hpp"
#include "vm/platform_interface.hpp"

namespace vm {

// Execute the given program. Will block until the execution is complete.
// TODO(bastian): How could we handle interupt signals with this api?
auto run(const novasm::Assembly* assembly, PlatformInterface* iface) noexcept -> ExecState;

} // namespace vm
