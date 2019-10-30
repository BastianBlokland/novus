#pragma once
#include <algorithm>
#include <optional>
#include <ostream>

namespace input {

class Span final {
public:
  Span() = delete;
  explicit Span(const int pos) : Span(pos, pos){};
  Span(int start, int end);

  auto operator==(const Span& rhs) const noexcept -> bool;
  auto operator!=(const Span& rhs) const noexcept -> bool;

  auto operator<(const Span& rhs) const noexcept -> bool;
  auto operator>(const Span& rhs) const noexcept -> bool;

  auto operator<(const int& rhs) const noexcept -> bool;
  auto operator>(const int& rhs) const noexcept -> bool;

  [[nodiscard]] auto getStart() const noexcept { return m_start; }
  [[nodiscard]] auto getEnd() const noexcept { return m_end; }

  template <typename SpanItr>
  [[nodiscard]] static auto combine(SpanItr begin, const SpanItr end) -> std::optional<Span> {
    static_assert(
        std::is_same<typename std::iterator_traits<SpanItr>::value_type, Span>::value,
        "Valuetype of input iterator has to be 'Span'");
    if (begin == end) {
      return std::nullopt;
    }
    auto minmax = std::minmax_element(begin, end);
    return Span{minmax.first->getStart(), minmax.second->getEnd()};
  }

  [[nodiscard]] static auto combine(Span a, Span b) -> Span;

private:
  int m_start, m_end;
};

auto operator<<(std::ostream& out, const Span& rhs) -> std::ostream&;

} // namespace input
