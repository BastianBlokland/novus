#include "vm/vm.hpp"
#include "internal/allocator.hpp"
#include "internal/executor.hpp"
#include "internal/executor_registry.hpp"
#include "vm/platform_interface.hpp"

namespace vm {

auto run(const novasm::Assembly* assembly, PlatformInterface* iface) noexcept -> ExecState {

  auto execRegistry = internal::ExecutorRegistry{};
  auto allocator    = internal::Allocator{&execRegistry};

  auto resultState = execute(
      assembly, iface, &execRegistry, &allocator, assembly->getEntrypoint(), 0, nullptr, nullptr);

  // Abort all executors that are still running.
  execRegistry.abortExecutors();
  return resultState;
}

} // namespace vm
