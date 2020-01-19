#pragma once
#include "internal/context.hpp"
#include "prog/expr/node_visitor_optional.hpp"
#include <optional>

namespace frontend::internal {

class CheckUnionExhaustiveness final : public prog::expr::OptionalNodeVisitor {
public:
  explicit CheckUnionExhaustiveness(const Context& context);

  [[nodiscard]] auto isExhaustive() const -> bool;

  auto visit(const prog::expr::UnionCheckExprNode& n) -> void override;
  auto visit(const prog::expr::UnionGetExprNode& n) -> void override;

private:
  const Context& m_context;
  std::optional<prog::sym::TypeId> m_unionType;
  std::vector<prog::sym::TypeId> m_checkedTypes;
};

} // namespace frontend::internal
