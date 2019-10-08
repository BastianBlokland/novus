#pragma once
#include <ostream>

namespace lex {

class SourceSpan final {
public:
  SourceSpan() = delete;
  explicit SourceSpan(const int pos) : SourceSpan(pos, pos){};
  SourceSpan(int start, int end);

  auto operator==(const SourceSpan& rhs) const noexcept -> bool;
  auto operator!=(const SourceSpan& rhs) const noexcept -> bool;

  auto operator<(const SourceSpan& rhs) const noexcept -> bool;
  auto operator>(const SourceSpan& rhs) const noexcept -> bool;

  auto operator<(const int& rhs) const noexcept -> bool;
  auto operator>(const int& rhs) const noexcept -> bool;

  [[nodiscard]] auto getStart() const noexcept { return m_start; }
  [[nodiscard]] auto getEnd() const noexcept { return m_end; }

private:
  int m_start, m_end;
};

auto operator<<(std::ostream& out, const SourceSpan& rhs) -> std::ostream&;

} // namespace lex
