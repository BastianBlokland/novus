#pragma once
#include "frontend/diag.hpp"
#include "frontend/source.hpp"
#include "parse/node_visitor_optional.hpp"
#include "prog/program.hpp"

namespace frontend::internal {

class DefineUserFuncs final : public parse::OptionalNodeVisitor {
public:
  DefineUserFuncs() = delete;
  DefineUserFuncs(const Source& src, prog::Program* prog);

  [[nodiscard]] auto hasErrors() const noexcept -> bool;
  [[nodiscard]] auto getDiags() const noexcept -> const std::vector<Diag>&;

  auto visit(const parse::FuncDeclStmtNode& n) -> void override;

private:
  const Source& m_src;
  prog::Program* m_prog;
  std::vector<Diag> m_diags;

  auto getFuncId(const parse::FuncDeclStmtNode& n) -> prog::sym::FuncId;
  auto declareInputs(const parse::FuncDeclStmtNode& n, prog::sym::ConstDeclTable* consts) -> bool;
};

} // namespace frontend::internal