#include "vm/assembly.hpp"
#include <cassert>
#include <utility>

namespace vm {

Assembly::Assembly(
    std::vector<std::string> litStrings,
    std::vector<uint8_t> instructions,
    std::vector<uint32_t> entryPoints) noexcept :
    m_litStrings{std::move(litStrings)},
    m_instructions{std::move(instructions)},
    m_entryPoints{std::move(entryPoints)} {}

auto Assembly::operator==(const Assembly& rhs) const noexcept -> bool {
  return m_litStrings == rhs.m_litStrings && m_instructions == rhs.m_instructions &&
      m_entryPoints == rhs.m_entryPoints;
}

auto Assembly::operator!=(const Assembly& rhs) const noexcept -> bool {
  return !Assembly::operator==(rhs);
}

auto Assembly::beginLitStrings() const noexcept -> litStringIterator {
  return m_litStrings.begin();
}

auto Assembly::endLitStrings() const noexcept -> litStringIterator { return m_litStrings.end(); }

auto Assembly::beginEntryPoints() const noexcept -> entryPointIterator {
  return m_entryPoints.begin();
}

auto Assembly::endEntryPoints() const noexcept -> entryPointIterator { return m_entryPoints.end(); }

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

} // namespace vm
