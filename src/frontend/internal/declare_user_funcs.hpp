#pragma once
#include "frontend/diag.hpp"
#include "frontend/source.hpp"
#include "parse/node_visitor_optional.hpp"
#include "prog/program.hpp"

namespace frontend::internal {

class DeclareUserFuncs final : public parse::OptionalNodeVisitor {
public:
  DeclareUserFuncs() = delete;
  DeclareUserFuncs(const Source& src, prog::Program* prog);

  [[nodiscard]] auto hasErrors() const noexcept -> bool;
  [[nodiscard]] auto getDiags() const noexcept -> const std::vector<Diag>&;

  auto visit(const parse::FuncDeclStmtNode& n) -> void override;

private:
  const Source& m_src;
  prog::Program* m_prog;
  std::vector<Diag> m_diags;

  auto validateFuncName(const lex::Token& nameToken) -> bool;

  auto getFuncInput(const parse::FuncDeclStmtNode& n) -> std::optional<prog::sym::Input>;
};

} // namespace frontend::internal
