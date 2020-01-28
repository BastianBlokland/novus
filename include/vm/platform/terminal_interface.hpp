#pragma once
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>

namespace vm::platform {

class TerminalInterface final {
public:
  TerminalInterface(int envArgsCount, char** envArgs) :
      m_envArgsCount{envArgsCount}, m_envArgs(envArgs) {}

  // NOLINTNEXTLINE: Cannot be static because it needs to match the interface.
  auto inline print(const char* data, unsigned int size) -> void {
    std::fwrite(data, sizeof(char), size, stdout);
  }

  // NOLINTNEXTLINE: Cannot be static because it needs to match the interface.
  auto inline read() -> char {
    auto res = std::getc(stdin);
    return res > 0 ? static_cast<char>(res) : '\0';
  }

  auto inline getEnvArg(int idx) -> const char* {
    return idx < 0 || idx >= m_envArgsCount ? nullptr : m_envArgs[idx];
  }

  auto inline getEnvArgCount() -> int { return m_envArgsCount; }

  // NOLINTNEXTLINE: Cannot be static because it needs to match the interface.
  auto inline getEnvVar(const char* name) -> const char* { return std::getenv(name); }

private:
  int m_envArgsCount;
  char** m_envArgs;
};

} // namespace vm::platform
