#include "internal/get_identifier.hpp"
#include "parse/node_expr_field.hpp"
#include "parse/node_expr_id.hpp"

namespace frontend::internal {

GetIdentifier::GetIdentifier() :
    m_instance{nullptr}, m_identifier{std::nullopt}, m_typeParams{std::nullopt} {}

auto GetIdentifier::getInstance() const noexcept -> const parse::Node* { return m_instance; }

auto GetIdentifier::getIdentifier() const noexcept -> const std::optional<lex::Token>& {
  return m_identifier;
}

auto GetIdentifier::getTypeParams() const noexcept -> const std::optional<parse::TypeParamList>& {
  return m_typeParams;
}

auto GetIdentifier::visit(const parse::IdExprNode& n) -> void {
  m_identifier = n.getId();
  m_typeParams = n.getTypeParams();
}

auto GetIdentifier::visit(const parse::FieldExprNode& n) -> void {
  m_instance   = &n[0];
  m_identifier = n.getId();
}

} // namespace frontend::internal
