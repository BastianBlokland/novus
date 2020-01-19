#pragma once
#include <string>
#include <vector>

namespace vm::platform {

class MemoryInterface final {
public:
  auto print(const char* data, unsigned int size) -> void;

  [[nodiscard]] auto getStdOut() const noexcept -> const std::vector<std::string>&;

private:
  std::vector<std::string> m_stdOut;
};

} // namespace vm::platform
