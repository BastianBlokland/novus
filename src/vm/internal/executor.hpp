#pragma once
#include "internal/executor_registry.hpp"
#include "internal/ref_allocator.hpp"
#include "internal/settings.hpp"
#include "novasm/assembly.hpp"
#include "vm/exec_state.hpp"
#include "vm/platform_interface.hpp"

namespace vm::internal {

// Execute a specific entrypoint in the assembly until completion.
//
// 'entryArgCount', 'entryArgSource', 'promise' are used for sub-executers (forked calls) that take
// arguments from a parent executor and place their result in the 'promise' object.
auto execute(
    const Settings& settings,
    const novasm::Assembly* assembly,
    PlatformInterface* iface,
    ExecutorRegistry* execRegistry,
    RefAllocator* refAlloc,
    uint32_t entryIpOffset,
    uint8_t entryArgCount,
    Value* entryArgSource,
    FutureRef* promise) noexcept -> ExecState;

} // namespace vm::internal
