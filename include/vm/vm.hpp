#pragma once
#include "vm/assembly.hpp"
#include "vm/result_code.hpp"

namespace vm {

template <typename PlatformInterface>
auto execute(const Assembly& assembly, PlatformInterface& iface) noexcept -> ResultCode;

} // namespace vm
