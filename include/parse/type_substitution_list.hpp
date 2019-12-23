#pragma once
#include "lex/token.hpp"
#include <vector>

namespace parse {

class TypeSubstitutionList final {
  friend auto operator<<(std::ostream& out, const TypeSubstitutionList& rhs) -> std::ostream&;

public:
  using iterator = typename std::vector<lex::Token>::const_iterator;

  TypeSubstitutionList() = delete;
  TypeSubstitutionList(
      lex::Token open,
      std::vector<lex::Token> subs,
      std::vector<lex::Token> commas,
      lex::Token close);

  auto operator==(const TypeSubstitutionList& rhs) const noexcept -> bool;
  auto operator!=(const TypeSubstitutionList& rhs) const noexcept -> bool;

  [[nodiscard]] auto begin() const -> iterator;
  [[nodiscard]] auto end() const -> iterator;

  [[nodiscard]] auto getSpan() const -> input::Span;
  [[nodiscard]] auto getOpen() const -> const lex::Token&;
  [[nodiscard]] auto getSubs() const -> const std::vector<lex::Token>&;
  [[nodiscard]] auto getCommas() const -> const std::vector<lex::Token>&;
  [[nodiscard]] auto getClose() const -> const lex::Token&;

  [[nodiscard]] auto validate() const -> bool;

private:
  lex::Token m_open;
  std::vector<lex::Token> m_subs;
  std::vector<lex::Token> m_commas;
  lex::Token m_close;

  auto print(std::ostream& out) const -> std::ostream&;
};

auto operator<<(std::ostream& out, const TypeSubstitutionList& rhs) -> std::ostream&;

} // namespace parse
