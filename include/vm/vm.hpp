#pragma once
#include "novasm/executable.hpp"
#include "vm/exec_state.hpp"
#include "vm/platform_interface.hpp"

namespace vm {

// Execute the given program. Will block until the execution is complete.
auto run(const novasm::Executable* executable, PlatformInterface* iface) noexcept -> ExecState;

} // namespace vm
