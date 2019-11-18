#pragma once
#include "frontend/diag.hpp"
#include "frontend/source.hpp"
#include "parse/node_visitor_optional.hpp"
#include "prog/program.hpp"
#include <utility>

namespace frontend::internal {

class DeclareUserFuncs final : public parse::OptionalNodeVisitor {
public:
  using Declaration = typename std::pair<prog::sym::FuncId, const parse::FuncDeclStmtNode&>;

  DeclareUserFuncs() = delete;
  DeclareUserFuncs(const Source& src, prog::Program* prog);

  [[nodiscard]] auto hasErrors() const noexcept -> bool;
  [[nodiscard]] auto getFuncs() const noexcept -> const std::vector<Declaration>&;
  [[nodiscard]] auto getDiags() const noexcept -> const std::vector<Diag>&;

  auto visit(const parse::FuncDeclStmtNode& n) -> void override;

private:
  const Source& m_src;
  prog::Program* m_prog;
  std::vector<Declaration> m_funcs;
  std::vector<Diag> m_diags;

  auto getRetType(const parse::FuncDeclStmtNode& n) -> std::optional<prog::sym::TypeId>;
  auto validateFuncName(const lex::Token& nameToken) -> bool;
  auto getFuncInput(const parse::FuncDeclStmtNode& n) -> std::optional<prog::sym::Input>;
};

} // namespace frontend::internal
