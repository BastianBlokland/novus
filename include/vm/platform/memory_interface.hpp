#pragma once
#include <string>
#include <vector>

namespace vm::platform {

class MemoryInterface final {
public:
  explicit MemoryInterface(std::string stdIn = {});

  auto print(const char* data, unsigned int size) -> void;
  auto read() -> char;

  [[nodiscard]] auto getStdOut() const noexcept -> const std::vector<std::string>&;

private:
  std::string m_stdIn;
  char* m_stdInPtr;
  std::vector<std::string> m_stdOut;
};

} // namespace vm::platform
