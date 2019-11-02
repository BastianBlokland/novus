#pragma once
#include "input/info.hpp"
#include "lex/lexer.hpp"
#include "parse/parser.hpp"
#include <string>
#include <vector>

namespace frontend {

class Source final {
  template <typename InputItrBegin, typename InputItrEnd>
  friend auto buildSource(std::string id, InputItrBegin begin, InputItrEnd end) -> Source;

  friend auto operator<<(std::ostream& out, const Source& rhs) -> std::ostream&;

public:
  using iterator = typename std::vector<parse::NodePtr>::const_iterator;

  Source()                      = delete;
  Source(const Source& rhs)     = delete;
  Source(Source&& rhs) noexcept = default;
  ~Source()                     = default;

  auto operator=(const Source& rhs) -> Source& = delete;
  auto operator=(Source&& rhs) noexcept -> Source& = delete;

  [[nodiscard]] auto begin() const noexcept -> iterator;
  [[nodiscard]] auto end() const noexcept -> iterator;

  [[nodiscard]] auto getId() const noexcept -> std::string;
  [[nodiscard]] auto getTextPos(unsigned int pos) const noexcept -> input::TextPos;

private:
  const std::string m_id;
  const std::vector<parse::NodePtr> m_nodes;
  const input::Info m_info;

  Source(std::string id, std::vector<parse::NodePtr> nodes, input::Info info);
};

auto operator<<(std::ostream& out, const Source& rhs) -> std::ostream&;

template <typename InputItrBegin, typename InputItrEnd>
auto buildSource(std::string id, InputItrBegin begin, InputItrEnd end) -> Source {
  static_assert(
      std::is_same<typename std::iterator_traits<InputItrBegin>::value_type, char>::value,
      "Valuetype of input iterator has to be 'char'");

  // InfoItr tracks info while walking the input (like taking note of line ending positions).
  auto info  = input::Info{};
  auto lexer = lex::Lexer{input::InfoItr{begin, &info}, end};
  auto nodes = parse::parseAll(lexer.begin(), lexer.end());
  return Source{std::move(id), std::move(nodes), std::move(info)};
}

} // namespace frontend
