#include "internal/get_identifier.hpp"
#include "parse/node_expr_id.hpp"

namespace frontend::internal {

auto GetIdentifier::getIdentifier() const noexcept -> const std::optional<lex::Token>& {
  return m_identifier;
}

auto GetIdentifier::visit(const parse::IdExprNode& n) -> void { m_identifier = n.getId(); }

} // namespace frontend::internal
