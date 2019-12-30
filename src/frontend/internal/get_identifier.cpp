#include "internal/get_identifier.hpp"
#include "parse/node_expr_id.hpp"

namespace frontend::internal {

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

} // namespace frontend::internal
