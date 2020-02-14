#include "vm/vm.hpp"
#include "internal/allocator.hpp"
#include "internal/executor.hpp"
#include "internal/executor_registry.hpp"
#include "vm/platform/memory_interface.hpp"
#include "vm/platform/terminal_interface.hpp"

namespace vm {

template <typename PlatformInterface>
auto run(const Assembly* assembly, PlatformInterface* iface) noexcept -> ExecState {

  auto execRegistry = internal::ExecutorRegistry{};
  auto allocator    = internal::Allocator{&execRegistry};

  auto resultState = execute(
      assembly, iface, &execRegistry, &allocator, assembly->getEntrypoint(), 0, nullptr, nullptr);

  // Abort all executors that are still running.
  execRegistry.abortExecutors();
  return resultState;
}

// Explicit instantiations.
template ExecState run(const Assembly* assembly, platform::MemoryInterface* iface);
template ExecState run(const Assembly* assembly, platform::TerminalInterface* iface);

} // namespace vm
