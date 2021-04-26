#pragma once
#include "gsl.hpp"
#include "internal/platform_utilities.hpp"

namespace vm::internal {

enum IOWatcherFlags : uint8_t {};

struct IOWatcher;
class ExecutorHandle;

auto ioWatcherCreate(const char* rootPath, IOWatcherFlags flags) noexcept -> IOWatcher*;
auto ioWatcherGet(
    IOWatcher* watcher, ExecutorHandle* execHandle, StringRef* result, PlatformError* pErr) noexcept
    -> bool;
auto ioWatcherDestroy(IOWatcher*) noexcept -> void;

} // namespace vm::internal
