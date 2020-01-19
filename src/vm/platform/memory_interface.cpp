#include "vm/platform/memory_interface.hpp"

namespace vm::platform {

auto MemoryInterface::getOutput() const noexcept -> const std::vector<std::string>& {
  return m_output;
}

auto MemoryInterface::print(const char* data, unsigned int size) -> void {
  m_output.emplace_back(data, size);
}

} // namespace vm::platform
