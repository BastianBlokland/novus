#include "vm/platform_interface.hpp"

namespace vm {

PlatformInterface::PlatformInterface(
    int envArgsCount,
    char* const* envArgs,
    std::FILE* stdIn,
    std::FILE* stdOut,
    std::FILE* stdErr) noexcept :
    m_envArgsCount{envArgsCount},
    m_envArgs{envArgs},
    m_stdIn{stdIn},
    m_stdOut{stdOut},
    m_stdErr{stdErr} {}

auto PlatformInterface::getEnvArg(int idx) noexcept -> const char* {
  return idx < 0 || idx >= m_envArgsCount ? nullptr : m_envArgs[idx];
}

auto PlatformInterface::getEnvArgCount() noexcept -> int { return m_envArgsCount; }

} // namespace vm
