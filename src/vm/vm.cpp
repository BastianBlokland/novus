#include "vm/vm.hpp"
#include "internal/allocator.hpp"
#include "internal/executor.hpp"
#include "vm/platform/memory_interface.hpp"
#include "vm/platform/terminal_interface.hpp"

namespace vm {

template <typename PlatformInterface>
auto execute(const Assembly& assembly, PlatformInterface& iface) noexcept -> ResultCode {
  auto allocator  = internal::Allocator{};
  auto resultCode = ResultCode::Ok;
  auto retVal     = internal::Value{};
  for (auto itr = assembly.beginEntryPoints(); itr != assembly.endEntryPoints(); ++itr) {
    resultCode = execute(&allocator, assembly, iface, *itr, &retVal);
    if (resultCode != ResultCode::Ok) {
      return resultCode;
    }
  }
  return resultCode;
}

// Explicit instantiations.
template ResultCode execute(const Assembly& assembly, platform::MemoryInterface& iface);
template ResultCode execute(const Assembly& assembly, platform::TerminalInterface& iface);

} // namespace vm
