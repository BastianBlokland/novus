#pragma once
#include "internal/func_template_table.hpp"
#include <unordered_map>

namespace frontend::internal {

class TypeInferExpr final : public parse::NodeVisitor {
public:
  enum class Flags : unsigned int {
    None = 0U,

    // Without the aggressive flag inference will only reach an conclusion when it has all the
    // information required, with aggressive it can act on incomplete data.
    Aggressive = 1U << 1U,

    AllowActionCalls = 1U << 2U,
  };

  TypeInferExpr() = delete;
  TypeInferExpr(
      Context* ctx,
      const TypeSubstitutionTable* typeSubTable,
      std::unordered_map<std::string, prog::sym::TypeId>* constTypes,
      Flags flags);

  [[nodiscard]] auto getInferredType() const noexcept -> prog::sym::TypeId;

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
  Context* m_ctx;
  const TypeSubstitutionTable* m_typeSubTable;
  std::unordered_map<std::string, prog::sym::TypeId>* m_constTypes;
  Flags m_flags;
  prog::sym::TypeId m_type;

  auto inferSubExpr(const parse::Node& n) -> prog::sym::TypeId;
  [[nodiscard]] auto inferDynCall(const parse::CallExprNode& n) -> prog::sym::TypeId;
  [[nodiscard]] auto inferFuncCall(
      const std::string& funcName, const prog::sym::TypeSet& argTypes, bool disableConvOnFirstArg)
      -> prog::sym::TypeId;

  auto setConstType(const lex::Token& constId, prog::sym::TypeId type) -> void;
  [[nodiscard]] auto inferConstType(const lex::Token& constId) -> prog::sym::TypeId;

  template <Flags F>
  [[nodiscard]] inline auto hasFlag() const noexcept {
    return (static_cast<unsigned int>(m_flags) & static_cast<unsigned int>(F)) ==
        static_cast<unsigned int>(F);
  }
};

inline auto operator|(TypeInferExpr::Flags lhs, TypeInferExpr::Flags rhs) noexcept {
  return static_cast<TypeInferExpr::Flags>(
      static_cast<unsigned int>(lhs) | static_cast<unsigned int>(rhs));
}

inline auto operator&(TypeInferExpr::Flags lhs, TypeInferExpr::Flags rhs) noexcept {
  return static_cast<TypeInferExpr::Flags>(
      static_cast<unsigned int>(lhs) & static_cast<unsigned int>(rhs));
}

inline auto operator~(TypeInferExpr::Flags rhs) noexcept {
  return static_cast<TypeInferExpr::Flags>(~static_cast<unsigned int>(rhs));
}

} // namespace frontend::internal
