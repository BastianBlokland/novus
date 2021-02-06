#pragma once
#include <cstdint>
#include <iostream>

namespace vm {

// Current (or last) state of an executor. Also used as the exit code of the virtual machine.
enum class ExecState : int8_t {
  /* Internal states for executors, will never be returned from the vm.
   */
  Aborted = -3, // The executor has been aborted.
  Paused  = -2, // The executor is paused, will never be returned from the vm.
  Running = -1, // The executor is running, will never be returned from the vm.

  /* Normal exit codes for the vm.
   */
  Success      = 0, // The executor has finished successfully, is the 'normal' result of the vm.
  Failed       = 1, // The executor has encountered a 'fail' instruction.
  AssertFailed = 2, // An assert in the user programs has failed.

  /* Error exit codes for the vm.
   */
  VmInitFailed    = 10, // Failed to initialize the vm, host is likely out of resources.
  InvalidAssembly = 11, // The executor has encountered an invalid instruction in the assembly.
  StackOverflow   = 12, // The stack memory of the executor has been exhausted.
  AllocFailed     = 13, // Failed to allocate memory, host is likely out of resources.
  DivByZero       = 14, // The executor has encountered a 'divide by zero' during execution.
  ForkFailed      = 15, // Failed to start a new executor, host is likely out of resources.
};

auto operator<<(std::ostream& out, const ExecState& rhs) noexcept -> std::ostream&;

} // namespace vm
