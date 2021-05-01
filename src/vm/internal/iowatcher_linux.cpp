
#include "internal/iowatcher.hpp"
#include "internal/os_include.hpp"
#include "internal/platform_utilities.hpp"
#include "internal/ref_allocator.hpp"
#include "internal/ref_string.hpp"
#include <array>
#include <cstring>
#include <sys/inotify.h>
#include <unordered_map>

namespace vm::internal {

namespace {

/**
 * Buffer of how many inotify events to read at a time. If more events then this are buffered up on
 * the os-side then events get dropped.
 */
constexpr size_t g_bufferMaxEntries = 64;
constexpr size_t g_bufferMaxSize    = (sizeof(inotify_event) + NAME_MAX + 1) * g_bufferMaxEntries;

} // namespace

/**
 * Internal helper for tracking a registed inotify watch.
 */
struct Watch {
  int id;
  gsl::owner<char*> path;

  Watch(int id, gsl::owner<char*> path) : id{id}, path{path} {}
  Watch(const Watch& rhs) = delete;
  Watch(Watch&& rhs) noexcept {
    id       = rhs.id;
    path     = rhs.path;
    rhs.path = nullptr;
  }

  ~Watch() { ::free(path); }
};

/**
 * Watcher object, stays alive for the duration of the watcher.
 */
struct IOWatcher {
  IOWatcherFlags flags;
  PlatformError error;

  /**
   * Reference counter.
   * Why is there is reference counter in this garbage collected object? Problem is that when a
   * thread is blocked in the os 'read' call we do not want to block the garbage collector.
   */
  std::atomic<int> refCount;

  /**
   * Only a single thread can perform any operation on the watcher.
   */
  std::mutex mutex;

  std::unordered_map<int, Watch> watches; // map of inotify watchid's to our watch representation.
  int inotifyHandle;

