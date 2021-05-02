
#include "internal/iowatcher.hpp"
#include "internal/os_include.hpp"
#include "internal/platform_utilities.hpp"
#include "internal/ref_allocator.hpp"
#include "internal/ref_string.hpp"
#include "internal/thread.hpp"
#include <CoreServices/CoreServices.h>
#include <array>

namespace vm::internal {

namespace {

/**
 * Delay between the fsevents api receiving the event and notifying us. Higher values lead to more
 * delay but also greater temporal coalecing.
 */
const double g_latency = 0.05; // In seconds.

/**
 * Maximum number of events to buffer up. If more events then this arrive before the application
 * reads them they will be silently dropped.
 */
constexpr size_t g_bufferMaxEntries = 64;

using PathBuffer = std::array<char, PATH_MAX>;

} // namespace

enum class WatcherState : uint8_t {
  Initializing,
  Listening,
  Stopped,
};

/**
 * Watcher object, stays alive for the duration of the watcher.
 */
struct IOWatcher {
  IOWatcherFlags flags;
  std::atomic<WatcherState> state;
  std::atomic<PlatformError> error;

  /**
   * Reference counter, allows the event-stream thread to gracefully shutdown while the reference to
   * the watcher has already been garbage collected.
   */
  std::atomic<int> refCount;

  std::mutex mutex;
  std::condition_variable eventsReceived;

  gsl::owner<char*> rootPath;
  CFStringRef rootPathCF;
  CFArrayRef pathsToWatchCF;

  FSEventStreamRef eventStream;
  CFRunLoopRef eventRunLoop;

