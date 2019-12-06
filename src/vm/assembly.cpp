#include "vm/assembly.hpp"
#include <cassert>
#include <utility>

namespace vm {

Assembly::Assembly(
    std::vector<std::string> litStrings,
    std::vector<uint8_t> instructions,
    std::vector<uint32_t> entryPoints) :
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

auto Assembly::readInt32(uint32_t ipOffset) const noexcept -> int32_t {
  auto raw = readUInt32(ipOffset);
  return reinterpret_cast<int32_t&>(raw); // NOLINT: Reinterpret cast
}

auto Assembly::readUInt32(uint32_t ipOffset) const noexcept -> uint32_t {
  uint32_t result = 0;
  for (auto i = ipOffset + 4; i >= ipOffset; --i) {
    result = (result << 8) + m_instructions[i]; // NOLINT: Signed bitwise operator
  }
  return result;
}

auto Assembly::readFloat(uint32_t ipOffset) const noexcept -> float {
  auto raw = readUInt32(ipOffset);
  return reinterpret_cast<float&>(raw); // NOLINT: Reinterpret cast
}

} // namespace vm
