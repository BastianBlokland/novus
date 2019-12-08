#pragma once
#include "frontend/diag.hpp"
#include "frontend/source.hpp"
#include "prog/program.hpp"

namespace frontend::internal {

class DefineUserFuncs final {
public:
  DefineUserFuncs() = delete;
  DefineUserFuncs(const Source& src, prog::Program* prog);

  [[nodiscard]] auto hasErrors() const noexcept -> bool;
  [[nodiscard]] auto getDiags() const noexcept -> const std::vector<Diag>&;

  auto define(prog::sym::FuncId id, const parse::FuncDeclStmtNode& n) -> void;

private:
  const Source& m_src;
  prog::Program* m_prog;
  std::vector<Diag> m_diags;

  auto declareInputs(const parse::FuncDeclStmtNode& n, prog::sym::ConstDeclTable* consts) -> bool;
  auto getExpr(
      const parse::Node& n,
      prog::sym::ConstDeclTable* consts,
      std::vector<prog::sym::ConstId>* visibleConsts,
      prog::sym::TypeId typeHint) -> prog::expr::NodePtr;
};

} // namespace frontend::internal
