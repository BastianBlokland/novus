#pragma once

namespace vm::internal {

auto interruptSetupHandler() noexcept -> bool;

[[nodiscard]] auto interuptIsRequested() noexcept -> bool;

[[nodiscard]] auto interuptResetRequested() noexcept -> bool;

} // namespace vm::internal
