#pragma once
#include "internal/os_include.hpp"
#include "internal/ref_allocator.hpp"
#include "internal/ref_string.hpp"
#include <atomic>
#include <condition_variable>
#include <mutex>

namespace vm::internal {

enum class ProcessExitErr : int32_t {
  InvalidProcess    = -1, // Process is not valid.
  ProcessTerminated = -2, // Process was terminated by a signal.
  UnknownErr        = -3, // Unknown error occurred while waiting for a process to finnish.
};

enum class ProcessState : int8_t {
  Unknown  = 0, // No executor has started waiting for his process.
  Waiting  = 1, // Another executor is already waiting for this process.
  Finished = 2, // Process has finished.
};

#if defined(_WIN32)
using ProcessType = PROCESS_INFORMATION;
#else // !_WIN32
using ProcessType = int;
#endif

class ProcessRef final : public Ref {
  friend class RefAllocator;

public:
  ProcessRef(const ProcessRef& rhs) = delete;
  ProcessRef(ProcessRef&& rhs)      = delete;
  ~ProcessRef() noexcept {
    if (isValid() && !isFinished()) {
      killImpl();
    }
#if defined(_WIN32)
    if (m_process.hThread) {
      CloseHandle(m_process.hThread);
    }
    if (m_process.hProcess) {
      CloseHandle(m_process.hProcess);
    }
#endif
  }

  auto operator=(const ProcessRef& rhs) -> ProcessRef& = delete;
  auto operator=(ProcessRef&& rhs) -> ProcessRef& = delete;

  [[nodiscard]] constexpr static auto getKind() { return RefKind::Process; }

  [[nodiscard]] auto isValid() noexcept -> bool {
#if defined(_WIN32)
    return m_process.hProcess != nullptr;
#else // !_WIN32
    return m_process > 0;
#endif
  }

  [[nodiscard]] auto isFinished() noexcept -> bool {
    return m_state.load(std::memory_order_acquire) == ProcessState::Finished;
  }

  [[nodiscard]] auto block() noexcept -> int32_t {
    // A processes can only be awaited once, so if we are the first executor then we wait for the
    // process, otherwise we wait for the other executor to finish waiting.
    switch (m_state.load(std::memory_order_relaxed)) {
    case ProcessState::Finished:
      // We've already waited on this process before: just return the exit-code.
      return m_exitCode.load(std::memory_order_acquire);
    case ProcessState::Waiting: {
      // Another executor is already waiting on this process: wait until it has finished.
      auto lk = std::unique_lock<std::mutex>{m_mutex};
      m_condVar.wait(
          lk, [this] { return m_state.load(std::memory_order_relaxed) == ProcessState::Finished; });
      return m_exitCode.load(std::memory_order_acquire);
    }
    case ProcessState::Unknown: {
      {
        auto lk = std::lock_guard<std::mutex>{m_mutex};
        if (m_state.load(std::memory_order_acquire) != ProcessState::Unknown) {
          // Another executor beat us to it, recurse to wait for it.
          return block();
        }
        m_state.store(ProcessState::Waiting, std::memory_order_release);
      }
      const auto result = blockImpl();
      {
        auto lk = std::lock_guard<std::mutex>{m_mutex};
        m_exitCode.store(result, std::memory_order_release);
        m_state.store(ProcessState::Finished, std::memory_order_release);
        m_condVar.notify_all();
      }
      return result;
    }
    }
    assert(false);
    return -1;
  }

private:
  ProcessType m_process;
  std::atomic<ProcessState> m_state;
  std::atomic<int32_t> m_exitCode;
  std::mutex m_mutex;
  std::condition_variable m_condVar;

  inline explicit ProcessRef(ProcessType process) noexcept :
      Ref(getKind()), m_process{process}, m_state{ProcessState::Unknown} {}

