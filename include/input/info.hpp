#pragma once
#include "input/textpos.hpp"
#include <utility>
#include <vector>

namespace input {

class Info final {
public:
  auto markLineEnd(unsigned int pos) -> void;

  [[nodiscard]] auto getTextPos(unsigned int pos) const noexcept -> TextPos;

private:
  std::vector<unsigned int> m_lines;
};

class InfoItrTraits {
public:
  using difference_type   = char;
  using value_type        = char;
  using pointer           = char;
  using reference         = char;
  using iterator_category = std::input_iterator_tag;
};

template <typename SourceItr>
class InfoItr final : public InfoItrTraits {

  static_assert(
      std::is_same<typename std::iterator_traits<SourceItr>::value_type, char>::value,
      "Valuetype of input iterator has to be 'char'");

public:
  InfoItr() = delete;

  InfoItr(SourceItr& source, Info* tracker) : m_source{&source}, m_tracker{tracker}, m_pos{} {}

  auto operator*() -> char { return **m_source; }

  auto operator-> () -> char { return **m_source; }

  template <typename OtherItr>
  auto operator==(const OtherItr& rhs) noexcept -> bool {
    return *m_source == rhs;
  }

  template <typename OtherItr>
  auto operator!=(const OtherItr& rhs) noexcept -> bool {
    return *m_source != rhs;
  }

  auto operator++() -> void {
    ++(*m_source);
    ++m_pos;
    if (**m_source == '\n' && m_tracker) {
      m_tracker->markLineEnd(m_pos);
    }
  }

private:
  SourceItr* m_source;
  Info* m_tracker;
  unsigned int m_pos;
};

template <typename InputItrBegin, typename InputItrEnd>
auto buildInfo(InputItrBegin inputBegin, InputItrEnd inputEnd) {
  auto info = Info{};
  for (auto itr = InfoItr{inputBegin, &info}; itr != inputEnd; ++itr) {
  };
  return info;
}

} // namespace input
