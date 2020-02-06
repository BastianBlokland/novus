#pragma once
#include "internal/allocator.hpp"
#include "vm/assembly.hpp"
#include "vm/result_code.hpp"

namespace vm::internal {

template <typename PlatformInterface>
auto execute(
    Allocator* allocator,
    const Assembly& assembly,
    PlatformInterface& iface,
    uint32_t entryPoint,
    Value* execRetVal) noexcept -> ResultCode;

} // namespace vm::internal
