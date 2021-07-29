#pragma once
#include "internal/os_include.hpp"
#include "internal/platform_utilities.hpp"
#include "internal/ref_allocator.hpp"
#include "internal/ref_string.hpp"
#include "vm/file.hpp"
#include <atomic>
#include <cassert>
#include <condition_variable>
#include <mutex>

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
  Kill     = 1, // Request a process to die.
};

enum ProcessFlags : uint8_t {
  ProcessPipeStdIn  = 1u << 0, // Create a pipe for writing to std in.
  ProcessPipeStdOut = 1u << 1, // Create a pipe for reading from std out.
  ProcessPipeStdErr = 1u << 2, // Create a pipe for reading from std err.
  ProcessNewGroup   = 1u << 3, // Create a new process group for the child proccess.
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

class ProcessRef final : public Ref {
  friend class RefAllocator;

public:
  ProcessRef(const ProcessRef& rhs) = delete;
  ProcessRef(ProcessRef&& rhs)      = delete;
  ~ProcessRef() noexcept {
    // Kill the process (if its still running).
    if (isValid() && !isFinished()) {
      killImpl();
      // Wait for the process to stop, this prevents leaking zombie processes.
      block();
    }
    // Close our handles to the in / out / err pipes if they are open.
    if (fileIsValid(m_stdIn)) {
      fileClose(m_stdIn);
    }
    if (fileIsValid(m_stdOut)) {
      fileClose(m_stdOut);
    }
    if (fileIsValid(m_stdErr)) {
      fileClose(m_stdErr);
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
      return -1l;
    }
#if defined(_WIN32)
    return m_process.dwProcessId;
#else // !_WIN32
    return m_process;
#endif
  }

  [[nodiscard]] auto getStdInPipe() const noexcept { return m_stdIn; }
  [[nodiscard]] auto getStdOutPipe() const noexcept { return m_stdOut; }
  [[nodiscard]] auto getStdErrPipe() const noexcept { return m_stdErr; }

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

  [[nodiscard]] auto sendSignal(PlatformError* pErr, ProcessSignalKind kind) noexcept -> bool {
    switch (kind) {
    case ProcessSignalKind::Interupt:
#if defined(_WIN32)
      // NOTE: Send 'CTRL_BREAK' instead of 'CTRL_C' because we cannot send ctrl-c to other process
      // groups (and we don't want to interupt our entire own process-group).
      if (!GenerateConsoleCtrlEvent(CTRL_BREAK_EVENT, m_process.dwProcessId)) {
        *pErr = PlatformError::ProcessUnknownError;
        return false;
      }
#else // !_WIN32
      if (::killpg(m_process, SIGINT)) {
        *pErr = PlatformError::ProcessUnknownError;
        return false;
      }
#endif
      return true;
    case ProcessSignalKind::Kill:
      if (!killImpl()) {
        *pErr = PlatformError::ProcessUnknownError;
        return false;
      }
      return true;
    }
    *pErr = PlatformError::ProcessInvalidSignal;
    return false;
  }

private:
  ProcessType m_process;
  ProcessFlags m_flags;
  std::atomic<ProcessState> m_state;
  std::atomic<int32_t> m_exitCode;
  std::mutex m_mutex;
  std::condition_variable m_condVar;

  FileHandle m_stdIn;
  FileHandle m_stdOut;
  FileHandle m_stdErr;

  ProcessRef(
      ProcessType process,
      ProcessFlags flags,
      FileHandle stdIn,
      FileHandle stdOut,
      FileHandle stdErr) noexcept :
      Ref(getKind()),
      m_process{process},
      m_flags{flags},
      m_state{ProcessState::Unknown},
      m_stdIn{stdIn},
      m_stdOut{stdOut},
      m_stdErr{stdErr} {}

