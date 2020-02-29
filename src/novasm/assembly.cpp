#include "novasm/assembly.hpp"
#include <cassert>
#include <utility>

namespace novasm {

Assembly::Assembly(
    uint32_t entrypoint,
    std::vector<std::string> litStrings,
    std::vector<uint8_t> instructions) noexcept :
    m_entrypoint{entrypoint},
    m_litStrings{std::move(litStrings)},
    m_instructions{std::move(instructions)} {}

auto Assembly::operator==(const Assembly& rhs) const noexcept -> bool {
  return m_entrypoint == rhs.m_entrypoint && m_litStrings == rhs.m_litStrings &&
      m_instructions == rhs.m_instructions;
}

auto Assembly::operator!=(const Assembly& rhs) const noexcept -> bool {
  return !Assembly::operator==(rhs);
}

auto Assembly::beginLitStrings() const noexcept -> LitStringIterator {
  return m_litStrings.begin();
}

auto Assembly::endLitStrings() const noexcept -> LitStringIterator { return m_litStrings.end(); }

auto Assembly::getEntrypoint() const noexcept -> uint32_t { return m_entrypoint; }

auto Assembly::getLitString(uint32_t id) const noexcept -> const std::string& {
  assert(id < m_litStrings.size());
  return m_litStrings[id];
}

auto Assembly::getInstructions() const noexcept -> const std::vector<uint8_t>& {
  return m_instructions;
}

auto Assembly::getIp(uint32_t ipOffset) const noexcept -> const uint8_t* {
  assert(ipOffset < m_instructions.size());
  return m_instructions.data() + ipOffset;
}

auto Assembly::getOffset(const uint8_t* ip) const noexcept -> uint32_t {
  return ip - m_instructions.data();
}

auto Assembly::isEnd(const uint8_t* ip) const noexcept -> bool {
  return ip == m_instructions.data() + m_instructions.size();
}

} // namespace novasm
