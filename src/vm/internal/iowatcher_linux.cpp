
#include "internal/iowatcher.hpp"
#include "internal/os_include.hpp"
#include "internal/platform_utilities.hpp"
#include "internal/ref_allocator.hpp"
#include "internal/ref_string.hpp"
#include <array>
#include <atomic>
#include <cstring>
#include <sys/inotify.h>
#include <unordered_map>

namespace vm::internal {

namespace {

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

} // namespace

struct IOWatcher {
  IOWatcherFlags flags;
  std::atomic<PlatformError> error;

  std::mutex mutex;
  std::unordered_map<int, Watch> watches;
  int inotifyHandle;
};

namespace {

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

auto concatPath(const char* a, const size_t aLen, const char* b, const size_t bLen) noexcept
    -> gsl::owner<char*> {
  const size_t len = aLen + 1 + bLen + 1;
  auto result      = reinterpret_cast<gsl::owner<char*>>(::malloc(len));
  ::memcpy(result, a, aLen);
  result[aLen] = '/';
  ::memcpy(result + aLen + 1, b, bLen);
  result[aLen + 1 + bLen] = '\0';
  return result;
}

auto concatPath(const char* path, const char* name) noexcept {
  return concatPath(path, ::strlen(path), name, ::strlen(name));
}

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
  int watchId = ::inotify_add_watch(watcher.inotifyHandle, path, getMask());
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

auto getNextModifiedFile(IOWatcher& watcher, ExecutorHandle* execHandle, StringRef* result)
    -> bool {
  constexpr size_t bufferSize = sizeof(struct inotify_event) + NAME_MAX + 1;
  std::array<char, bufferSize> buffer;
  auto* event = reinterpret_cast<struct inotify_event*>(buffer.data());

  execHandle->setState(ExecState::Paused);

  // Read a inotify event.
  const auto bytesRead = ::read(watcher.inotifyHandle, buffer.data(), bufferSize);

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

  // Only a single thread is allowed to perform operations on the watcher object.
  std::lock_guard<std::mutex> lk(watcher.mutex);

  // Lookup the watch accociated with the watch-id.
  auto lookupItr = watcher.watches.find(event->wd);
  if (lookupItr == watcher.watches.end()) {
    return false;
  }
  auto& watch = lookupItr->second;

  if (event->mask & IN_Q_OVERFLOW) {
    // Getting here means the application is reading the events too slowly and the kernel buffer has
    // filled up. At the moment we just ignore this and the overflowed events get silently dropped,
    // in the future we could consider reporting this error to the application.
  }

  // If the event is a 'close-write' then we return the absolute path of the modified file.
  if (event->mask & IN_CLOSE_WRITE) {
    const size_t dirLen       = ::strlen(watch.path);
    const size_t nameLen      = ::strlen(event->name);
    const size_t requiredSize = dirLen + 1 + nameLen;
    if (result->getSize() < requiredSize) {
      watcher.error = PlatformError::IOWatcherUnknownError;
      return false;
    }
    ::memcpy(result->getCharDataPtr(), watch.path, dirLen);
    *(result->getCharDataPtr() + dirLen) = '/';
    ::memcpy(result->getCharDataPtr() + dirLen + 1, event->name, nameLen);
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
  return false;
}

}; // namespace

auto ioWatcherCreate(const char* rootPath, IOWatcherFlags flags) noexcept -> IOWatcher* {
  auto* watcher  = new IOWatcher;
  watcher->flags = flags;
  watcher->error = PlatformError::None;

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

auto ioWatcherDestroy(IOWatcher* watcher) noexcept -> void {
  watcher->watches.clear();
  if (watcher->inotifyHandle != -1) {
    ::close(watcher->inotifyHandle);
  }
  delete watcher;
}

} // namespace vm::internal
