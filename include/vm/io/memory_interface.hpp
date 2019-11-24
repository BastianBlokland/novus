#pragma once
#include "vm/io/interface.hpp"
#include <string>
#include <vector>

namespace vm::io {

class MemoryInterface final : public Interface {
public:
  auto print(const char* data, unsigned int size) -> void override;

  [[nodiscard]] auto getOutput() const noexcept -> const std::vector<std::string>&;

private:
  std::vector<std::string> m_output;
};

} // namespace vm::io
