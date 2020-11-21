#pragma once
#include "internal/os_include.hpp"
#include "internal/ref_allocator.hpp"
#include "internal/ref_string.hpp"
#include <atomic>
#include <condition_variable>
#include <mutex>

#if defined(_WIN32)
#define fdopen _fdopen
#endif // _WIN32

namespace vm::internal {

enum class ProcessExitErr : int32_t {
  InvalidProcess = -1, // Process is not valid or was terminated by a signal.
  UnknownErr     = -2, // Unknown error occurred while waiting for a process to finnish.
};

enum class ProcessState : uint8_t {
  Unknown  = 0, // No executor has started waiting for his process.
  Waiting  = 1, // Another executor is already waiting for this process.
  Finished = 2, // Process has finished.
};

enum class ProcessSignalKind : uint8_t {
  Interupt = 0, // Request a process to interupt.
};

#if defined(_WIN32)
using ProcessType = PROCESS_INFORMATION;
#else // !_WIN32
using ProcessType = int;
#endif

inline auto invalidProcess() -> ProcessType {
#if defined(_WIN32)
  return PROCESS_INFORMATION{};
#else // !_WIN32
  return -1;
#endif
};

#if defined(_WIN32)
template <typename... Handles>
auto CloseHandle(Handles... handles) {
  (::CloseHandle(handles), ...);
}
#else // !_WIN32
template <typename... Descriptors>
auto close(Descriptors... descriptors) {
  (::close(descriptors), ...);
}
#endif // !_WIN32

class ProcessRef final : public Ref {
  friend class RefAllocator;

public:
  ProcessRef(const ProcessRef& rhs) = delete;
  ProcessRef(ProcessRef&& rhs)      = delete;
  ~ProcessRef() noexcept {
    // Close our handles to the in / out / err pipes.
    if (m_stdIn) {
      std::fclose(m_stdIn);
    }
    if (m_stdOut) {
      std::fclose(m_stdOut);
    }
    if (m_stdErr) {
      std::fclose(m_stdErr);
    }
    // Kill the process (if its still running).
    if (isValid() && !isFinished()) {
      killImpl();
      // Wait for the process to stop, this prevents leaking zombie processes.
      block();
    }
    // Close any handles we have to the process.
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

  [[nodiscard]] auto getId() noexcept -> int64_t {
    if (!isValid()) {
      return -1ul;
    }
#if defined(_WIN32)
    return m_process.dwProcessId;
#else // !_WIN32
    return m_process;
#endif
  }

  [[nodiscard]] auto getStdIn() const noexcept { return m_stdIn; }
  [[nodiscard]] auto getStdOut() const noexcept { return m_stdOut; }
  [[nodiscard]] auto getStdErr() const noexcept { return m_stdErr; }

  [[nodiscard]] auto isFinished() noexcept -> bool {
    return m_state.load(std::memory_order_acquire) == ProcessState::Finished;
  }

  auto block() noexcept -> int32_t {
    // A process can only be awaited once, so if we are the first executor then we wait for the
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

  auto sendSignal(ProcessSignalKind kind) const noexcept -> bool {
    switch (kind) {
    case ProcessSignalKind::Interupt:
#if defined(_WIN32)
      // NOTE: Send 'CTRL_BREAK' instead of 'CTRL_C' because we cannot send ctrl-c to other process
      // groups (and we don't want to interupt our entire own process-group).
      return GenerateConsoleCtrlEvent(CTRL_BREAK_EVENT, m_process.dwProcessId);
#else // !_WIN32
      return ::kill(m_process, SIGINT) == 0;
#endif
    }
    return false;
  }

private:
  ProcessType m_process;
  std::atomic<ProcessState> m_state;
  std::atomic<int32_t> m_exitCode;
  std::mutex m_mutex;
  std::condition_variable m_condVar;

  gsl::owner<FILE*> m_stdIn;
  gsl::owner<FILE*> m_stdOut;
  gsl::owner<FILE*> m_stdErr;

  ProcessRef(
      ProcessType process,
      gsl::owner<FILE*> stdIn,
      gsl::owner<FILE*> stdOut,
      gsl::owner<FILE*> stdErr) noexcept :
      Ref(getKind()),
      m_process{process},
      m_state{ProcessState::Unknown},
      m_stdIn{stdIn},
      m_stdOut{stdOut},
      m_stdErr{stdErr} {}

  ProcessRef(ProcessType process, int stdInDesc, int stdOutDesc, int stdErrDesc) noexcept :
      ProcessRef{
          process, fdopen(stdInDesc, "a"), fdopen(stdOutDesc, "r"), fdopen(stdErrDesc, "r")} {};

#if defined(_WIN32)
  ProcessRef(ProcessType process, HANDLE stdIn, HANDLE stdOut, HANDLE stdErr) noexcept :
      ProcessRef{
          process,
          _open_osfhandle(reinterpret_cast<intptr_t>(stdIn), _O_APPEND),
          _open_osfhandle(reinterpret_cast<intptr_t>(stdOut), _O_RDONLY),
          _open_osfhandle(reinterpret_cast<intptr_t>(stdErr), _O_RDONLY)} {};
#endif

  auto killImpl() -> void {
#if defined(_WIN32)
    TerminateProcess(m_process.hProcess, static_cast<UINT>(ProcessExitErr::InvalidProcess));
#else // !_WIN32
    ::kill(m_process, SIGKILL);
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
      return static_cast<int32_t>(ProcessExitErr::InvalidProcess);
    }
    return static_cast<int32_t>(ProcessExitErr::UnknownErr);
#endif
  }
};

inline auto processStart(RefAllocator* alloc, const StringRef* cmdLineStr) -> ProcessRef* {
#if defined(_WIN32)
  // Make a local copy of the command-line string for preprocessing.
  // Double the size as we need to escape characters.
  char* localStr = static_cast<char*>(malloc(cmdLineStr->getSize() * 2));
  memcpy(localStr, cmdLineStr->getCharDataPtr(), cmdLineStr->getSize() + 1); // +1 for null term.

  char* cmdLineStrStart = nullptr;

  // Preprocess the command-line string:
  // * Remove starting whitespace.
  // * Replace single quote by double quotes.
  // * Escape double quotes by prefixing with a backslash.
  for (char* itr = localStr;; ++itr) {
    switch (*itr) {
    case '\0':
      break;
    case ' ':
    case '\t':
    case '\n':
    case '\r':
      continue;
    default:
      if (itr[0] == '"') {
        memmove(itr + 1, itr, strlen(itr) + 1);
        *itr++ = '\\';
      } else if (itr[0] == '\'') {
        *itr = '"';
      } else if (itr[0] == '\\' && itr[1] == '\'') {
        // Treat backslash-quote as a normal quote (remove the backslash).
        memmove(itr, itr + 1, strlen(itr));
        ++itr;
      }
      if (!cmdLineStrStart) {
        cmdLineStrStart = itr;
      }
      continue;
    }
    break;
  }

  // Create pipes to read from / write to the child process stdIn, stdOut and stdErr streams.
  SECURITY_ATTRIBUTES saAttr = {};
  saAttr.nLength             = sizeof(SECURITY_ATTRIBUTES);
  saAttr.bInheritHandle      = true;

  std::array<HANDLE, 2> pipeStdIn, pipeStdOut, pipeStdErr;
  if (!CreatePipe(&pipeStdIn[0], &pipeStdIn[1], &saAttr, 0) ||        // Create stdIn pipe.
      !SetHandleInformation(pipeStdIn[1], HANDLE_FLAG_INHERIT, 0) ||  // Mark parentside noninherit.
      !CreatePipe(&pipeStdOut[0], &pipeStdOut[1], &saAttr, 0) ||      // Create stdOut pipe.
      !SetHandleInformation(pipeStdOut[0], HANDLE_FLAG_INHERIT, 0) || // Mark parentside noninherit.
      !CreatePipe(&pipeStdErr[0], &pipeStdErr[1], &saAttr, 0) ||      // Create stdErr pipe.
      !SetHandleInformation(pipeStdErr[0], HANDLE_FLAG_INHERIT, 0))   // Mark parentside noninherit.
  {
    // Failed to create all the pipes, close whichever child pipes where created.
    CloseHandle(pipeStdIn[0], pipeStdOut[1], pipeStdErr[1]);
    return alloc->allocPlain<ProcessRef>(
        invalidProcess(), pipeStdIn[1], pipeStdOut[0], pipeStdErr[0]);
  }

  STARTUPINFO startupInfo = {};
  startupInfo.cb          = sizeof(STARTUPINFO);
  startupInfo.hStdInput   = pipeStdIn[0];
  startupInfo.hStdOutput  = pipeStdOut[1];
  startupInfo.hStdError   = pipeStdErr[1];
  startupInfo.dwFlags |= STARTF_USESTDHANDLES;

  PROCESS_INFORMATION processInfo = {};

  bool success = CreateProcess(
      nullptr,
      cmdLineStrStart,
      nullptr,
      nullptr,
      true,
      NORMAL_PRIORITY_CLASS | CREATE_NEW_PROCESS_GROUP,
      nullptr,
      nullptr,
      &startupInfo,
      &processInfo);
  free(localStr); // Free our local copy of the command-line string.

  // Close the child side of the pipes.
  CloseHandle(pipeStdIn[0], pipeStdOut[1], pipeStdErr[1]);
  return alloc->allocPlain<ProcessRef>(
      success ? processInfo : invalidProcess(), pipeStdIn[1], pipeStdOut[0], pipeStdErr[0]);

#else // !_WIN32

  // Create pipes to communicate with the child process.
  std::array<int, 2> pipeStdIn, pipeStdOut, pipeStdErr;
  if (pipe(pipeStdIn.data()) || pipe(pipeStdOut.data()) || pipe(pipeStdErr.data())) {
    // Failed to create all the pipes, close whichever child pipes where created.
    close(pipeStdIn[0], pipeStdOut[1], pipeStdErr[1]);
    return alloc->allocPlain<ProcessRef>(
        invalidProcess(), pipeStdIn[1], pipeStdOut[0], pipeStdErr[0]);
  }

  const auto childPid = fork();
  switch (childPid) {
  case 0: {
    // Executed on child process:

    // Close the parent side of the pipes.
    close(pipeStdIn[1], pipeStdOut[0], pipeStdErr[0]);

    // Duplicate the child side of the pipes onto the stdIn, stdOut and stdErr of this process.
    dup2(pipeStdIn[0], 0);
    dup2(pipeStdOut[1], 1);
    dup2(pipeStdErr[1], 2);

    // Split the given cmdLineStr on whitespace into a null-terminted array of c-strings.
    char* itr = cmdLineStr->getCharDataPtr();
    auto argV = std::vector<char*>{};
    for (bool wordStart = true, quoted = false; *itr; ++itr) {
      switch (*itr) {
      case '\'':
        quoted = !quoted;
        [[fallthrough]];
      case ' ':
      case '\t':
      case '\n':
      case '\r':
        if (!quoted) {
          *itr      = '\0'; // Null-terminate the previous word.
          wordStart = true;
        }
        break;
      case '\\':
        if (itr[1] == '\'') {
          // Treat backslash-quote as a normal quote (remove the backslash).
          memmove(itr, itr + 1, strlen(itr));
          ++itr;
        }
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
    // Close the child side of the pipes.
    close(pipeStdIn[0], pipeStdOut[1], pipeStdErr[1]);
    return alloc->allocPlain<ProcessRef>(childPid, pipeStdIn[1], pipeStdOut[0], pipeStdErr[0]);
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

inline auto processSendSignal(ProcessRef* process, ProcessSignalKind kind) {
  assert(process);
  return process->isValid() && process->sendSignal(kind);
}

inline auto processGetId(ProcessRef* process) {
  assert(process);
  return process->getId();
}

inline auto getProcessRef(const Value& val) noexcept { return val.getDowncastRef<ProcessRef>(); }

} // namespace vm::internal
