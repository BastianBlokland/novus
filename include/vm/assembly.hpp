#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace vm {

class Assembly final {
public:
  using litStringIterator  = typename std::vector<std::string>::const_iterator;
  using entryPointIterator = typename std::vector<uint32_t>::const_iterator;

  Assembly(
      std::vector<std::string> litStrings,
      std::vector<uint8_t> instructions,
      std::vector<uint32_t> entryPoints) noexcept;
  Assembly(const Assembly& rhs)     = delete;
  Assembly(Assembly&& rhs) noexcept = default;
  ~Assembly() noexcept              = default;

  auto operator=(const Assembly& rhs) -> Assembly& = delete;
  auto operator=(Assembly&& rhs) -> Assembly& = delete;

  auto operator==(const Assembly& rhs) const noexcept -> bool;
  auto operator!=(const Assembly& rhs) const noexcept -> bool;

  [[nodiscard]] auto beginLitStrings() const noexcept -> litStringIterator;
  [[nodiscard]] auto endLitStrings() const noexcept -> litStringIterator;

  [[nodiscard]] auto beginEntryPoints() const noexcept -> entryPointIterator;
  [[nodiscard]] auto endEntryPoints() const noexcept -> entryPointIterator;

  [[nodiscard]] auto getLitString(uint32_t id) const noexcept -> const std::string&;
  [[nodiscard]] auto getInstructions() const noexcept -> const std::vector<uint8_t>&;
  [[nodiscard]] auto getIp(uint32_t ipOffset) const noexcept -> const uint8_t*;

  [[nodiscard]] auto readInt32(uint32_t ipOffset) const noexcept -> int32_t;
  [[nodiscard]] auto readUInt32(uint32_t ipOffset) const noexcept -> uint32_t;
  [[nodiscard]] auto readFloat(uint32_t ipOffset) const noexcept -> float;

private:
  std::vector<std::string> m_litStrings;
  std::vector<uint8_t> m_instructions;
  std::vector<uint32_t> m_entryPoints;
};

} // namespace vm
