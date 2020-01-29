#include "vm/platform/memory_interface.hpp"

namespace vm::platform {

MemoryInterface::MemoryInterface() : m_stdInPtr{nullptr} {}

auto MemoryInterface::setStdIn(std::string stdIn) -> void {
  if (m_stdInPtr) {
    throw std::logic_error{"StdIn cannot be changed after vm started reading"};
  }
  m_stdIn = std::move(stdIn);
}

auto MemoryInterface::getEnvArg(int idx) -> const char* {
  return idx < 0 || idx >= static_cast<int>(m_envArgs.size()) ? nullptr : m_envArgs[idx].data();
}

auto MemoryInterface::getEnvArgCount() -> int { return static_cast<int>(m_envArgs.size()); }

auto MemoryInterface::setEnvArgs(std::vector<std::string> envArgs) -> void {
  m_envArgs = std::move(envArgs);
}

auto MemoryInterface::setEnvVars(std::unordered_map<std::string, std::string> envVars) -> void {
  m_envVars = std::move(envVars);
}

auto MemoryInterface::getStdOut() const noexcept -> const std::vector<std::string>& {
  return m_stdOut;
}

auto MemoryInterface::conWrite(const char* data, unsigned int size) -> void {
  m_stdOut.emplace_back(data, size);
}

auto MemoryInterface::conRead() -> char {
  if (m_stdInPtr == nullptr) {
    if (m_stdIn.data() == nullptr) {
      return '\0';
    }
    m_stdInPtr = m_stdIn.data();
  }
  return *m_stdInPtr == '\0' ? *m_stdInPtr : *m_stdInPtr++;
}

auto MemoryInterface::getEnvVar(const char* name) -> const char* {
  auto nameStr = std::string(name);
  auto itr     = m_envVars.find(nameStr);
  if (itr == m_envVars.end()) {
    return nullptr;
  }
  return itr->second.data();
}

} // namespace vm::platform
