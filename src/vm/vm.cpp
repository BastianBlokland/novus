#include "vm/vm.hpp"
#include "internal/executor.hpp"
#include "internal/executor_registry.hpp"
#include "internal/ref_allocator.hpp"
#include "vm/platform_interface.hpp"
#include <csignal>

namespace vm {

static auto setupSignalHandlers() {
  // Ignore sig-pipe (we want to handle it on a per call basis instead of globally).
  signal(SIGPIPE, SIG_IGN);
}

auto run(const novasm::Assembly* assembly, PlatformInterface* iface) noexcept -> ExecState {

  setupSignalHandlers();

  auto settings           = internal::Settings{};
  settings.socketsEnabled = true;

  auto execRegistry = internal::ExecutorRegistry{};
  auto memAlloc     = internal::MemoryAllocator{};
  auto refAlloc     = internal::RefAllocator{&memAlloc};
  auto gc           = internal::GarbageCollector{&refAlloc, &execRegistry};

  auto resultState = execute(
      settings,
      assembly,
      iface,
      &execRegistry,
      &refAlloc,
      assembly->getEntrypoint(),
      0,
      nullptr,
      nullptr);

  // Abort all executors that are still running.
  execRegistry.abortExecutors();

  // Terminate the garbage-collector (finishes any ongoing collections).
  gc.terminateCollector();

  return resultState;
}

} // namespace vm
