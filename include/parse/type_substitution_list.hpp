#pragma once
#include "lex/token.hpp"
#include <vector>

namespace parse {

// List of types to substitute in a templated function.
// Example: '{TIn, TResult}'.
// Note: Differs from 'TypeParamList' in that value has to be plain identifiers and not nested
// types.
class TypeSubstitutionList final {
  friend auto operator<<(std::ostream& out, const TypeSubstitutionList& rhs) -> std::ostream&;

public:
  using Iterator = typename std::vector<lex::Token>::const_iterator;

  TypeSubstitutionList() = delete;
  TypeSubstitutionList(
      lex::Token open,
      std::vector<lex::Token> subs,
      std::vector<lex::Token> commas,
      lex::Token close,
      bool missingComma = false);

  auto operator==(const TypeSubstitutionList& rhs) const noexcept -> bool;
  auto operator!=(const TypeSubstitutionList& rhs) const noexcept -> bool;

  [[nodiscard]] auto begin() const -> Iterator;
  [[nodiscard]] auto end() const -> Iterator;

  [[nodiscard]] auto getCount() const -> size_t;
  [[nodiscard]] auto getSpan() const -> input::Span;
  [[nodiscard]] auto getOpen() const -> const lex::Token&;
  [[nodiscard]] auto getSubs() const -> const std::vector<lex::Token>&;
  [[nodiscard]] auto getCommas() const -> const std::vector<lex::Token>&;
  [[nodiscard]] auto getClose() const -> const lex::Token&;

  [[nodiscard]] auto validate() const -> bool;
  [[nodiscard]] auto hasMissingComma() const -> bool;

private:
  lex::Token m_open;
  std::vector<lex::Token> m_subs;
  std::vector<lex::Token> m_commas;
  lex::Token m_close;
  bool m_missingComma;

  auto print(std::ostream& out) const -> std::ostream&;
};

auto operator<<(std::ostream& out, const TypeSubstitutionList& rhs) -> std::ostream&;

} // namespace parse
