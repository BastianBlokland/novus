#include "vm/vm.hpp"
#include "internal/allocator.hpp"
#include "internal/executor.hpp"
#include "vm/platform/memory_interface.hpp"
#include "vm/platform/terminal_interface.hpp"

namespace vm {

template <typename PlatformInterface>
auto run(const Assembly* assembly, PlatformInterface* iface) noexcept -> ExecState {

  auto allocator   = internal::Allocator{};
  auto resultState = ExecState::Success;

  for (auto itr = assembly->beginEntryPoints(); itr != assembly->endEntryPoints(); ++itr) {
    auto entryPoint = *itr;
    resultState     = execute(assembly, iface, &allocator, entryPoint, 0, nullptr, nullptr);
    if (resultState != ExecState::Success) {
      return resultState;
    }
  }

  return resultState;
}

// Explicit instantiations.
template ExecState run(const Assembly* assembly, platform::MemoryInterface* iface);
template ExecState run(const Assembly* assembly, platform::TerminalInterface* iface);

} // namespace vm
