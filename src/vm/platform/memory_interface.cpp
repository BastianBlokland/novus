#include "vm/platform/memory_interface.hpp"

namespace vm::platform {

MemoryInterface::MemoryInterface() noexcept : m_stdInPtr{nullptr} {}

auto MemoryInterface::setStdIn(std::string stdIn) noexcept -> bool {
  if (m_stdInPtr) {
    return false;
  }
  m_stdIn = std::move(stdIn);
  return true;
}

auto MemoryInterface::setEnvArgs(std::vector<std::string> envArgs) noexcept -> void {
  m_envArgs = std::move(envArgs);
}

auto MemoryInterface::setEnvVars(std::unordered_map<std::string, std::string> envVars) noexcept
    -> void {
  m_envVars = std::move(envVars);
}

auto MemoryInterface::getEnvArg(int idx) noexcept -> const char* {
  return idx < 0 || idx >= static_cast<int>(m_envArgs.size()) ? nullptr : m_envArgs[idx].data();
}

auto MemoryInterface::getEnvArgCount() noexcept -> int {
  return static_cast<int>(m_envArgs.size());
}

auto MemoryInterface::getStdOut() const noexcept -> const std::vector<std::string>& {
  return m_stdOut;
}

auto MemoryInterface::conWrite(const char* data, unsigned int size) noexcept -> void {
  m_stdOut.emplace_back(data, size);
}

auto MemoryInterface::conRead() noexcept -> char {
  if (m_stdInPtr == nullptr) {
    if (m_stdIn.data() == nullptr) {
      return '\0';
    }
    m_stdInPtr = m_stdIn.data();
  }
  return *m_stdInPtr == '\0' ? *m_stdInPtr : *m_stdInPtr++;
}

auto MemoryInterface::getEnvVar(const char* name) noexcept -> const char* {
  auto nameStr = std::string(name);
  auto itr     = m_envVars.find(nameStr);
  if (itr == m_envVars.end()) {
    return nullptr;
  }
  return itr->second.data();
}

} // namespace vm::platform
