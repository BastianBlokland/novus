#pragma once
#include "input/textpos.hpp"
#include <stdexcept>
#include <utility>
#include <vector>

namespace input {

class Info final {
  template <typename SourceItr>
  friend class InfoItr;

public:
  Info();
  Info(const Info& rhs)     = delete;
  Info(Info&& rhs) noexcept = default;
  ~Info()                   = default;

  auto operator=(const Info& rhs) -> Info& = delete;
  auto operator=(Info&& rhs) noexcept -> Info& = default;

  [[nodiscard]] auto getLineCount() const noexcept -> unsigned int;
  [[nodiscard]] auto getCharCount() const noexcept -> unsigned int;
  [[nodiscard]] auto getTextPos(unsigned int pos) const noexcept -> TextPos;

private:
  std::vector<unsigned int> m_lines;
  unsigned int m_end;

  auto markLineEnd(unsigned int pos) -> void;
  auto markFileEnd(unsigned int pos) -> void;
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

  InfoItr(SourceItr& source, Info* tracker) : m_source{&source}, m_tracker{tracker}, m_pos{} {
    if (!m_tracker) {
      throw std::invalid_argument{"Given tracker cannot be null"};
    }
  }

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
    switch (**m_source) {
    case '\n':
      m_tracker->markLineEnd(m_pos);
    case '\0':
      m_tracker->markFileEnd(m_pos);
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
  }
  return info;
}

} // namespace input
