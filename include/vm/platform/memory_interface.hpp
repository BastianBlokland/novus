#pragma once
#include <string>
#include <unordered_map>
#include <vector>

namespace vm::platform {

class MemoryInterface final {
public:
  MemoryInterface() noexcept;

  auto setStdIn(std::string stdIn) noexcept -> bool;
  auto setEnvArgs(std::vector<std::string> envArgs) noexcept -> void;
  auto setEnvVars(std::unordered_map<std::string, std::string> envVars) noexcept -> void;

  auto lockConWrite() noexcept -> void {}
  auto unlockConWrite() noexcept -> void {}

  auto lockConRead() noexcept -> void {}
  auto unlockConRead() noexcept -> void {}

  auto conWrite(const char* data, unsigned int size) noexcept -> void;
  auto conRead() noexcept -> char;
  auto getEnvArg(int idx) noexcept -> const char*;
  auto getEnvArgCount() noexcept -> int;
  auto getEnvVar(const char* name) noexcept -> const char*;

  [[nodiscard]] auto getStdOut() const noexcept -> const std::vector<std::string>&;

private:
  std::string m_stdIn;
  char* m_stdInPtr;
  std::vector<std::string> m_envArgs;
  std::unordered_map<std::string, std::string> m_envVars;
  std::vector<std::string> m_stdOut;
};

} // namespace vm::platform
