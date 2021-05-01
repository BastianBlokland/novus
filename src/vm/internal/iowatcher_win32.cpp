#include "internal/executor_handle.hpp"
#include "internal/iowatcher.hpp"
#include "internal/os_include.hpp"
#include "internal/platform_utilities.hpp"
#include "internal/ref_string.hpp"
#include <atomic>
#include <cstring>
#include <mutex>

namespace vm::internal {

namespace {

/**
 * Buffer of how many windows events to read at a time. If more events then this are buffered up on
 * the os-side then events get dropped.
 */
constexpr size_t g_bufferMaxEntries = 64;
constexpr size_t g_bufferMaxSize    = sizeof(FILE_NOTIFY_INFORMATION) * g_bufferMaxEntries;

/**
 * Deadzone where a another change to the same file is ignored in micro-seconds.
 * Helpfull because windows has no 'close-write' type event so you will get multiple events for the
 * same logical write.
 */
constexpr int64_t g_microSecChangeDeadzone = 100'000;

} // namespace

/**
 * Internal buffer helper.
 */
template <size_t Size>
struct WatcherBuffer {
  WatcherBuffer() {
    head = buffer;
    ::memset(buffer, 0, Size);
  }

  [[nodiscard]] auto data() -> char* { return buffer; }
  [[nodiscard]] auto usedSize() const -> size_t { return head - buffer; }
  [[nodiscard]] auto remainingSize() const -> size_t { return Size - usedSize(); }

  auto setSize(size_t size) {
    assert(size <= Size);
    head = buffer + size;
  }
  auto clear() { head = buffer; }
  auto forward(size_t amount) {
    assert(remainingSize() >= amount);
    head += amount;
  }
  template <size_t OtherSize>
  auto write(const WatcherBuffer<OtherSize>& other) {
    return write(other.buffer, other.usedSize());
  }
  auto writeStr(const char* str) { write(str, ::strlen(str)); }
  auto write(const char* source, size_t amount) {
    assert(remainingSize() >= amount);
    ::memcpy(head, source, amount);
    forward(amount);
  }
  template <size_t OtherSize>
  auto replace(const WatcherBuffer<OtherSize>& other) {
    return replace(other.buffer, other.usedSize());
  }
  auto replace(const char* source, size_t amount) {
    clear();
    write(source, amount);
  }
  template <size_t OtherSize>
  auto equal(const WatcherBuffer<OtherSize>& other) {
    return equal(other.buffer, other.usedSize());
  }
  auto equal(const char* other, size_t otherSize) {
    return otherSize == usedSize() && ::memcmp(buffer, other, otherSize) == 0;
  }
  auto nullTerminate() {
    assert(remainingSize() >= 1);
    *head = '\0';
  }

  auto tryCopyToStr(StringRef* result) const -> bool {
    const size_t size = usedSize();
    if (result->getSize() < size) {
      return false;
    }
    ::memcpy(result->getCharDataPtr(), buffer, size);
    result->updateSize(size);
    return true;
  }

  char* head;
  char buffer[Size];
};

/**
 * Watcher object, stays alive for the duration of the watcher.
 */
struct IOWatcher {
  IOWatcherFlags flags;
  PlatformError error;
  HANDLE dirHandle; // win32 handle to an open directory.

  /**
   * Reference counter.
   * Why is there is reference counter in this garbage collected object? Problem is that when a
   * thread is blocked in the win32 'ReadDirectoryChangesW' call we cannot destruct the object, so
   * it would block vm shutdown or garbage sweeping.
   */
  std::atomic<int> refCount;

  /**
   * Only a single thread can perform any operation on the watcher.
   */
  std::mutex mutex;

