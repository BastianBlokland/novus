#pragma once
#include "frontend/diag.hpp"
#include "frontend/source.hpp"
#include "parse/node_visitor_optional.hpp"
#include "prog/program.hpp"

namespace frontend::internal {

class DefineExecStmts final : public parse::OptionalNodeVisitor {
public:
  DefineExecStmts() = delete;
  DefineExecStmts(const Source& src, prog::Program* prog);

  [[nodiscard]] auto hasErrors() const noexcept -> bool;
  [[nodiscard]] auto getDiags() const noexcept -> const std::vector<Diag>&;

  auto visit(const parse::ExecStmtNode& n) -> void override;

private:
  const Source& m_src;
  prog::Program* m_prog;
  std::vector<Diag> m_diags;

  [[nodiscard]] auto getExpr(
      const parse::Node& n,
      prog::sym::ConstDeclTable* consts,
      std::vector<prog::sym::ConstId>* visibleConsts,
      prog::sym::TypeId typeHint) -> prog::expr::NodePtr;
};

} // namespace frontend::internal
