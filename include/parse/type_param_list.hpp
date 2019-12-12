#pragma once
#include "lex/token.hpp"
#include <vector>

namespace parse {

class TypeParamList final {
  friend auto operator<<(std::ostream& out, const TypeParamList& rhs) -> std::ostream&;

public:
  TypeParamList() = delete;
  TypeParamList(
      lex::Token open,
      std::vector<lex::Token> params,
      std::vector<lex::Token> commas,
      lex::Token close);

  auto operator==(const TypeParamList& rhs) const noexcept -> bool;
  auto operator!=(const TypeParamList& rhs) const noexcept -> bool;

  [[nodiscard]] auto getSpan() const -> input::Span;
  [[nodiscard]] auto getParams() const -> const std::vector<lex::Token>&;

  [[nodiscard]] auto validate() const -> bool;

private:
  const lex::Token m_open;
  const std::vector<lex::Token> m_params;
  const std::vector<lex::Token> m_commas;
  const lex::Token m_close;

  auto print(std::ostream& out) const -> std::ostream&;
};

auto operator<<(std::ostream& out, const TypeParamList& rhs) -> std::ostream&;

} // namespace parse
