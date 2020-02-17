#pragma once
#include "lex/token.hpp"
#include "parse/type.hpp"

namespace parse {

class RetTypeSpec final {
public:
  RetTypeSpec(lex::Token arrow, Type type);

  auto operator==(const RetTypeSpec& rhs) const noexcept -> bool;
  auto operator!=(const RetTypeSpec& rhs) const noexcept -> bool;

  [[nodiscard]] auto getArrow() const noexcept -> const lex::Token&;
  [[nodiscard]] auto getType() const noexcept -> const Type&;

  [[nodiscard]] auto validate() const -> bool;

private:
  lex::Token m_arrow;
  Type m_type;
};

auto operator<<(std::ostream& out, const RetTypeSpec& rhs) -> std::ostream&;

} // namespace parse
