#include "vm/platform/memory_interface.hpp"

namespace vm::platform {

MemoryInterface::MemoryInterface(std::string stdIn) :
    m_stdIn{std::move(stdIn)}, m_stdInPtr{m_stdIn.data()}, m_stdOut{} {}

auto MemoryInterface::getStdOut() const noexcept -> const std::vector<std::string>& {
  return m_stdOut;
}

auto MemoryInterface::print(const char* data, unsigned int size) -> void {
  m_stdOut.emplace_back(data, size);
}

auto MemoryInterface::read() -> char { return *m_stdInPtr == '\0' ? *m_stdInPtr : *m_stdInPtr++; }

} // namespace vm::platform