  char* event;
  WatcherBuffer<g_bufferMaxSize> eventsBuffer;
  WatcherBuffer<PATH_MAX> rootPath;
  WatcherBuffer<PATH_MAX> lastModPath;
  int64_t lastModTime;
};

namespace {

auto watcherDestroyImpl(IOWatcher* watcher) {
  if (watcher->dirHandle != INVALID_HANDLE_VALUE) {
    ::CloseHandle(watcher->dirHandle);
  }
  delete watcher;
}

// Increment the reference counter.
auto watcherRefInc(IOWatcher* watcher) {
  watcher->refCount.fetch_add(1, std::memory_order_acq_rel);
}

// Decrement the reference counter.
auto watcherRefDec(IOWatcher* watcher) {
  if (watcher->refCount.fetch_sub(1, std::memory_order_acq_rel) == 0) {
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

auto getError() noexcept {
  switch (::GetLastError()) {
  case ERROR_ACCESS_DENIED:
    return PlatformError::FileNoAccess;
  case ERROR_SHARING_VIOLATION:
    return PlatformError::FileLocked;
  case ERROR_FILE_NOT_FOUND:
  case ERROR_PATH_NOT_FOUND:
  case ERROR_INVALID_DRIVE:
    return PlatformError::FileNotFound;
  case ERROR_BUFFER_OVERFLOW:
    return PlatformError::FilePathTooLong;
  case ERROR_DIRECTORY:
    return PlatformError::FileIsNotDirectory;
  case ERROR_INVALID_NAME:
  case ERROR_BAD_PATHNAME:
    return PlatformError::FileInvalidFileName;
  case ERROR_TOO_MANY_OPEN_FILES:
    return PlatformError::FileTooManyOpenFiles;
  }
  return PlatformError::IOWatcherUnknownError;
}

auto openDirHandle(IOWatcher& watcher) noexcept -> void {
  watcher.dirHandle = ::CreateFileA(
      watcher.rootPath.data(),
      FILE_LIST_DIRECTORY,
      FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
      nullptr,
      OPEN_EXISTING,
      FILE_FLAG_BACKUP_SEMANTICS,
      nullptr);
  if (watcher.dirHandle == INVALID_HANDLE_VALUE) {
    watcher.error = getError();
  }
}

template <size_t BufferSize>
auto getFilePath(
    IOWatcher& watcher,
    const WCHAR* fileName,
    size_t fileNameSize,
    WatcherBuffer<BufferSize>& buffer) -> bool {
  assert(watcher.rootPath.usedSize());

  buffer.clear();
  if (buffer.remainingSize() < watcher.rootPath.usedSize() + 2) { // +1 for seperator, +1 for null.
    return false;
  }
  buffer.write(watcher.rootPath);
  if (*(buffer.head - 1) != '/' && *(buffer.head - 1) != '\\') {
    buffer.write("\\", 1);
  }
  const int pathLen = ::WideCharToMultiByte( // Convert fileName to utf8 and add to result.
      CP_UTF8,
      0,
      fileName,
      fileNameSize,
      buffer.head,
      buffer.remainingSize() - 1, // -1 to reserve space for a null-terminator.
      nullptr,
      nullptr);
  if (pathLen == 0) {
    return false;
  }
  buffer.forward(pathLen);
  buffer.nullTerminate();
  return true;
}

/**
 * Read a batch of changes from win32.
 */
auto readEvents(IOWatcher& watcher, ExecutorHandle* execHandle) -> bool {

  watcher.eventsBuffer.clear();
  execHandle->setState(ExecState::Paused);

  DWORD bytesWritten;
  const bool success = ::ReadDirectoryChangesW(
      watcher.dirHandle,
      watcher.eventsBuffer.head,
      watcher.eventsBuffer.remainingSize(),
      true,
      FILE_NOTIFY_CHANGE_LAST_WRITE,
      &bytesWritten,
      nullptr,
      nullptr);

  execHandle->setState(ExecState::Running);
  if (execHandle->trap()) {
    return false; // Aborted.
  }
  if (!success) {
    watcher.error = getError();
    return false; // Failed.
  }
  if (bytesWritten == 0) {
    // TODO: Handle the case that our buffer is too small to receive all available events.
    return false;
  }
  watcher.event = watcher.eventsBuffer.data();
  watcher.eventsBuffer.setSize(bytesWritten);
  return true;
}

/**
 * Wait for a modified file and write the absolute path in the 'result' string-ref.
 */
auto getNextModifiedFile(IOWatcher& watcher, ExecutorHandle* execHandle, StringRef* result)
    -> bool {

  // Check if we have an event on our buffer, if not ask win32 for a next event batch.
  // NOTE: If no events are in the buffer this will block here.
  const bool hasEvents = watcher.event || readEvents(watcher, execHandle);
  if (!hasEvents) {
    return false;
  }

  WatcherBuffer<PATH_MAX> fileBuffer;

  while (watcher.event) {
    auto* event = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(watcher.event);
    if (event->NextEntryOffset) {
      watcher.event = watcher.event + event->NextEntryOffset;
    } else {
      watcher.event = nullptr;
    }

    if (event->FileNameLength > 0 && event->Action == FILE_ACTION_MODIFIED) {
      // Retrieve the absolute path for this event.
      const int numWideChars = static_cast<int>(event->FileNameLength / sizeof(WCHAR));
      if (!getFilePath(watcher, event->FileName, numWideChars, fileBuffer)) {
        watcher.error = PlatformError::IOWatcherUnknownError;
        return false;
      }
      // Retrieve the attributes for that event.
      WIN32_FILE_ATTRIBUTE_DATA fileAttributes;
      if (!::GetFileAttributesExA(fileBuffer.data(), GetFileExInfoStandard, &fileAttributes)) {
        continue; // Filter out files that have since been deleted, or are otherwise unreadable.
      }
      if (fileAttributes.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
        continue; // Filter out directories.
      }
      const int64_t lastModTime = winFileTimeToMicroSinceEpoch(fileAttributes.ftLastWriteTime);
      if (watcher.lastModPath.equal(fileBuffer) &&
          (lastModTime - watcher.lastModTime) < g_microSecChangeDeadzone) {
        // Note: Windows can send multiple events for the write to the same file, so we filter the
        // event if the modification time is less then 50 milliseconds apart from the last.
        continue;
      } else {
        // Save the last modified file path and time.
        watcher.lastModPath.replace(fileBuffer);
        watcher.lastModTime = lastModTime;
      }
      // Copy the absolute path to the result string.
      if (!fileBuffer.tryCopyToStr(result)) {
        watcher.error = PlatformError::IOWatcherUnknownError;
        return false;
      }
      return true;
    }
  }
  return false; // No event available.
}

} // namespace

auto ioWatcherCreate(const char* rootPath, IOWatcherFlags flags) noexcept -> IOWatcher* {
  auto* watcher     = new IOWatcher;
  watcher->flags    = flags;
  watcher->error    = PlatformError::None;
  watcher->refCount = 1;
  watcher->rootPath.writeStr(rootPath);
  watcher->event = nullptr;
  openDirHandle(*watcher);
  return watcher;
}

auto ioWatcherGet(
    IOWatcher* watcher, ExecutorHandle* execHandle, StringRef* result, PlatformError* pErr) noexcept
    -> bool {

  auto ref = WatcherRef(watcher);

  // Acquire the watcher lock.
  // NOTE: This can block for a long time as another executor might be already waiting for a batch
  // of events from win32.
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
