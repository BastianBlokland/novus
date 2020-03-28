#pragma once
#include "internal/const_binder.hpp"
#include "internal/context.hpp"
#include "internal/type_substitution_table.hpp"
#include <utility>

namespace frontend::internal {

using Signature = std::pair<prog::sym::TypeSet, prog::sym::TypeId>;

class GetExpr final : public parse::NodeVisitor {
public:
  enum class Flags : unsigned int {
    None = 0U,

    // Checked constants access means that any constants that this expression declares are only
    // visible when this expression evaluates to 'true'. For example the 'as' expression (with a
    // constant) is only valid in a 'CheckedConstsAccess' expression.
    CheckedConstsAccess = 1U << 1U,

    AllowPureFuncCalls = 1U << 2U,
    AllowActionCalls   = 1U << 3U,
  };

  GetExpr() = delete;
  GetExpr(
      Context* ctx,
      const TypeSubstitutionTable* typeSubTable,
      ConstBinder* constBinder,
      prog::sym::TypeId typeHint,
      std::optional<Signature> selfSig,
      Flags flags);

  [[nodiscard]] auto getValue() -> prog::expr::NodePtr&;

  auto visit(const parse::CommentNode& n) -> void override;
  auto visit(const parse::ErrorNode& n) -> void override;
  auto visit(const parse::AnonFuncExprNode& n) -> void override;
  auto visit(const parse::BinaryExprNode& n) -> void override;
  auto visit(const parse::CallExprNode& n) -> void override;
  auto visit(const parse::ConditionalExprNode& n) -> void override;
  auto visit(const parse::ConstDeclExprNode& n) -> void override;
  auto visit(const parse::IdExprNode& n) -> void override;
  auto visit(const parse::FieldExprNode& n) -> void override;
  auto visit(const parse::GroupExprNode& n) -> void override;
  auto visit(const parse::IndexExprNode& n) -> void override;
  auto visit(const parse::IsExprNode& n) -> void override;
  auto visit(const parse::LitExprNode& n) -> void override;
  auto visit(const parse::ParenExprNode& n) -> void override;
  auto visit(const parse::SwitchExprElseNode& n) -> void override;
  auto visit(const parse::SwitchExprIfNode& n) -> void override;
  auto visit(const parse::SwitchExprNode& n) -> void override;
  auto visit(const parse::UnaryExprNode& n) -> void override;
  auto visit(const parse::EnumDeclStmtNode& n) -> void override;
  auto visit(const parse::ExecStmtNode& n) -> void override;
  auto visit(const parse::FuncDeclStmtNode& n) -> void override;
  auto visit(const parse::ImportStmtNode& n) -> void override;
  auto visit(const parse::StructDeclStmtNode& n) -> void override;
  auto visit(const parse::UnionDeclStmtNode& n) -> void override;

private:
  enum class BinLogicOp { And, Or };

  Context* m_ctx;
  const TypeSubstitutionTable* m_typeSubTable;
  ConstBinder* m_constBinder;
  prog::sym::TypeId m_typeHint;
  std::optional<Signature> m_selfSig;
  Flags m_flags;

  prog::expr::NodePtr m_expr;

  [[nodiscard]] auto getChildExprs(const parse::Node& n, unsigned int skipAmount = 0U)
      -> std::optional<std::pair<std::vector<prog::expr::NodePtr>, prog::sym::TypeSet>>;

  [[nodiscard]] auto getChildExprs(
      const parse::Node& n, const parse::Node* additionalNode, unsigned int skipAmount = 0U)
      -> std::optional<std::pair<std::vector<prog::expr::NodePtr>, prog::sym::TypeSet>>;

  [[nodiscard]] auto
  getSubExpr(const parse::Node& n, prog::sym::TypeId typeHint, bool checkedConstsAccess = false)
      -> prog::expr::NodePtr;

  [[nodiscard]] auto getSubExpr(
      const parse::Node& n,
      std::vector<prog::sym::ConstId>* visibleConsts,
      prog::sym::TypeId typeHint,
      bool checkedConstsAccess = false) -> prog::expr::NodePtr;

  [[nodiscard]] auto
  applyImplicitConversion(prog::expr::NodePtr* expr, prog::sym::TypeId toType, input::Span span)
      -> bool;

  [[nodiscard]] auto getBinLogicOpExpr(const parse::BinaryExprNode& n, BinLogicOp op)
      -> prog::expr::NodePtr;

  [[nodiscard]] auto getStaticFieldExpr(
      const lex::Token& nameToken,
      const std::optional<parse::TypeParamList>& typeParams,
      const lex::Token& fieldToken) -> prog::expr::NodePtr;

  [[nodiscard]] auto getConstExpr(const parse::IdExprNode& n) -> prog::expr::NodePtr;

  [[nodiscard]] auto getSelfCallExpr(const parse::CallExprNode& n) -> prog::expr::NodePtr;

  [[nodiscard]] auto getDynCallExpr(const parse::CallExprNode& n) -> prog::expr::NodePtr;

  [[nodiscard]] auto declareConst(const lex::Token& nameToken, prog::sym::TypeId type)
      -> std::optional<prog::sym::ConstId>;

  [[nodiscard]] auto isExhaustive(const std::vector<prog::expr::NodePtr>& conditions) const -> bool;

  [[nodiscard]] auto getFunctionsInclConversions(
      const lex::Token& nameToken,
      const std::optional<parse::TypeParamList>& typeParams,
      const prog::sym::TypeSet& argTypes) -> std::vector<prog::sym::FuncId>;

  [[nodiscard]] auto getFunctions(
      const std::string& funcName,
      const std::optional<parse::TypeParamList>& typeParams,
      const prog::sym::TypeSet& argTypes,
      input::Span span) -> std::vector<prog::sym::FuncId>;

  template <Flags F>
  [[nodiscard]] inline auto hasFlag() const noexcept {
    return (static_cast<unsigned int>(m_flags) & static_cast<unsigned int>(F)) ==
        static_cast<unsigned int>(F);
  }

  [[nodiscard]] inline auto getOvOptions(
      int maxImplicitConvs, bool excludeNonUser = false, bool noConvOnFirstArg = false) const
      noexcept {

    auto ovFlags = prog::OvFlags::None;
    if (!hasFlag<Flags::AllowPureFuncCalls>()) {
      ovFlags = ovFlags | prog::OvFlags::ExclPureFuncs;
    }
    if (!hasFlag<Flags::AllowActionCalls>()) {
      ovFlags = ovFlags | prog::OvFlags::ExclActions;
    }
    if (excludeNonUser) {
      ovFlags = ovFlags | prog::OvFlags::ExclNonUser;
    }
    if (noConvOnFirstArg) {
      ovFlags = ovFlags | prog::OvFlags::NoConvOnFirstArg;
    }
    return prog::OvOptions{ovFlags, maxImplicitConvs};
  }
};

inline auto operator|(GetExpr::Flags lhs, GetExpr::Flags rhs) noexcept {
  return static_cast<GetExpr::Flags>(
      static_cast<unsigned int>(lhs) | static_cast<unsigned int>(rhs));
}

inline auto operator&(GetExpr::Flags lhs, GetExpr::Flags rhs) noexcept {
  return static_cast<GetExpr::Flags>(
      static_cast<unsigned int>(lhs) & static_cast<unsigned int>(rhs));
}

inline auto operator~(GetExpr::Flags rhs) noexcept {
  return static_cast<GetExpr::Flags>(~static_cast<unsigned int>(rhs));
}

} // namespace frontend::internal
