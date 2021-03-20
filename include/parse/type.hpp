#pragma once
#include "gsl.hpp"
#include "lex/token.hpp"

namespace parse {

class TypeParamList;

// Parsed type including type-paramters.
// Example in source: 'Person{int, bool}' or 'Person'.
class Type final {
  friend auto operator<<(std::ostream& out, const Type& rhs) -> std::ostream&;

public:
  Type() = delete;
  explicit Type(lex::Token id);
  Type(lex::Token id, TypeParamList paramList);
  Type(const Type& other) noexcept;
  Type(Type&& other) noexcept;
  ~Type();

  auto operator=(const Type& other) noexcept -> Type&;
  auto operator=(Type&& other) noexcept -> Type&;

  auto operator==(const Type& rhs) const noexcept -> bool;
  auto operator!=(const Type& rhs) const noexcept -> bool;

  [[nodiscard]] auto getSpan() const -> input::Span;
  [[nodiscard]] auto getId() const -> const lex::Token&;
  [[nodiscard]] auto isStaticInt() const -> bool;
  [[nodiscard]] auto getParamList() const -> const TypeParamList*;
  [[nodiscard]] auto getParamCount() const -> unsigned int;

  [[nodiscard]] auto validate() const -> bool;

private:
  lex::Token m_id;
  gsl::owner<TypeParamList*> m_paramList;

  auto print(std::ostream& out) const -> std::ostream&;
};

auto operator<<(std::ostream& out, const Type& rhs) -> std::ostream&;

} // namespace parse
