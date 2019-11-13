#include "vm/program.hpp"
#include <cassert>
#include <utility>

namespace vm {

Program::Program(std::vector<uint8_t> instructions, std::vector<uint32_t> entryPoints) :
    m_instructions{std::move(instructions)}, m_entryPoints{std::move(entryPoints)} {}

auto Program::beginEntryPoints() const noexcept -> entryPointIterator {
  return m_entryPoints.begin();
}

auto Program::endEntryPoints() const noexcept -> entryPointIterator { return m_entryPoints.end(); }

auto Program::getIp(uint32_t ipOffset) const noexcept -> const uint8_t* {
  assert(ipOffset < m_instructions.size());
  return m_instructions.data() + ipOffset;
}

} // namespace vm
