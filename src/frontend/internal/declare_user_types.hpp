#pragma once
#include "internal/context.hpp"
#include "parse/node_visitor_optional.hpp"
#include <vector>

namespace frontend::internal {

template <typename NodeType>
class TypeDeclInfo final {
public:
  TypeDeclInfo(prog::sym::TypeId id, Context* ctx, const NodeType& parseNode) :
      m_id{id}, m_ctx{ctx}, m_parseNode{parseNode} {}

  [[nodiscard]] auto getId() const noexcept -> prog::sym::TypeId { return m_id; }
  [[nodiscard]] auto getCtx() const noexcept -> Context* { return m_ctx; }
  [[nodiscard]] auto getParseNode() const noexcept -> const NodeType& { return m_parseNode; }

private:
  prog::sym::TypeId m_id;
  Context* m_ctx;
  const NodeType& m_parseNode;
};

using StructDeclInfo = TypeDeclInfo<parse::StructDeclStmtNode>;
using UnionDeclInfo  = TypeDeclInfo<parse::UnionDeclStmtNode>;
using EnumDeclInfo   = TypeDeclInfo<parse::EnumDeclStmtNode>;

class DeclareUserTypes final : public parse::OptionalNodeVisitor {
public:
  DeclareUserTypes() = delete;
  explicit DeclareUserTypes(
      Context* ctx,
      std::vector<StructDeclInfo>* structDecls,
      std::vector<UnionDeclInfo>* unionDecls,
      std::vector<EnumDeclInfo>* enumDecls);

  auto visit(const parse::StructDeclStmtNode& n) -> void override;
  auto visit(const parse::UnionDeclStmtNode& n) -> void override;
  auto visit(const parse::EnumDeclStmtNode& n) -> void override;

private:
  Context* m_ctx;
  std::vector<StructDeclInfo>* m_structDecls;
  std::vector<UnionDeclInfo>* m_unionDecls;
  std::vector<EnumDeclInfo>* m_enumDecls;

  auto validateTypeName(const std::string& name, input::Span span, unsigned int numTypeParams)
      -> bool;
};

} // namespace frontend::internal
