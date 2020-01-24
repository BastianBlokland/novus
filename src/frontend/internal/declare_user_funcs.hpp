#pragma once
#include "internal/context.hpp"
#include "parse/node_visitor_optional.hpp"

namespace frontend::internal {

class FuncDeclInfo final {
public:
  FuncDeclInfo(
      prog::sym::FuncId id,
      Context* ctx,
      std::string displayName,
      const parse::FuncDeclStmtNode& parseNode) :
      m_id{id}, m_ctx{ctx}, m_displayName{std::move(displayName)}, m_parseNode{parseNode} {}

  [[nodiscard]] auto getId() const noexcept -> prog::sym::FuncId { return m_id; }
  [[nodiscard]] auto getCtx() const noexcept -> Context* { return m_ctx; }
  [[nodiscard]] auto getDisplayName() const noexcept -> std::string { return m_displayName; }
  [[nodiscard]] auto getParseNode() const noexcept -> const parse::FuncDeclStmtNode& {
    return m_parseNode;
  }

private:
  prog::sym::FuncId m_id;
  Context* m_ctx;
  std::string m_displayName;
  const parse::FuncDeclStmtNode& m_parseNode;
};

class DeclareUserFuncs final : public parse::OptionalNodeVisitor {
public:
  DeclareUserFuncs() = delete;
  DeclareUserFuncs(Context* ctx, std::vector<FuncDeclInfo>* funcDecls);

  auto visit(const parse::FuncDeclStmtNode& n) -> void override;

private:
  Context* m_ctx;
  std::vector<FuncDeclInfo>* m_funcDecls;

  auto validateFuncTemplateArgList(
      const parse::ArgumentListDecl& args, const std::vector<std::string>& typeSubParams) -> bool;

  auto validateType(const parse::Type& type, const std::vector<std::string>& typeSubParams) -> bool;
};

} // namespace frontend::internal
