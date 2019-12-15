#pragma once
#include "frontend/diag.hpp"
#include "frontend/source.hpp"
#include "internal/func_template_table.hpp"
#include "parse/node_visitor_optional.hpp"
#include "prog/program.hpp"
#include <utility>

namespace frontend::internal {

class DeclareUserFuncs final : public parse::OptionalNodeVisitor {
public:
  using DeclarationInfo = typename std::pair<prog::sym::FuncId, const parse::FuncDeclStmtNode&>;

  DeclareUserFuncs() = delete;
  DeclareUserFuncs(const Source& src, prog::Program* prog, FuncTemplateTable* funcTemplates);

  [[nodiscard]] auto hasErrors() const noexcept -> bool;
  [[nodiscard]] auto getFuncs() const noexcept -> const std::vector<DeclarationInfo>&;
  [[nodiscard]] auto getDiags() const noexcept -> const std::vector<Diag>&;

  auto visit(const parse::FuncDeclStmtNode& n) -> void override;

private:
  const Source& m_src;
  prog::Program* m_prog;
  FuncTemplateTable* m_funcTemplates;
  std::vector<DeclarationInfo> m_funcs;
  std::vector<Diag> m_diags;

  auto validateFuncName(const lex::Token& nameToken) -> bool;
};

} // namespace frontend::internal
