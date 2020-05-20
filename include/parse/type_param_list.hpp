#pragma once
#include "lex/token.hpp"
#include "parse/type.hpp"
#include <vector>

namespace parse {

// List of type parameters.
// Example in source: '{int, float}' or '{Person{bool}, Person{Child{int}}}'.
class TypeParamList final {
  friend auto operator<<(std::ostream& out, const TypeParamList& rhs) -> std::ostream&;

public:
  using Iterator = typename std::vector<Type>::const_iterator;

  TypeParamList() = delete;
  TypeParamList(
      lex::Token open, std::vector<Type> params, std::vector<lex::Token> commas, lex::Token close);

  auto operator==(const TypeParamList& rhs) const noexcept -> bool;
  auto operator!=(const TypeParamList& rhs) const noexcept -> bool;

  [[nodiscard]] auto operator[](unsigned int i) const -> const Type&;

  [[nodiscard]] auto getCount() const -> unsigned int;

  [[nodiscard]] auto begin() const -> Iterator;
  [[nodiscard]] auto end() const -> Iterator;

  [[nodiscard]] auto getSpan() const -> input::Span;
  [[nodiscard]] auto getOpen() const -> const lex::Token&;
  [[nodiscard]] auto getTypes() const -> const std::vector<Type>&;
  [[nodiscard]] auto getCommas() const -> const std::vector<lex::Token>&;
  [[nodiscard]] auto getClose() const -> const lex::Token&;

  [[nodiscard]] auto validate() const -> bool;

private:
  lex::Token m_open;
  std::vector<Type> m_params;
  std::vector<lex::Token> m_commas;
  lex::Token m_close;

  auto print(std::ostream& out) const -> std::ostream&;
};

auto operator<<(std::ostream& out, const TypeParamList& rhs) -> std::ostream&;

} // namespace parse
