#include "novasm/executable.hpp"
#include <cassert>
#include <utility>

namespace novasm {

Executable::Executable(
    std::string compilerVersion,
    uint32_t entrypoint,
    std::vector<std::string> litStrings,
    std::vector<uint8_t> instructions) noexcept :
    m_compilerVersion{std::move(compilerVersion)},
    m_entrypoint{entrypoint},
    m_litStrings{std::move(litStrings)},
    m_instructions{std::move(instructions)} {}

auto Executable::operator==(const Executable& rhs) const noexcept -> bool {
  return m_entrypoint == rhs.m_entrypoint && m_litStrings == rhs.m_litStrings &&
      m_instructions == rhs.m_instructions;
}

auto Executable::operator!=(const Executable& rhs) const noexcept -> bool {
  return !Executable::operator==(rhs);
}

auto Executable::beginLitStrings() const noexcept -> LitStringIterator {
  return m_litStrings.begin();
}

auto Executable::endLitStrings() const noexcept -> LitStringIterator { return m_litStrings.end(); }

auto Executable::getCompilerVersion() const noexcept -> const std::string& {
  return m_compilerVersion;
}

auto Executable::getEntrypoint() const noexcept -> uint32_t { return m_entrypoint; }

auto Executable::getLitString(uint32_t id) const noexcept -> const std::string& {
  assert(id < m_litStrings.size());
  return m_litStrings[id];
}

auto Executable::getInstructions() const noexcept -> const std::vector<uint8_t>& {
  return m_instructions;
}

auto Executable::getIp(uint32_t ipOffset) const noexcept -> const uint8_t* {
  assert(ipOffset < m_instructions.size());
  return m_instructions.data() + ipOffset;
}

auto Executable::getOffset(const uint8_t* ip) const noexcept -> uint32_t {
  return ip - m_instructions.data();
}

auto Executable::isEnd(const uint8_t* ip) const noexcept -> bool {
  return ip == m_instructions.data() + m_instructions.size();
}

} // namespace novasm
