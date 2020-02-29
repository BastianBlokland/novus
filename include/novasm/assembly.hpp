#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace novasm {

class Assembly final {
public:
  using LitStringIterator = typename std::vector<std::string>::const_iterator;

  Assembly(
      uint32_t entrypoint,
      std::vector<std::string> litStrings,
      std::vector<uint8_t> instructions) noexcept;
  Assembly(const Assembly& rhs)     = delete;
  Assembly(Assembly&& rhs) noexcept = default;
  ~Assembly() noexcept              = default;

  auto operator=(const Assembly& rhs) -> Assembly& = delete;
  auto operator=(Assembly&& rhs) -> Assembly& = delete;

  auto operator==(const Assembly& rhs) const noexcept -> bool;
  auto operator!=(const Assembly& rhs) const noexcept -> bool;

  [[nodiscard]] auto beginLitStrings() const noexcept -> LitStringIterator;
  [[nodiscard]] auto endLitStrings() const noexcept -> LitStringIterator;

  [[nodiscard]] auto getEntrypoint() const noexcept -> uint32_t;
  [[nodiscard]] auto getLitString(uint32_t id) const noexcept -> const std::string&;
  [[nodiscard]] auto getInstructions() const noexcept -> const std::vector<uint8_t>&;
  [[nodiscard]] auto getIp(uint32_t ipOffset) const noexcept -> const uint8_t*;
  [[nodiscard]] auto getOffset(const uint8_t* ip) const noexcept -> uint32_t;
  [[nodiscard]] auto isEnd(const uint8_t* ip) const noexcept -> bool;

private:
  uint32_t m_entrypoint;
  std::vector<std::string> m_litStrings;
  std::vector<uint8_t> m_instructions;
};

} // namespace novasm
