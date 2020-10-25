#pragma once

namespace vm::internal {

auto setupInterruptHandler() noexcept -> bool;

[[nodiscard]] auto isInterruptRequested() noexcept -> bool;

} // namespace vm::internal
