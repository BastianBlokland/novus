#include "vm/io/memory_interface.hpp"

namespace vm::io {

auto MemoryInterface::getOutput() const noexcept -> const std::vector<std::string>& {
  return m_output;
}

auto MemoryInterface::print(std::string text) -> void { m_output.push_back(std::move(text)); }

} // namespace vm::io
