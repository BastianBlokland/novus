#pragma once
#include <atomic>
#include <cstdio>
#include <cstdlib>
#include <mutex>
#include <string>
#include <vector>

namespace vm::platform {

class TerminalInterface final {
public:
  TerminalInterface(int envArgsCount, char** envArgs) noexcept :
      m_envArgsCount{envArgsCount}, m_envArgs(envArgs) {}

  auto lockConWrite() noexcept -> void {
    // Just spin to aquire the console-write lock as this lock should only be held for a very short
    // amount of time wile writing to the console.
    while (m_conWriteFlag.test_and_set(std::memory_order_acquire)) {
    }
  }

  auto unlockConWrite() noexcept -> void { m_conWriteFlag.clear(std::memory_order_release); }

  auto lockConRead() noexcept -> void { m_conReadMutex.lock(); }

  auto unlockConRead() noexcept -> void { m_conReadMutex.unlock(); }

  // NOLINTNEXTLINE: Cannot be static because it needs to match the interface.
  auto inline conWrite(const char* data, unsigned int size) noexcept -> void {
    std::fwrite(data, sizeof(char), size, stdout);
  }

  // NOLINTNEXTLINE: Cannot be static because it needs to match the interface.
  auto inline conRead() noexcept -> char {
    auto res = std::getc(stdin);
    return res > 0 ? static_cast<char>(res) : '\0';
  }

  auto inline getEnvArg(int idx) noexcept -> const char* {
    return idx < 0 || idx >= m_envArgsCount ? nullptr : m_envArgs[idx];
  }

  auto inline getEnvArgCount() noexcept -> int { return m_envArgsCount; }

  // NOLINTNEXTLINE: Cannot be static because it needs to match the interface.
  auto inline getEnvVar(const char* name) noexcept -> const char* { return std::getenv(name); }

private:
  std::atomic_flag m_conWriteFlag = ATOMIC_FLAG_INIT;
  std::mutex m_conReadMutex       = {};
  int m_envArgsCount;
  char** m_envArgs;
};

} // namespace vm::platform