  char* eventsBufferHead;
  char* eventsBufferEnd;
  std::array<char, g_bufferMaxSize> eventsBuffer;
};

namespace {

auto watcherDestroyImpl(IOWatcher* watcher) {
  watcher->watches.clear();
  if (watcher->inotifyHandle != -1) {
    ::close(watcher->inotifyHandle);
  }
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
 * Mask of inotify events to wait for.
 */
auto getMask() noexcept -> uint32_t {
  return IN_CLOSE_WRITE | IN_CREATE | IN_DELETE_SELF | IN_MOVE_SELF | IN_MOVED_TO | IN_DONT_FOLLOW |
      IN_EXCL_UNLINK | IN_ONLYDIR;
}

auto getError() noexcept {
  switch (errno) {
  case EACCES:
  case EPERM:
  case EROFS:
    return PlatformError::FileNoAccess;
  case ETXTBSY:
    return PlatformError::FileLocked;
  case ENOENT:
    return PlatformError::FileNotFound;
  case ENOTDIR:
    return PlatformError::FileIsNotDirectory;
  case ENAMETOOLONG:
    return PlatformError::FilePathTooLong;
  case EINVAL:
    return PlatformError::FileInvalidFileName;
  case EMFILE:
  case ENFILE:
    return PlatformError::FileTooManyOpenFiles;
  case EEXIST:
    return PlatformError::IOWatcherFileAlreadyWatched;
  }
  return PlatformError::IOWatcherUnknownError;
}

/**
 * Helper utility for concatenating two paths with a '/' seperator.
 * NOTE: Returns a malloced string.
 */
auto concatPath(const char* a, const size_t aLen, const char* b, const size_t bLen) noexcept
    -> gsl::owner<char*> {
  const bool needsSeperator = aLen > 0 && *(a + aLen - 1) != '/';

  const size_t len = aLen + needsSeperator + bLen + 1;
  auto result      = reinterpret_cast<gsl::owner<char*>>(::malloc(len));
  ::memcpy(result, a, aLen);
  if (needsSeperator) {
    result[aLen] = '/';
  }
  ::memcpy(result + aLen + needsSeperator, b, bLen);
  result[aLen + needsSeperator + bLen] = '\0';
  return result;
}

auto concatPath(const char* path, const char* name) noexcept {
  return concatPath(path, ::strlen(path), name, ::strlen(name));
}

/**
 * Setup watches for the specified directory and all of its subdirectories.
 * NOTE: Will take ownership of the provided (malloced) path string.
 */
auto setupWatchesReq(IOWatcher& watcher, gsl::owner<char*> path, bool allowMissing) noexcept
    -> void {
  // Open the directory.
  DIR* dir = ::opendir(path);
  if (!dir) {
    if (!allowMissing) {
      watcher.error = getError();
    }
    ::free(path);
    return;
  }

  // Setup a watch for it.
  const int watchId = ::inotify_add_watch(watcher.inotifyHandle, path, getMask());
  if (watchId == -1) {
    watcher.error = getError();
    ::free(path);
    ::closedir(dir);
    return;
  }

  // Update our watch collection (can replace an existing watch with the same id).
  auto watchesItr = watcher.watches.find(watchId);
  if (watchesItr != watcher.watches.end()) {
    ::free(watchesItr->second.path);
    watchesItr->second.path = path;
  } else {
    watcher.watches.insert({watchId, Watch{watchId, path}});
  }

  // Setup watches for its sub-directories.
  const size_t pathLen = ::strlen(path);
  while (struct dirent* dirEnt = ::readdir(dir)) {
    const size_t childNameLen = ::strlen(dirEnt->d_name);

    // Skip the '.' and '..' entries.
    if (dirEnt->d_name[0] == '.' &&
        (childNameLen == 1 || (childNameLen == 2 && dirEnt->d_name[1] == '.'))) {
      continue;
    }
    // Setup watches for any sub directories.
    if (dirEnt->d_type == DT_DIR) {
      setupWatchesReq(watcher, concatPath(path, pathLen, dirEnt->d_name, childNameLen), true);
    }
  }

  ::closedir(dir);
}

/**
 * Read a batch of changes from inotify.
 */
auto readEvents(IOWatcher& watcher, ExecutorHandle* execHandle) -> bool {
  execHandle->setState(ExecState::Paused);

  const auto bytesRead =
      ::read(watcher.inotifyHandle, watcher.eventsBuffer.data(), g_bufferMaxSize);

  execHandle->setState(ExecState::Running);
  if (execHandle->trap()) {
    return false; // Aborted.
  }

  if (bytesRead <= 0) {
    switch (errno) {
    case EAGAIN:
    case EINTR:
      return false;
    }
    watcher.error = getError();
    return false;
  }

  watcher.eventsBufferHead = watcher.eventsBuffer.data();
  watcher.eventsBufferEnd  = watcher.eventsBufferHead + bytesRead;
  return true;
}

auto getNextModifiedFile(IOWatcher& watcher, ExecutorHandle* execHandle, StringRef* result)
    -> bool {

  // Check if we have an event on our buffer, if not ask inotify for a next event batch.
  // NOTE: If no events are in the buffer this will block here.
  const bool hasEvents = watcher.eventsBufferHead || readEvents(watcher, execHandle);
  if (!hasEvents) {
    return false;
  }

  while (watcher.eventsBufferHead) {
    auto* event = reinterpret_cast<struct inotify_event*>(watcher.eventsBufferHead);
    watcher.eventsBufferHead += sizeof(inotify_event) + event->len;
    if (watcher.eventsBufferHead >= watcher.eventsBufferEnd) {
      watcher.eventsBufferHead = nullptr;
    }

    if (event->mask & IN_Q_OVERFLOW) {
      // Getting here means the application is reading the events too slowly and the kernel buffer
      // has filled up. At the moment we just ignore this and the overflowed events get silently
      // dropped, in the future we could consider reporting this error to the application.
      continue;
    }

    // Lookup the watch associated with the watch-id.
    auto lookupItr = watcher.watches.find(event->wd);
    if (lookupItr == watcher.watches.end()) {
      continue;
    }
    auto& watch = lookupItr->second;

    // If the event is a 'close-write' then we return the absolute path of the modified file.
    if (event->mask & IN_CLOSE_WRITE) {
      const size_t dirLen       = ::strlen(watch.path);
      const bool needsSeperator = dirLen > 0 && *(watch.path + dirLen - 1) != '/';
      const size_t nameLen      = ::strlen(event->name);
      const size_t requiredSize = dirLen + needsSeperator + nameLen;
      if (result->getSize() < requiredSize) {
        watcher.error = PlatformError::IOWatcherUnknownError;
        return false;
      }
      ::memcpy(result->getCharDataPtr(), watch.path, dirLen);
      if (needsSeperator) {
        *(result->getCharDataPtr() + dirLen) = '/';
      }
      ::memcpy(result->getCharDataPtr() + dirLen + needsSeperator, event->name, nameLen);
      result->updateSize(requiredSize);
      return true;
    }

    // For created directories we start watching them.
    if ((event->mask & IN_ISDIR) && (event->mask & (IN_CREATE | IN_MOVED_TO))) {
      setupWatchesReq(watcher, concatPath(watch.path, event->name), true);
    }
    // If the OS stops tracking the file then we remove our watch also.
    if (event->mask & IN_IGNORED) {
      watcher.watches.erase(lookupItr);
    }
  }
  return false; // No event available.
}

}; // namespace

auto ioWatcherCreate(const char* rootPath, IOWatcherFlags flags) noexcept -> IOWatcher* {
  auto* watcher             = new IOWatcher;
  watcher->flags            = flags;
  watcher->error            = PlatformError::None;
  watcher->eventsBufferHead = nullptr;
  watcher->refCount         = 1;

  watcher->inotifyHandle = ::inotify_init();
  if (watcher->inotifyHandle == -1) {
    watcher->error = getError();
    return watcher;
  }
  auto owningRootPath = reinterpret_cast<gsl::owner<char*>>(::strdup(rootPath));
  setupWatchesReq(*watcher, owningRootPath, false);
  return watcher;
}

auto ioWatcherGet(
    IOWatcher* watcher, ExecutorHandle* execHandle, StringRef* result, PlatformError* pErr) noexcept
    -> bool {

  auto ref = WatcherRef(watcher);

  // Acquire the watcher lock.
  // NOTE: This can block for a long time as another executor might be already waiting for an event
  // from inotify.
  execHandle->setState(ExecState::Paused);
  std::lock_guard<std::mutex> lk(watcher->mutex);
  execHandle->setState(ExecState::Running);
  if (execHandle->trap()) {
    return false; // Aborted.
  }

  if (watcher->error != PlatformError::None) {
    *pErr = watcher->error;
    result->updateSize(0);
    return false;
  }

  bool fileModified;
  do {
    fileModified = getNextModifiedFile(*watcher, execHandle, result);
    if (execHandle->trap()) {
      return false; // Aborted.
    }
  } while (!fileModified && watcher->error == PlatformError::None);

  if (!fileModified) {
    *pErr = watcher->error;
    result->updateSize(0);
  }
  return fileModified;
}

auto ioWatcherDestroy(IOWatcher* watcher) noexcept -> void { watcherRefDec(watcher); }

} // namespace vm::internal