  auto killImpl() noexcept -> bool {
#if defined(_WIN32)
    return ::TerminateProcess(m_process.hProcess, 9999); // TODO: Decide what exitcode to use here.
#else
    return ::killpg(m_process, SIGKILL) == 0;
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
    assert(exitCode != STILL_ACTIVE);
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

inline auto processStart(
    RefAllocator* alloc, PlatformError* pErr, const StringRef* cmdLineStr, ProcessFlags flags)
    -> ProcessRef* {

  if (cmdLineStr->getSize() == 0) {
    *pErr = PlatformError::ProcessInvalidCmdLine;
    return alloc->allocPlain<ProcessRef>(
        invalidProcess(), flags, fileInvalid(), fileInvalid(), fileInvalid());
  }

  using Pipe                    = std::array<FileHandle, 2>;
  static const Pipe invalidPipe = {fileInvalid(), fileInvalid()};

  // Create pipes to communicate with the child process.
  Pipe pipeStdIn = invalidPipe, pipeStdOut = invalidPipe, pipeStdErr = invalidPipe;

#if defined(_WIN32)
  // Make a local copy of the command-line string for preprocessing.
  // Double the size as we need to escape characters.
  char* localStr = static_cast<char*>(::malloc(cmdLineStr->getSize() * 2));
  ::memcpy(localStr, cmdLineStr->getCharDataPtr(), cmdLineStr->getSize() + 1); // +1 for null term.

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
        ::memmove(itr + 1, itr, ::strlen(itr) + 1);
        *itr++ = '\\';
      } else if (itr[0] == '\'') {
        *itr = '"';
      } else if (itr[0] == '\\' && itr[1] == '\'') {
        // Treat backslash-quote as a normal quote (remove the backslash).
        ::memmove(itr, itr + 1, ::strlen(itr));
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

  bool pipeFailed = false;
  pipeFailed |= flags & ProcessPipeStdIn && !CreatePipe(&pipeStdIn[0], &pipeStdIn[1], &saAttr, 0);
  pipeFailed |=
      flags & ProcessPipeStdOut && !CreatePipe(&pipeStdOut[0], &pipeStdOut[1], &saAttr, 0);
  pipeFailed |=
      flags & ProcessPipeStdErr && !CreatePipe(&pipeStdErr[0], &pipeStdErr[1], &saAttr, 0);
  if (pipeFailed) {
    // Failed to create all the pipes, close whichever child pipes where created.
    fileClose(pipeStdIn[0], pipeStdOut[1], pipeStdErr[1]);

    ::free(localStr); // Free our local copy of the command-line string.

    *pErr = PlatformError::ProcessFailedToCreatePipes;
    return alloc->allocPlain<ProcessRef>(
        invalidProcess(), flags, pipeStdIn[1], pipeStdOut[0], pipeStdErr[0]);
  }

  size_t attributeListSize;
  ::InitializeProcThreadAttributeList(nullptr, 1, 0, &attributeListSize);
  auto* attributeList = static_cast<LPPROC_THREAD_ATTRIBUTE_LIST>(::malloc(attributeListSize));
  if (attributeList) {
    ::InitializeProcThreadAttributeList(attributeList, 1, 0, &attributeListSize);
  }

  std::array handlesToInherit = {pipeStdIn[0], pipeStdOut[1], pipeStdErr[1]};
  if (attributeList && flags & (ProcessPipeStdIn | ProcessPipeStdOut | ProcessPipeStdErr)) {
    ::UpdateProcThreadAttribute(
        attributeList,
        0,
        PROC_THREAD_ATTRIBUTE_HANDLE_LIST,
        handlesToInherit.data(),
        handlesToInherit.size() * sizeof(HANDLE),
        nullptr,
        nullptr);
  }

  STARTUPINFOEX startupInfoEx          = {};
  startupInfoEx.StartupInfo.cb         = sizeof(startupInfoEx);
  startupInfoEx.StartupInfo.hStdInput  = pipeStdIn[0];
  startupInfoEx.StartupInfo.hStdOutput = pipeStdOut[1];
  startupInfoEx.StartupInfo.hStdError  = pipeStdErr[1];
  startupInfoEx.lpAttributeList        = attributeList;

  if (flags & (ProcessPipeStdIn | ProcessPipeStdOut | ProcessPipeStdErr)) {
    startupInfoEx.StartupInfo.dwFlags |= STARTF_USESTDHANDLES;
  }

  PROCESS_INFORMATION processInfo = {};

  DWORD creationFlags = NORMAL_PRIORITY_CLASS | EXTENDED_STARTUPINFO_PRESENT;
  if (flags & ProcessNewGroup) {
    creationFlags |= CREATE_NEW_PROCESS_GROUP;
  }

  const bool success = ::CreateProcess(
      nullptr,
      cmdLineStrStart,
      nullptr,
      nullptr,
      true,
      creationFlags,
      nullptr,
      nullptr,
      &startupInfoEx.StartupInfo,
      &processInfo);

  if (!success) {
    switch (::GetLastError()) {
    case ERROR_NOACCESS:
      *pErr = PlatformError::ProcessNoAccess;
      break;
    case ERROR_FILE_NOT_FOUND:
      *pErr = PlatformError::ProcessExecutableNotFound;
      break;
    case ERROR_INVALID_STARTING_CODESEG:
    case ERROR_INVALID_STACKSEG:
    case ERROR_INVALID_MODULETYPE:
    case ERROR_INVALID_EXE_SIGNATURE:
    case ERROR_EXE_MARKED_INVALID:
    case ERROR_BAD_EXE_FORMAT:
      *pErr = PlatformError::ProcessExecutableInvalid;
      break;
    default:
      *pErr = PlatformError::ProcessUnknownError;
      break;
    }
  }

  ::free(localStr); // Free our local copy of the command-line string.
  if (attributeList) {
    DeleteProcThreadAttributeList(attributeList);
    ::free(attributeList);
  }

  // Close the child side of the pipes.
  fileClose(pipeStdIn[0], pipeStdOut[1], pipeStdErr[1]);

  if (!success) {
    CloseHandle(processInfo.hThread);
    CloseHandle(processInfo.hProcess);
    return alloc->allocPlain<ProcessRef>(
        invalidProcess(), flags, pipeStdIn[1], pipeStdOut[0], pipeStdErr[0]);
  }
  return alloc->allocPlain<ProcessRef>(
      processInfo, flags, pipeStdIn[1], pipeStdOut[0], pipeStdErr[0]);

#else // !_WIN32

  bool pipeFailed = false;
  pipeFailed |= flags & ProcessPipeStdIn && ::pipe(pipeStdIn.data());
  pipeFailed |= flags & ProcessPipeStdOut && ::pipe(pipeStdOut.data());
  pipeFailed |= flags & ProcessPipeStdErr && ::pipe(pipeStdErr.data());
  if (pipeFailed) {
    // Failed to create all the pipes, close whichever child pipes where created.
    fileClose(pipeStdIn[0], pipeStdOut[1], pipeStdErr[1]);

    *pErr = PlatformError::ProcessFailedToCreatePipes;
    return alloc->allocPlain<ProcessRef>(
        invalidProcess(), flags, pipeStdIn[1], pipeStdOut[0], pipeStdErr[0]);
  }

  const auto childPid = fork();
  switch (childPid) {
  case 0: {
    // Executed on child process:

    if (flags & ProcessNewGroup) {
      // Create a new session (with a new progress group) for the child process.
      auto sid = setsid();
      if (unlikely(sid == -1)) {
        const char* setSidFailedMsg = "[err_1] Failed to create a new process group\n";
        const size_t bytesWritten   = ::write(2, setSidFailedMsg, ::strlen(setSidFailedMsg));
        (void)bytesWritten;
        ::abort();
      }
    }

    // Close the parent side of the pipes (if they are created).
    fileClose(pipeStdIn[1], pipeStdOut[0], pipeStdErr[0]);

    // Duplicate the child side of the pipes onto the stdIn, stdOut and stdErr of this process.
    bool dupFailed = false;
    dupFailed |= flags & ProcessPipeStdIn && ::dup2(pipeStdIn[0], 0) == -1;
    dupFailed |= flags & ProcessPipeStdOut && ::dup2(pipeStdOut[1], 1) == -1;
    dupFailed |= flags & ProcessPipeStdErr && ::dup2(pipeStdErr[1], 2) == -1;
    if (unlikely(dupFailed)) {
      const char* dupFailedMsg  = "[err_2] Failed to setup input and output pipes\n";
      const size_t bytesWritten = ::write(2, dupFailedMsg, ::strlen(dupFailedMsg));
      (void)bytesWritten;
      ::abort();
    }

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
          ::memmove(itr, itr + 1, ::strlen(itr));
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
      ::execvp(argV[0], argV.data());
    }

    // Only reachable when exec failed.
    const char* execFailedMsg;
    switch (errno) {
    case ENOENT:
      execFailedMsg = "[err_3] Executable not found: ";
      break;
    case EACCES:
      execFailedMsg = "[err_4] Access to executable denied: ";
      break;
    case EINVAL:
      execFailedMsg = "[err_5] Unsupported executable: ";
      break;
    case ENOMEM:
      execFailedMsg = "[err_6] Not enough memory for executable: ";
      break;
    default:
      execFailedMsg = "[err_7] Unknown error while executing: ";
      break;
    }
    size_t bytesWritten = ::write(2, execFailedMsg, ::strlen(execFailedMsg));
    if (argV[0]) {
      bytesWritten += ::write(2, argV[0], ::strlen(argV[0]));
    }
    bytesWritten += ::write(2, "\n", 1);
    (void)bytesWritten;
    ::abort();
  }
  default:
    if (childPid < 0) {
      switch (errno) {
      case EAGAIN:
        *pErr = PlatformError::ProcessLimitReached;
        break;
      default:
        *pErr = PlatformError::ProcessUnknownError;
        break;
      }
    }
    // Close the child side of the pipes (if they exist).
    fileClose(pipeStdIn[0], pipeStdOut[1], pipeStdErr[1]);
    return alloc->allocPlain<ProcessRef>(
        childPid, flags, pipeStdIn[1], pipeStdOut[0], pipeStdErr[0]);
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

inline auto processSendSignal(ProcessRef* process, PlatformError* pErr, ProcessSignalKind kind) {
  assert(process);
  if (!process->isValid()) {
    *pErr = PlatformError::ProcessInvalid;
    return false;
  }
  return process->sendSignal(pErr, kind);
}

inline auto processGetId(ProcessRef* process) -> int64_t {
  assert(process);
  return process->getId();
}

inline auto getProcessRef(const Value& val) noexcept { return val.getDowncastRef<ProcessRef>(); }

} // namespace vm::internal
