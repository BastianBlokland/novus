#pragma once

namespace lex {

class SourceSpan final {
public:
  SourceSpan() = delete;
  SourceSpan(const int pos) : SourceSpan(pos, pos){};
  SourceSpan(const int start, const int end);

  auto operator==(const SourceSpan& rhs) const noexcept -> bool;
  auto operator!=(const SourceSpan& rhs) const noexcept -> bool;

  auto operator<(const SourceSpan& rhs) const noexcept -> bool;
  auto operator>(const SourceSpan& rhs) const noexcept -> bool;

  auto operator<(const int& rhs) const noexcept -> bool;
  auto operator>(const int& rhs) const noexcept -> bool;

  auto getStart() const noexcept { return m_start; }
  auto getEnd() const noexcept { return m_end; }

private:
  int m_start, m_end;
};

auto operator<<(std::ostream& out, const SourceSpan& rhs) -> std::ostream&;

} // namespace lex
