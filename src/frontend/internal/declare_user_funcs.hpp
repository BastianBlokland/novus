#pragma once
#include "internal/context.hpp"
#include "parse/node_visitor_optional.hpp"
#include <utility>

namespace frontend::internal {

class DeclareUserFuncs final : public parse::OptionalNodeVisitor {
public:
  using DeclarationInfo = typename std::pair<prog::sym::FuncId, const parse::FuncDeclStmtNode&>;

  DeclareUserFuncs() = delete;
  explicit DeclareUserFuncs(Context* context);

  [[nodiscard]] auto getFuncs() const noexcept -> const std::vector<DeclarationInfo>&;

  auto visit(const parse::FuncDeclStmtNode& n) -> void override;

private:
  Context* m_context;
  std::vector<DeclarationInfo> m_funcs;

  auto validateFuncName(const lex::Token& nameToken) -> bool;
};

} // namespace frontend::internal
