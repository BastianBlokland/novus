#pragma once
#include "internal/context.hpp"
#include "lex/token_payload_id.hpp"
#include "prog/operator.hpp"

namespace frontend::internal {

[[nodiscard]] inline auto getName(const lex::Token& token) -> std::string {
  if (token.getKind() != lex::TokenKind::Identifier) {
    return "__unknown";
  }
  return token.getPayload<lex::IdentifierTokenPayload>()->getIdentifier();
}

[[nodiscard]] inline auto getName(const parse::Type& parseType) -> std::string {
  return getName(parseType.getId());
}

[[nodiscard]] inline auto getName(Context* context, prog::sym::TypeId typeId) -> std::string {
  if (!typeId.isConcrete()) {
    return "unknown";
  }
  return context->getProg()->getTypeDecl(typeId).getName();
}

[[nodiscard]] inline auto getOperator(const lex::Token& token) -> std::optional<prog::Operator> {
  switch (token.getKind()) {
  case lex::TokenKind::OpPlus:
    return prog::Operator::Plus;
  case lex::TokenKind::OpMinus:
    return prog::Operator::Minus;
  case lex::TokenKind::OpStar:
    return prog::Operator::Star;
  case lex::TokenKind::OpSlash:
    return prog::Operator::Slash;
  case lex::TokenKind::OpRem:
    return prog::Operator::Rem;
  case lex::TokenKind::OpAmp:
    return prog::Operator::Amp;
  case lex::TokenKind::OpPipe:
    return prog::Operator::Pipe;
  case lex::TokenKind::OpEqEq:
    return prog::Operator::EqEq;
  case lex::TokenKind::OpBang:
    return prog::Operator::Bang;
  case lex::TokenKind::OpBangEq:
    return prog::Operator::BangEq;
  case lex::TokenKind::OpLe:
    return prog::Operator::Le;
  case lex::TokenKind::OpLeEq:
    return prog::Operator::LeEq;
  case lex::TokenKind::OpGt:
    return prog::Operator::Gt;
  case lex::TokenKind::OpGtEq:
    return prog::Operator::GtEq;
  case lex::TokenKind::OpColonColon:
    return prog::Operator::ColonColon;
  case lex::TokenKind::OpSquareSquare:
    return prog::Operator::SquareSquare;
  case lex::TokenKind::OpParenParen:
    return prog::Operator::ParenParen;
  default:
    return std::nullopt;
  }
}

[[nodiscard]] inline auto getText(const prog::Operator& op) -> std::string {
  switch (op) {
  case prog::Operator::Plus:
    return "+";
  case prog::Operator::Minus:
    return "-";
  case prog::Operator::Star:
    return "*";
  case prog::Operator::Slash:
    return "/";
  case prog::Operator::Rem:
    return "%";
  case prog::Operator::Amp:
    return "&";
  case prog::Operator::Pipe:
    return "|";
  case prog::Operator::EqEq:
    return "==";
  case prog::Operator::Bang:
    return "!";
  case prog::Operator::BangEq:
    return "!=";
  case prog::Operator::Le:
    return "<";
  case prog::Operator::LeEq:
    return "<=";
  case prog::Operator::Gt:
    return ">";
  case prog::Operator::GtEq:
    return ">=";
  case prog::Operator::ColonColon:
    return "::";
  case prog::Operator::SquareSquare:
    return "[]";
  case prog::Operator::ParenParen:
    return "()";
  }
  return "__unknown";
}

[[nodiscard]] auto getOrInstType(
    Context* context,
    const TypeSubstitutionTable* subTable,
    const lex::Token& nameToken,
    const std::optional<parse::TypeParamList>& typeParams,
    const prog::sym::TypeSet& constructorArgs) -> std::optional<prog::sym::TypeId>;

[[nodiscard]] auto
getOrInstType(Context* context, const TypeSubstitutionTable* subTable, const parse::Type& parseType)
    -> std::optional<prog::sym::TypeId>;

[[nodiscard]] auto instType(
    Context* context,
    const TypeSubstitutionTable* subTable,
    const lex::Token& nameToken,
    const parse::TypeParamList& typeParams) -> std::optional<prog::sym::TypeId>;

[[nodiscard]] auto getRetType(
    Context* context, const TypeSubstitutionTable* subTable, const parse::FuncDeclStmtNode& n)
    -> std::optional<prog::sym::TypeId>;

[[nodiscard]] auto inferRetType(
    Context* context,
    const TypeSubstitutionTable* subTable,
    const parse::FuncDeclStmtNode& funcDeclParseNode,
    const prog::sym::TypeSet& input,
    bool aggressive) -> prog::sym::TypeId;

[[nodiscard]] auto getFuncInput(
    Context* context, const TypeSubstitutionTable* subTable, const parse::FuncDeclStmtNode& n)
    -> std::optional<prog::sym::TypeSet>;

[[nodiscard]] auto
getSubstitutionParams(Context* context, const parse::TypeSubstitutionList& subList)
    -> std::optional<std::vector<std::string>>;

[[nodiscard]] auto getTypeSet(
    Context* context,
    const TypeSubstitutionTable* subTable,
    const std::vector<parse::Type>& parseTypes) -> std::optional<prog::sym::TypeSet>;

[[nodiscard]] auto getConstName(
    Context* context,
    const TypeSubstitutionTable* subTable,
    const prog::sym::ConstDeclTable& consts,
    const lex::Token& nameToken) -> std::optional<std::string>;

[[nodiscard]] auto
mangleName(Context* context, const std::string& name, const prog::sym::TypeSet& typeParams)
    -> std::string;

[[nodiscard]] auto isType(Context* context, const std::string& name) -> bool;

} // namespace frontend::internal
