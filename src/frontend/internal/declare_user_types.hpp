#pragma once
#include "internal/context.hpp"
#include "parse/node_visitor_optional.hpp"
#include <utility>

namespace frontend::internal {

class DeclareUserTypes final : public parse::OptionalNodeVisitor {
public:
  using StructDeclarationInfo =
      typename std::pair<prog::sym::TypeId, const parse::StructDeclStmtNode&>;
  using UnionDeclarationInfo =
      typename std::pair<prog::sym::TypeId, const parse::UnionDeclStmtNode&>;
  using EnumDeclarationInfo = typename std::pair<prog::sym::TypeId, const parse::EnumDeclStmtNode&>;

  DeclareUserTypes() = delete;
  explicit DeclareUserTypes(Context* context);

  [[nodiscard]] auto getStructs() const noexcept -> const std::vector<StructDeclarationInfo>&;
  [[nodiscard]] auto getUnions() const noexcept -> const std::vector<UnionDeclarationInfo>&;
  [[nodiscard]] auto getEnums() const noexcept -> const std::vector<EnumDeclarationInfo>&;

  auto visit(const parse::StructDeclStmtNode& n) -> void override;
  auto visit(const parse::UnionDeclStmtNode& n) -> void override;
  auto visit(const parse::EnumDeclStmtNode& n) -> void override;

private:
  Context* m_context;
  std::vector<StructDeclarationInfo> m_structs;
  std::vector<UnionDeclarationInfo> m_unions;
  std::vector<EnumDeclarationInfo> m_enums;

  auto validateTypeName(const lex::Token& nameToken) -> bool;
};

} // namespace frontend::internal
