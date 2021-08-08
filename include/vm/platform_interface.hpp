#pragma once
#include "vm/file.hpp"
#include <string>

namespace vm {

// External interface in and out of the vm.
class PlatformInterface final {
public:
  PlatformInterface(
      std::string programPath,
      int envArgsCount,
      const char** envArgs,
      FileHandle stdIn,
      FileHandle stdOut,
      FileHandle stdErr) noexcept;

  // String containing the absolute path to the currently running program.
  // Note: Can be empty if the program only exists in memory (For example: inline exec in tests.
  auto getProgramPath() noexcept -> const std::string& { return m_programPath; }

  // Retrieve the null terminated c-style string of the environment argument at the specified index.
  auto envGetArg(int idx) noexcept -> const char*;

  // Retrieve how many environment arguments are passed to the program.
  auto envGetArgCount() noexcept -> int;

  // Filehandle that is used when a user program opens an input console-stream.
  auto getStdIn() noexcept { return m_stdIn; }

  // Filehandle that is used when a user program opens an output console-stream.
  auto getStdOut() noexcept { return m_stdOut; }

  // Filehandle that is used when a user program opens an error console-stream.
  auto getStdErr() noexcept { return m_stdErr; }

private:
  std::string m_programPath;
  int m_envArgsCount;
  const char** m_envArgs;
  FileHandle m_stdIn;
  FileHandle m_stdOut;
  FileHandle m_stdErr;
};

} // namespace vm
