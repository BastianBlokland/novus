#pragma once
#include <cstdint>
#include <iostream>

namespace vm {

// Current (or last) state of an executor. Also used as the exit code of the virtual machine.
enum class ExecState : int8_t {
  Paused  = -2, // The executor is paused, will never be returned from the vm.
  Running = -1, // The executor is running, will never be returned from the vm.
  Success = 0,  // The executor has finished successfully, is the 'normal' result of the vm.
  Failed  = 1, // The executor has encountered a 'fail' instruction, is not an error case for the vm
               // but a mechanism for a user-program to indicate failure.
  Aborted = 2, // The executor has been aborted, happens to executors when the vm is tearing down.
  InvalidAssembly = 3, // The executor has encountered an invalid instruction in the assembly, this
                       // cannot be recovered from.
  StackOverflow = 4,   // The stack memory of the executor has been exhausted, ususally indicates a
                       // bad user program.
  AllocFailed = 5,  // The executor has failed to allocate memory, usually indicates the system is
                    // out of memory. This cannot be recovered from.
  DivByZero    = 6, // The executor has encountered a 'divide by zero' during execution.
  AssertFailed = 7, // An assert in the user programs has failed, note this is not an error-case for
                    // the vm itself.
};

auto operator<<(std::ostream& out, const ExecState& rhs) noexcept -> std::ostream&;

} // namespace vm
