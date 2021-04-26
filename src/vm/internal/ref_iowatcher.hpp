#pragma once
#include "gsl.hpp"
#include "internal/iowatcher.hpp"
#include "internal/platform_utilities.hpp"
#include "internal/ref_allocator.hpp"
#include "internal/ref_string.hpp"

namespace vm::internal {

class IOWatcherRef final : public Ref {
  friend class RefAllocator;

public:
  IOWatcherRef(const IOWatcherRef& rhs) = delete;
  IOWatcherRef(IOWatcherRef&& rhs)      = delete;
  ~IOWatcherRef() noexcept { ioWatcherDestroy(m_watcher); }

  auto operator=(const IOWatcherRef& rhs) -> IOWatcherRef& = delete;
  auto operator=(IOWatcherRef&& rhs) -> IOWatcherRef& = delete;

  [[nodiscard]] constexpr static auto getKind() { return RefKind::IOWatcher; }

  [[nodiscard]] auto
  get(ExecutorHandle* execHandle, PlatformError* pErr, StringRef* result) noexcept -> bool {
    return ioWatcherGet(m_watcher, execHandle, result, pErr);
  }

private:
  gsl::owner<IOWatcher*> m_watcher;

  IOWatcherRef(const char* rootPath, IOWatcherFlags flags) noexcept : Ref(getKind()) {
    m_watcher = ioWatcherCreate(rootPath, flags);
  }
};

inline auto
ioWatcherCreate(RefAllocator* alloc, const StringRef* path, IOWatcherFlags flags) noexcept
    -> IOWatcherRef* {
  return alloc->allocPlain<IOWatcherRef>(path->getCharDataPtr(), flags);
}

inline auto ioWatcherGet(
    ExecutorHandle* execHandle,
    PlatformError* pErr,
    IOWatcherRef* watcher,
    StringRef* result) noexcept -> bool {
  assert(execHandle && pErr && watcher && result);
  return watcher->get(execHandle, pErr, result);
}

} // namespace vm::internal
