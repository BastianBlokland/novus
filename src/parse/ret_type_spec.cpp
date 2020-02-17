#include "parse/ret_type_spec.hpp"

namespace parse {

RetTypeSpec::RetTypeSpec(lex::Token arrow, Type type) :
    m_arrow{std::move(arrow)}, m_type{std::move(type)} {}

auto RetTypeSpec::operator==(const RetTypeSpec& rhs) const noexcept -> bool {
  return m_arrow == rhs.m_arrow && m_type == rhs.m_type;
}

auto RetTypeSpec::operator!=(const RetTypeSpec& rhs) const noexcept -> bool {
  return !RetTypeSpec::operator==(rhs);
}

auto RetTypeSpec::getArrow() const noexcept -> const lex::Token& { return m_arrow; }

auto RetTypeSpec::getType() const noexcept -> const Type& { return m_type; }

auto RetTypeSpec::validate() const -> bool {
  return m_arrow.getKind() == lex::TokenKind::SepArrow && m_type.validate();
}

auto operator<<(std::ostream& out, const RetTypeSpec& rhs) -> std::ostream& {
  return out << "->" << rhs.getType();
}

} // namespace parse
