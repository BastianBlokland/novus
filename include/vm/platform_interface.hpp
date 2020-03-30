#pragma once
#include <cstdio>

namespace vm {

class PlatformInterface final {
public:
  PlatformInterface(
      int envArgsCount,
      char* const* envArgs,
      std::FILE* stdIn,
      std::FILE* stdOut,
      std::FILE* stdErr) noexcept;

  auto getStdIn() noexcept { return m_stdIn; }

  auto getStdOut() noexcept { return m_stdOut; }

  auto getStdErr() noexcept { return m_stdErr != nullptr ? m_stdErr : m_stdOut; }

  auto getEnvArg(int idx) noexcept -> const char*;

  auto getEnvArgCount() noexcept -> int;

private:
  int m_envArgsCount;
  char* const* m_envArgs;
  std::FILE* m_stdIn;
  std::FILE* m_stdOut;
  std::FILE* m_stdErr;
};

} // namespace vm
