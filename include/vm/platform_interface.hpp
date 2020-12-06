#pragma once
#include <cstdio>

namespace vm {

// External interface in and out of the vm.
class PlatformInterface final {
public:
  PlatformInterface(
      const char* programPath,
      int envArgsCount,
      char* const* envArgs,
      std::FILE* stdIn,
      std::FILE* stdOut,
      std::FILE* stdErr) noexcept;

  // Null-terminated c-style string containing the absolute path to the currently running program.
  // Note: Can be null if the program only exists in memory (For example: inline exec by nove).
  auto getProgramPath() noexcept { return m_programPath; }

  // Retreive the null terminated c-style string of the environment argument at the specified index.
  auto envGetArg(int idx) noexcept -> const char*;

  // Retrieve how many environment arguments are passed to the program.
  auto envGetArgCount() noexcept -> int;

  // Filehandle that is used when a user program opens an input console-stream.
  auto getStdIn() noexcept { return m_stdIn; }

  // Filehandle that is used when a user program opens an output console-stream.
  auto getStdOut() noexcept { return m_stdOut; }

  // Filehandle that is used when a user program opens an error console-stream.
  // Note: if stdErr is null then stdOut is used instead.
  auto getStdErr() noexcept { return m_stdErr != nullptr ? m_stdErr : m_stdOut; }

private:
  const char* m_programPath;
  int m_envArgsCount;
  char* const* m_envArgs;
  std::FILE* m_stdIn;
  std::FILE* m_stdOut;
  std::FILE* m_stdErr;
};

} // namespace vm
