
#include "internal/iowatcher.hpp"
#include "internal/ref_allocator.hpp"
#include "internal/ref_string.hpp"

namespace vm::internal {

/**
 * Watcher object, stays alive for the duration of the watcher.
 */
struct IOWatcher {
  IOWatcherFlags flags;
};

auto ioWatcherCreate(const char*, IOWatcherFlags flags) noexcept -> IOWatcher* {
  return new IOWatcher{flags};
}

auto ioWatcherGet(IOWatcher*, ExecutorHandle*, StringRef* result, PlatformError* pErr) noexcept
    -> bool {
  *pErr = PlatformError::IOWatcherNotSupported;
  result->updateSize(0);
  return false;
}

auto ioWatcherDestroy(IOWatcher* watcher) noexcept -> void { delete watcher; }

} // namespace vm::internal
