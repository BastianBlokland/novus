#pragma once
#include <string>
#include <unordered_map>
#include <vector>

namespace vm::platform {

class MemoryInterface final {
public:
  MemoryInterface();

  auto setStdIn(std::string stdIn) -> void;
  auto setEnvArgs(std::vector<std::string> envArgs) -> void;
  auto setEnvVars(std::unordered_map<std::string, std::string> envVars) -> void;

  auto print(const char* data, unsigned int size) -> void;
  auto read() -> char;
  auto getEnvArg(int idx) -> const char*;
  auto getEnvArgCount() -> int;
  auto getEnvVar(const char* name) -> const char*;

  [[nodiscard]] auto getStdOut() const noexcept -> const std::vector<std::string>&;

private:
  std::string m_stdIn;
  char* m_stdInPtr;
  std::vector<std::string> m_envArgs;
  std::unordered_map<std::string, std::string> m_envVars;
  std::vector<std::string> m_stdOut;
};

} // namespace vm::platform
