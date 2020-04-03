#pragma once
#include "filesystem.hpp"
#include "input/info.hpp"
#include "lex/lexer.hpp"
#include "parse/parser.hpp"
#include <optional>
#include <string>
#include <vector>

namespace frontend {

class Source final {
  template <typename InputItrBegin, typename InputItrEnd>
  friend auto buildSource(
      std::string id, std::optional<filesystem::path> path, InputItrBegin begin, InputItrEnd end)
      -> Source;

  friend auto operator<<(std::ostream& out, const Source& rhs) -> std::ostream&;

public:
  using Path     = filesystem::path;
  using Iterator = std::vector<parse::NodePtr>::const_iterator;

  Source()                      = delete;
  Source(const Source& rhs)     = delete;
  Source(Source&& rhs) noexcept = default;
  ~Source()                     = default;

  auto operator=(const Source& rhs) -> Source& = delete;
  auto operator=(Source&& rhs) noexcept -> Source& = default;

  [[nodiscard]] auto begin() const noexcept -> Iterator;
  [[nodiscard]] auto end() const noexcept -> Iterator;

  [[nodiscard]] auto getId() const noexcept -> const std::string&;
  [[nodiscard]] auto getPath() const noexcept -> const std::optional<Path>&;
  [[nodiscard]] auto getTextPos(unsigned int pos) const noexcept -> input::TextPos;

  auto accept(parse::NodeVisitor* visitor) const -> void;

private:
  std::string m_id;
  std::optional<Path> m_path;
  std::vector<parse::NodePtr> m_nodes;
  input::Info m_info;

  Source(
      std::string id,
      std::optional<Path> path,
      std::vector<parse::NodePtr> nodes,
      input::Info info);
};

auto operator<<(std::ostream& out, const Source& rhs) -> std::ostream&;

template <typename InputItrBegin, typename InputItrEnd>
auto buildSource(
    std::string id, std::optional<filesystem::path> path, InputItrBegin begin, InputItrEnd end)
    -> Source {

  static_assert(
      std::is_same<typename std::iterator_traits<InputItrBegin>::value_type, char>::value,
      "Valuetype of input iterator has to be 'char'");

  // InfoItr tracks info while walking the input (like taking note of line ending positions).
  auto info  = input::Info{};
  auto lexer = lex::Lexer{input::InfoItr{begin, &info}, end};
  auto nodes = parse::parseAll(lexer.begin(), lexer.end());
  return Source{std::move(id), std::move(path), std::move(nodes), std::move(info)};
}

} // namespace frontend