  size_t eventsHead;
  size_t eventsTail;
  std::array<PathBuffer, g_bufferMaxEntries> eventBuffer;
};

namespace {

auto watcherDestroyImpl(IOWatcher* watcher) {
  ::CFRelease(watcher->pathsToWatchCF);
  ::CFRelease(watcher->rootPathCF);
  ::free(watcher->rootPath);
  delete watcher;
}

// Increment the reference counter.
auto watcherRefInc(IOWatcher* watcher) {
  watcher->refCount.fetch_add(1, std::memory_order_acq_rel);
}

// Decrement the reference counter.
auto watcherRefDec(IOWatcher* watcher) {
  if (watcher->refCount.fetch_sub(1, std::memory_order_acq_rel) == 1) {
    watcherDestroyImpl(watcher);
  }
}

/**
 * RAII helper for increment and decrementing the ref counter.
 */
class WatcherRef {
public:
  WatcherRef(IOWatcher* watcher) : m_watcher{watcher} { watcherRefInc(m_watcher); }
  ~WatcherRef() { watcherRefDec(m_watcher); }

private:
  IOWatcher* m_watcher;
};

/**
 * Callback that will be invoked by the fsevents api.
 * Docs: https://developer.apple.com/documentation/coreservices/fseventstreamcallback
 */
auto fseventCallback(
    ConstFSEventStreamRef,
    void* userdata,
    size_t numEvents,
    void* eventPaths,
    const FSEventStreamEventFlags eventFlags[],
    const FSEventStreamEventId[]) noexcept -> void {
  auto* watcher = static_cast<IOWatcher*>(userdata);

  auto lk = std::lock_guard<std::mutex>{watcher->mutex};
  for (size_t i = 0; i != numEvents; ++i) {
    const char* path                    = (reinterpret_cast<char**>(eventPaths))[i];
    const FSEventStreamEventFlags flags = eventFlags[i];
    const bool fileModified             = flags & kFSEventStreamEventFlagItemModified;
    if (!fileModified) {
      continue;
    }
    if (flags & kFSEventStreamEventFlagItemIsSymlink) {
      continue; // TODO: Decide if it makes sense to ignore symlinks.
    }
    // Add the path to the queue.
    const size_t pathLen = ::strlen(path);
    if (pathLen + 1 < PATH_MAX) {
      ::memcpy(watcher->eventBuffer[watcher->eventsTail].data(), path, pathLen + 1);
      watcher->eventsTail = (watcher->eventsTail + 1) % g_bufferMaxEntries;
      if (watcher->eventsHead == watcher->eventsTail) {
        watcher->eventsHead = (watcher->eventsHead + 1) % g_bufferMaxEntries;
      }
    }
  }
  watcher->eventsReceived.notify_all();
}

auto eventStreamThread(IOWatcher* watcher) noexcept -> void {
  auto ref = WatcherRef(watcher);

  auto context = FSEventStreamContext{};
  context.info = watcher;

  watcher->eventStream = ::FSEventStreamCreate(
      nullptr,
      &fseventCallback,
      &context,
      watcher->pathsToWatchCF,
      kFSEventStreamEventIdSinceNow,
      g_latency,
      kFSEventStreamCreateFlagFileEvents);
  if (!watcher->eventStream) {
    watcher->error = PlatformError::IOWatcherUnknownError;
    return;
  }

  // Setup the event stream to run on this thread.
  watcher->eventRunLoop = ::CFRunLoopGetCurrent();
  ::FSEventStreamScheduleWithRunLoop(
      watcher->eventStream, watcher->eventRunLoop, kCFRunLoopDefaultMode);
  ::FSEventStreamStart(watcher->eventStream);

  watcher->state = WatcherState::Listening;

  // Execute the 'run loop', blocks until the runloop has been stopped.
  ::CFRunLoopRun();

  watcher->state = WatcherState::Stopped;

  // Free the event-stream resources.
  ::FSEventStreamStop(watcher->eventStream);
  ::FSEventStreamInvalidate(watcher->eventStream);
  ::FSEventStreamRelease(watcher->eventStream);
}

auto stopEventStreamThread(IOWatcher& watcher) noexcept { ::CFRunLoopStop(watcher.eventRunLoop); }

} // namespace

auto ioWatcherCreate(const char* rootPath, IOWatcherFlags flags) noexcept -> IOWatcher* {
  auto* watcher     = new IOWatcher;
  watcher->flags    = flags;
  watcher->state    = WatcherState::Initializing;
  watcher->error    = PlatformError::None;
  watcher->refCount = 1;
  watcher->rootPath = reinterpret_cast<gsl::owner<char*>>(::strdup(rootPath));
  watcher->rootPathCF =
      CFStringCreateWithCString(nullptr, watcher->rootPath, kCFStringEncodingUTF8);
  watcher->pathsToWatchCF = ::CFArrayCreate(
      nullptr, (const void**)&watcher->rootPathCF, 1, &kCFTypeArrayCallBacks); // NOLINT
  watcher->eventsHead = 0;
  watcher->eventsTail = 0;

  const auto startRes = threadStart(&eventStreamThread, watcher);
  if (unlikely(startRes != ThreadStartResult::Success)) {
    // NOTE: Is it worth making a specific error for this?
    watcher->error = PlatformError::IOWatcherUnknownError;
  }
  return watcher;
}

auto ioWatcherGet(
    IOWatcher* watcher, ExecutorHandle* execHandle, StringRef* result, PlatformError* pErr) noexcept
    -> bool {

  auto ref = WatcherRef(watcher);

  /**
   * Wait for an event to be received.
   */
  do {
    execHandle->setState(ExecState::Paused);

    auto shouldAwake = [watcher] {
      return watcher->eventsHead != watcher->eventsTail || watcher->error != PlatformError::None;
    };

    auto lk = std::unique_lock<std::mutex>{watcher->mutex};
    watcher->eventsReceived.wait(lk, shouldAwake);

    execHandle->setState(ExecState::Running);
    if (execHandle->trap()) {
      return false; // Aborted.
    }
    if (watcher->error != PlatformError::None) {
      *pErr = watcher->error;
      result->updateSize(0);
      return false;
    }
    const size_t pathLen = ::strlen(watcher->eventBuffer[watcher->eventsHead].data());
    if (pathLen >= result->getSize()) {
      *pErr = PlatformError::IOWatcherUnknownError;
      result->updateSize(0);
      return false;
    }
    ::memcpy(result->getCharDataPtr(), watcher->eventBuffer[watcher->eventsHead].data(), pathLen);
    result->updateSize(pathLen);
    watcher->eventsHead = (watcher->eventsHead + 1) % g_bufferMaxEntries;
    return true;
  } while (true);
}

auto ioWatcherDestroy(IOWatcher* watcher) noexcept -> void {
  if (watcher->state == WatcherState::Listening) {
    stopEventStreamThread(*watcher);
  }
  watcherRefDec(watcher);
}

} // namespace vm::internal
