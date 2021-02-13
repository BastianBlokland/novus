#include "vm/platform_interface.hpp"

namespace vm {

PlatformInterface::PlatformInterface(
    std::string programPath,
    int envArgsCount,
    const char** envArgs,
    FileHandle stdIn,
    FileHandle stdOut,
    FileHandle stdErr) noexcept :
    m_programPath{std::move(programPath)},
    m_envArgsCount{envArgsCount},
    m_envArgs{envArgs},
    m_stdIn{stdIn},
    m_stdOut{stdOut},
    m_stdErr{stdErr} {}

auto PlatformInterface::envGetArg(int idx) noexcept -> const char* {
  return idx < 0 || idx >= m_envArgsCount ? nullptr : m_envArgs[idx];
}

auto PlatformInterface::envGetArgCount() noexcept -> int { return m_envArgsCount; }

} // namespace vm
