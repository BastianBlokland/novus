#pragma once
#include "internal/allocator.hpp"
#include "internal/executor_registry.hpp"
#include "vm/assembly.hpp"
#include "vm/exec_state.hpp"

namespace vm::internal {

template <typename PlatformInterface>
auto execute(
    const Assembly* assembly,
    PlatformInterface* iface,
    ExecutorRegistry* execRegistry,
    Allocator* allocator,
    uint32_t entryIpOffset,
    uint8_t entryArgCount,
    Value* entryArgSource,
    FutureRef* promise) noexcept -> ExecState;

} // namespace vm::internal
