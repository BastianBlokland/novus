#pragma once
#include "internal/context.hpp"
#include "parse/node_visitor_optional.hpp"
#include <tuple>

namespace frontend::internal {

class DeclareUserFuncs final : public parse::OptionalNodeVisitor {
public:
  using DeclarationInfo =
      typename std::tuple<prog::sym::FuncId, std::string, const parse::FuncDeclStmtNode&>;

  DeclareUserFuncs() = delete;
  explicit DeclareUserFuncs(Context* context);

  [[nodiscard]] auto getFuncs() const noexcept -> const std::vector<DeclarationInfo>&;

  auto visit(const parse::FuncDeclStmtNode& n) -> void override;

private:
  Context* m_context;
  std::vector<DeclarationInfo> m_funcs;

  auto validateFuncName(const lex::Token& nameToken) -> bool;

  auto validateFuncTemplateArgList(
      const parse::ArgumentListDecl& args, const std::vector<std::string>& typeSubParams) -> bool;

  auto validateType(const parse::Type& type, const std::vector<std::string>& typeSubParams) -> bool;
};

} // namespace frontend::internal
