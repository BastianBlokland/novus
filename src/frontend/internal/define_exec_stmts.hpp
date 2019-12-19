#pragma once
#include "internal/context.hpp"
#include "parse/node_visitor_optional.hpp"

namespace frontend::internal {

class DefineExecStmts final : public parse::OptionalNodeVisitor {
public:
  DefineExecStmts() = delete;
  explicit DefineExecStmts(Context* context);

  auto visit(const parse::ExecStmtNode& n) -> void override;

private:
  Context* m_context;

  [[nodiscard]] auto getExpr(
      const parse::Node& n,
      prog::sym::ConstDeclTable* consts,
      std::vector<prog::sym::ConstId>* visibleConsts,
      prog::sym::TypeId typeHint) -> prog::expr::NodePtr;
};

} // namespace frontend::internal
