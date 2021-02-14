#pragma once
#include "internal/platform_utilities.hpp"
#include "internal/ref_process.hpp"
#include "internal/stream_opts.hpp"

namespace vm::internal {

enum class ProcessStreamKind : uint8_t {
  StdIn  = 0,
  StdOut = 1,
  StdErr = 2,
};

auto getProcessStreamPlatformError() noexcept -> PlatformError;

// Implementation of the 'stream' interface that reads / writes to streams from other processes.
// Note: To avoid needing a vtable there is no abstract 'Stream' class but instead there are wrapper
// functions that dispatch based on the 'RefKind' (see stream_utilities.hpp).
class ProcessStreamRef final : public Ref {
  friend class RefAllocator;

public:
  ProcessStreamRef(const ProcessStreamRef& rhs) = delete;
  ProcessStreamRef(ProcessStreamRef&& rhs)      = delete;
  ~ProcessStreamRef() noexcept                  = default;

  auto operator=(const ProcessStreamRef& rhs) -> ProcessStreamRef& = delete;
  auto operator=(ProcessStreamRef&& rhs) -> ProcessStreamRef& = delete;

  [[nodiscard]] constexpr static auto getKind() { return RefKind::StreamProcess; }

  [[nodiscard]] auto getProcess() noexcept { return m_process; }

  [[nodiscard]] auto isValid() noexcept -> bool { return fileIsValid(getFile()); }

  auto readString(ExecutorHandle* execHandle, PlatformError* pErr, StringRef* str) noexcept
      -> bool {
    if (unlikely(m_streamKind == ProcessStreamKind::StdIn)) {
      *pErr = PlatformError::StreamReadNotSupported;
      return false;
    }

    if (unlikely(str->getSize() == 0)) {
      return true;
    }

    execHandle->setState(ExecState::Paused);

    const int bytesRead = fileRead(getFile(), str->getCharDataPtr(), str->getSize());

    execHandle->setState(ExecState::Running);
    if (execHandle->trap()) {
      return false; // Aborted.
    }

    if (bytesRead < 0) {
      *pErr = getProcessStreamPlatformError();
      str->updateSize(0);
      return false;
    }
    if (bytesRead == 0) {
      *pErr = PlatformError::StreamNoDataAvailable;
      str->updateSize(0);
      return false;
    }
    str->updateSize(bytesRead);
    return bytesRead > 0;
  }

  auto readChar(ExecutorHandle* execHandle, PlatformError* pErr) noexcept -> char {
    if (unlikely(m_streamKind == ProcessStreamKind::StdIn)) {
      *pErr = PlatformError::StreamReadNotSupported;
      return false;
    }

    // Can block so we mark ourselves as paused so the gc can trigger in the mean time.
    execHandle->setState(ExecState::Paused);

    char res;
    const int bytesRead = fileRead(getFile(), &res, 1);

    execHandle->setState(ExecState::Running);
    if (execHandle->trap()) {
      return '\0'; // Aborted.
    }

    if (bytesRead != 1) {
      *pErr = getProcessStreamPlatformError();
      return '\0';
    }
    if (bytesRead == 0) {
      *pErr = PlatformError::StreamNoDataAvailable;
      return '\0';
    }
    return res;
  }

  auto writeString(ExecutorHandle* execHandle, PlatformError* pErr, StringRef* str) noexcept
      -> bool {
    if (unlikely(m_streamKind != ProcessStreamKind::StdIn)) {
      *pErr = PlatformError::StreamWriteNotSupported;
      return false;
    }

    execHandle->setState(ExecState::Paused);

    const int bytesWritten = fileWrite(getFile(), str->getCharDataPtr(), str->getSize());

    execHandle->setState(ExecState::Running);
    if (execHandle->trap()) {
      return false; // Aborted.
    }

    if (bytesWritten != static_cast<int>(str->getSize())) {
      *pErr = getProcessStreamPlatformError();
      return false;
    }
    return true;
  }

  auto writeChar(ExecutorHandle* execHandle, PlatformError* pErr, uint8_t val) noexcept -> bool {
    if (unlikely(m_streamKind != ProcessStreamKind::StdIn)) {
      *pErr = PlatformError::StreamWriteNotSupported;
      return false;
    }

    // Can block so we mark ourselves as paused so the gc can trigger in the mean time.
    execHandle->setState(ExecState::Paused);

    auto* valChar          = static_cast<char*>(static_cast<void*>(&val));
    const int bytesWritten = fileWrite(getFile(), valChar, 1);

    execHandle->setState(ExecState::Running);
    if (execHandle->trap()) {
      return false; // Aborted.
    }

    if (bytesWritten != 1) {
      *pErr = getProcessStreamPlatformError();
      return false;
    }
    return true;
  }

  auto flush(PlatformError* /*unused*/) noexcept -> bool {
    // At the moment this is a no-op, in the future we can consider adding additional buffering to
    // console streams (so flush would write to the handle).
    return true;
  }

  auto setOpts(PlatformError* pErr, StreamOpts /*unused*/) noexcept -> bool {
    // On unix we could implement non-blocking by setting the file-descriptor to be non-blocking,
    // but this is not something we can implement on windows.
    *pErr = PlatformError::StreamOptionsNotSupported;
    return false;
  }

  auto unsetOpts(PlatformError* pErr, StreamOpts /*unused*/) noexcept -> bool {
    *pErr = PlatformError::StreamOptionsNotSupported;
    return false;
  }

private:
  ProcessRef* m_process;
  ProcessStreamKind m_streamKind;

  ProcessStreamRef(ProcessRef* process, ProcessStreamKind streamKind) noexcept :
      Ref{getKind()}, m_process{process}, m_streamKind{streamKind} {}

  [[nodiscard]] auto getFile() noexcept -> FileHandle {
    switch (m_streamKind) {
    case ProcessStreamKind::StdIn:
      return m_process->getStdIn();
    case ProcessStreamKind::StdOut:
      return m_process->getStdOut();
    case ProcessStreamKind::StdErr:
      return m_process->getStdErr();
    }
    return fileInvalid();
  }
};

inline auto openProcessStream(ProcessRef* process, RefAllocator* alloc, ProcessStreamKind kind)
    -> ProcessStreamRef* {
  return alloc->allocPlain<ProcessStreamRef>(process, kind);
}

inline auto getProcessStreamPlatformError() noexcept -> PlatformError {
#if defined(_WIN32)
  switch (::GetLastError()) {
  case ERROR_BROKEN_PIPE:
    return PlatformError::StreamNoDataAvailable;
  }
#else // !(_WIN32
  switch (errno) {
  case EPIPE:
    return PlatformError::StreamNoDataAvailable;
  }
#endif
  return PlatformError::ConsoleUnknownError;
}

} // namespace vm::internal
