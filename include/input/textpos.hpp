#pragma once
#include <ostream>

namespace input {

class TextPos final {
  friend auto operator<<(std::ostream& out, const TextPos& rhs) -> std::ostream&;

public:
  TextPos() = delete;
  TextPos(unsigned int line, unsigned int col);

  auto operator==(const TextPos& rhs) const noexcept -> bool;
  auto operator!=(const TextPos& rhs) const noexcept -> bool;

  [[nodiscard]] auto getLine() const noexcept -> unsigned int;
  [[nodiscard]] auto getCol() const noexcept -> unsigned int;

private:
  unsigned int m_line, m_col;
};

auto operator<<(std::ostream& out, const TextPos& rhs) -> std::ostream&;

} // namespace input
