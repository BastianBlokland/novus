#pragma once
#include "frontend/diag.hpp"
#include "frontend/source.hpp"
#include "parse/node_visitor_optional.hpp"
#include "prog/program.hpp"
#include <utility>

namespace frontend::internal {

class DeclareUserTypes final : public parse::OptionalNodeVisitor {
public:
  using DeclarationInfo = typename std::pair<prog::sym::TypeId, const parse::StructDeclStmtNode&>;

  DeclareUserTypes() = delete;
  DeclareUserTypes(const Source& src, prog::Program* prog);

  [[nodiscard]] auto hasErrors() const noexcept -> bool;
  [[nodiscard]] auto getTypes() const noexcept -> const std::vector<DeclarationInfo>&;
  [[nodiscard]] auto getDiags() const noexcept -> const std::vector<Diag>&;

  auto visit(const parse::StructDeclStmtNode& n) -> void override;

private:
  const Source& m_src;
  prog::Program* m_prog;
  std::vector<DeclarationInfo> m_types;
  std::vector<Diag> m_diags;

  auto validateTypeName(const lex::Token& nameToken) -> bool;
};

} // namespace frontend::internal