  auto killImpl() -> void {
#if defined(_WIN32)
    TerminateProcess(m_process.hProcess, static_cast<UINT>(ProcessExitErr::ProcessTerminated));
#else // !_WIN32
    ::kill(m_process, SIGTERM);
#endif
  }

  // Block until the process has finished.
  // NOTE: Can only be called once per process.
  auto blockImpl() -> int32_t {
#if defined(_WIN32)
    WaitForSingleObject(m_process.hProcess, INFINITE);
    DWORD exitCode;
    if (!GetExitCodeProcess(m_process.hProcess, &exitCode)) {
      return static_cast<int32_t>(ProcessExitErr::UnknownErr);
    }
    return static_cast<int32_t>(exitCode);
#else // !_WIN32
    int status;
    int res = waitpid(m_process, &status, 0);
    if (res != m_process) {
      // Failed to wait for the process, most likely because it was awaited before.
      return static_cast<int32_t>(ProcessExitErr::UnknownErr);
    }
    if (WIFEXITED(status)) {
      return WEXITSTATUS(status);
    }
    if (WIFSIGNALED(status)) {
      return static_cast<int32_t>(ProcessExitErr::ProcessTerminated);
    }
    return static_cast<int32_t>(ProcessExitErr::UnknownErr);
#endif
  }
};

inline auto processStart(RefAllocator* alloc, const StringRef* cmdLineStr) -> ProcessRef* {
#if defined(_WIN32)
  char* cmdLineStrItr = cmdLineStr->getCharDataPtr();
  // Skip starting whitespace, reason is to be more consistent with how we handle the input on unix.
  for (;; ++cmdLineStrItr) {
    switch (*cmdLineStrItr) {
    case ' ':
    case '\t':
    case '\n':
    case '\r':
      continue;
    }
    break;
  }
  STARTUPINFO startupInfo         = {};
  PROCESS_INFORMATION processInfo = {};
  if (!CreateProcess(
          nullptr,
          cmdLineStrItr,
          nullptr,
          nullptr,
          false,
          NORMAL_PRIORITY_CLASS | CREATE_NO_WINDOW,
          nullptr,
          nullptr,
          &startupInfo,
          &processInfo)) {
    // Failed to create process.
    return alloc->allocPlain<ProcessRef>(PROCESS_INFORMATION{});
  }
  return alloc->allocPlain<ProcessRef>(processInfo);
#else // !_WIN32
  const auto childPid = fork();
  switch (childPid) {
  case 0: {
    // Executed on child process:
    // Split the given cmdLineStr on whitespace into a null-terminted array of c-strings.
    char* itr = cmdLineStr->getCharDataPtr();
    auto argV = std::vector<char*>{};
    for (bool wordStart = true, quoted = false; *itr; ++itr) {
      switch (*itr) {
      case ' ':
      case '\t':
      case '\n':
      case '\r':
        if (!quoted) {
          *itr      = '\0'; // Null-terminate the previous word.
          wordStart = true;
        }
        break;
      case '\'':
        quoted = !quoted;
        [[fallthrough]];
      default:
        if (wordStart) {
          argV.push_back(itr);
          wordStart = false;
        }
        break;
      }
    }
    argV.push_back(nullptr); // Terminating null pointer.

    // Start executing the target program.
    if (argV[0]) {
      execvp(argV[0], argV.data());
    }

    // Only reachable when exec failed.
    std::fprintf(stderr, "Failed to start process: '%s'\n", argV[0] ? argV[0] : "");
    std::fflush(stderr);
    std::abort();
  }
  default:
    // Note: Pid is -1 in-case fork fails.
    return alloc->allocPlain<ProcessRef>(childPid);
  }
#endif
}

inline auto processBlock(ProcessRef* process) {
  assert(process);
  if (!process->isValid()) {
    return static_cast<int32_t>(ProcessExitErr::InvalidProcess);
  }
  return process->block();
}

inline auto getProcessRef(const Value& val) noexcept { return val.getDowncastRef<ProcessRef>(); }

} // namespace vm::internal
