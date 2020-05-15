#include "vm/vm.hpp"
#include "internal/executor.hpp"
#include "internal/executor_registry.hpp"
#include "internal/ref_allocator.hpp"
#include "vm/platform_interface.hpp"

namespace vm {

auto run(const novasm::Assembly* assembly, PlatformInterface* iface) noexcept -> ExecState {

  auto execRegistry = internal::ExecutorRegistry{};
  auto refAlloc     = internal::RefAllocator{};
  auto gc           = internal::GarbageCollector{&refAlloc, &execRegistry};

  auto resultState = execute(
      assembly, iface, &execRegistry, &refAlloc, assembly->getEntrypoint(), 0, nullptr, nullptr);

  // Abort all executors that are still running.
  execRegistry.abortExecutors();

  // Terminate the garbage-collector (finishes any ongoing collections).
  gc.terminateCollector();

  return resultState;
}

} // namespace vm
