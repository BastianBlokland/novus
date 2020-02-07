#include "vm/vm.hpp"
#include "internal/allocator.hpp"
#include "internal/executor.hpp"
#include "vm/platform/memory_interface.hpp"
#include "vm/platform/terminal_interface.hpp"

namespace vm {

template <typename PlatformInterface>
auto execute(const Assembly& assembly, PlatformInterface& iface) noexcept -> ExecState {
  auto allocator = internal::Allocator{};
  auto execState = ExecState::Success;
  auto retVal    = internal::Value{};
  for (auto itr = assembly.beginEntryPoints(); itr != assembly.endEntryPoints(); ++itr) {
    execState = execute(&allocator, assembly, iface, *itr, &retVal);
    if (execState != ExecState::Success) {
      return execState;
    }
  }
  return execState;
}

// Explicit instantiations.
template ExecState execute(const Assembly& assembly, platform::MemoryInterface& iface);
template ExecState execute(const Assembly& assembly, platform::TerminalInterface& iface);

} // namespace vm
