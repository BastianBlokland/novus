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

  [[nodiscard]] auto isValid() noexcept -> bool { return getFile() != nullptr; }

  auto readString(ExecutorHandle* execHandle, PlatformError* /*unused*/, StringRef* str) noexcept
      -> bool {
    if (unlikely(str->getSize() == 0)) {
      return false;
    }

    // Can block so we mark ourselves as paused so the gc can trigger in the mean time.
    execHandle->setState(ExecState::Paused);

    const auto bytesRead = std::fread(str->getCharDataPtr(), 1U, str->getSize(), getFile());

    // After resuming check if we should wait for gc (or if we are aborted).
    execHandle->setState(ExecState::Running);
    if (execHandle->trap()) {
      return false;
    }

    str->updateSize(bytesRead);
    return bytesRead > 0;
  }

  auto readChar(ExecutorHandle* execHandle, PlatformError* /*unused*/) noexcept -> char {
    // Can block so we mark ourselves as paused so the gc can trigger in the mean time.
    execHandle->setState(ExecState::Paused);

    const auto res = std::getc(getFile());

    // After resuming check if we should wait for gc (or if we are aborted).
    execHandle->setState(ExecState::Running);
    if (execHandle->trap()) {
      return '\0';
    }

    return res > 0 ? static_cast<char>(res) : '\0';
  }

  auto writeString(ExecutorHandle* execHandle, PlatformError* /*unused*/, StringRef* str) noexcept
      -> bool {
    // Can block so we mark ourselves as paused so the gc can trigger in the mean time.
    execHandle->setState(ExecState::Paused);

    const auto res = std::fwrite(str->getDataPtr(), str->getSize(), 1, getFile()) == 1;

    // After resuming check if we should wait for gc (or if we are aborted).
    execHandle->setState(ExecState::Running);
    if (execHandle->trap()) {
      return false;
    }

    return res;
  }

  auto writeChar(ExecutorHandle* execHandle, PlatformError* /*unused*/, uint8_t val) noexcept
      -> bool {
    // Can block so we mark ourselves as paused so the gc can trigger in the mean time.
    execHandle->setState(ExecState::Paused);

    const auto res = std::fputc(val, getFile()) == val;

    // After resuming check if we should wait for gc (or if we are aborted).
    execHandle->setState(ExecState::Running);
    if (execHandle->trap()) {
      return false;
    }

    return res;
  }

  auto flush() noexcept -> bool { return std::fflush(getFile()) == 0; }

  auto setOpts(PlatformError* /*unused*/, StreamOpts /*unused*/) noexcept -> bool {
    // On unix we could implement non-blocking by setting the file-descriptor to be non-blocking,
    // but this is not something we can implement on windows.
    return false;
  }

  auto unsetOpts(PlatformError* /*unused*/, StreamOpts /*unused*/) noexcept -> bool {
    return false;
  }

private:
  ProcessRef* m_process;
  ProcessStreamKind m_streamKind;

  ProcessStreamRef(ProcessRef* process, ProcessStreamKind streamKind) noexcept :
      Ref{getKind()}, m_process{process}, m_streamKind{streamKind} {}

  [[nodiscard]] auto getFile() noexcept -> FILE* {
    switch (m_streamKind) {
    case ProcessStreamKind::StdIn:
      return m_process->getStdIn();
    case ProcessStreamKind::StdOut:
      return m_process->getStdOut();
    case ProcessStreamKind::StdErr:
      return m_process->getStdErr();
    }
    return nullptr;
  }
};

inline auto openProcessStream(ProcessRef* process, RefAllocator* alloc, ProcessStreamKind kind)
    -> ProcessStreamRef* {
  return alloc->allocPlain<ProcessStreamRef>(process, kind);
}

} // namespace vm::internal
