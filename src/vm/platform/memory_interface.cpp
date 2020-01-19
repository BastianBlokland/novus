#include "vm/platform/memory_interface.hpp"

namespace vm::platform {

auto MemoryInterface::getStdOut() const noexcept -> const std::vector<std::string>& {
  return m_stdOut;
}

auto MemoryInterface::print(const char* data, unsigned int size) -> void {
  m_stdOut.emplace_back(data, size);
}

} // namespace vm::platform
