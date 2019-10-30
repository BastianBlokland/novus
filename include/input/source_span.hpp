#pragma once
#include <algorithm>
#include <optional>
#include <ostream>

namespace input {

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

  template <typename SpanItr>
  [[nodiscard]] static auto combine(SpanItr begin, const SpanItr end) -> std::optional<SourceSpan> {
    static_assert(
        std::is_same<typename std::iterator_traits<SpanItr>::value_type, SourceSpan>::value,
        "Valuetype of input iterator has to be 'SourceSpan'");
    if (begin == end) {
      return std::nullopt;
    }
    auto minmax = std::minmax_element(begin, end);
    return SourceSpan{minmax.first->getStart(), minmax.second->getEnd()};
  }

  [[nodiscard]] static auto combine(SourceSpan a, SourceSpan b) -> SourceSpan;

private:
  int m_start, m_end;
};

auto operator<<(std::ostream& out, const SourceSpan& rhs) -> std::ostream&;

} // namespace input
