#include "vm/assembly.hpp"
#include <cassert>
#include <utility>

namespace vm {

Assembly::Assembly(std::vector<uint8_t> instructions, std::vector<uint32_t> entryPoints) :
    m_instructions{std::move(instructions)}, m_entryPoints{std::move(entryPoints)} {}

auto Assembly::beginEntryPoints() const noexcept -> entryPointIterator {
  return m_entryPoints.begin();
}

auto Assembly::endEntryPoints() const noexcept -> entryPointIterator { return m_entryPoints.end(); }

auto Assembly::getInstructions() const noexcept -> const std::vector<uint8_t>& {
  return m_instructions;
}

auto Assembly::getIp(uint32_t ipOffset) const noexcept -> const uint8_t* {
  assert(ipOffset < m_instructions.size());
  return m_instructions.data() + ipOffset;
}

auto Assembly::readInt32(uint32_t ipOffset) const noexcept -> int32_t {
  int32_t result = 0;
  for (auto i = ipOffset + 4; i >= ipOffset; --i) {
    result = (result << 8) + m_instructions[i]; // NOLINT: Signed bitwise operator
  }
  return result;
}

auto Assembly::readUInt32(uint32_t ipOffset) const noexcept -> uint32_t {
  uint32_t result = 0;
  for (auto i = ipOffset + 4; i >= ipOffset; --i) {
    result = (result << 8) + m_instructions[i]; // NOLINT: Signed bitwise operator
  }
  return result;
}

} // namespace vm
