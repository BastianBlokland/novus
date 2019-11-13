#pragma once
#include <vector>

namespace vm {

class Program final {
public:
  using entryPointIterator = typename std::vector<uint32_t>::const_iterator;

  Program(std::vector<uint8_t> instructions, std::vector<uint32_t> entryPoints);
  Program(const Program& rhs)     = delete;
  Program(Program&& rhs) noexcept = default;
  ~Program()                      = default;

  auto operator=(const Program& rhs) -> Program& = delete;
  auto operator=(Program&& rhs) noexcept -> Program& = delete;

  [[nodiscard]] auto beginEntryPoints() const noexcept -> entryPointIterator;
  [[nodiscard]] auto endEntryPoints() const noexcept -> entryPointIterator;

  [[nodiscard]] auto getIp(uint32_t ipOffset) const noexcept -> const uint8_t*;

private:
  std::vector<uint8_t> m_instructions;
  std::vector<uint32_t> m_entryPoints;
};

} // namespace vm
