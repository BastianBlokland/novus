#pragma once
#include "frontend/diag.hpp"
#include "frontend/source.hpp"
#include "lex/token.hpp"
#include "lex/token_payload_id.hpp"
#include "parse/node_stmt_func_decl.hpp"
#include "parse/type_param_list.hpp"
#include "prog/operator.hpp"
#include "prog/program.hpp"
#include "type_substitution_table.hpp"

namespace frontend::internal {

inline auto getName(const lex::Token& token) -> std::string {
  if (token.getKind() != lex::TokenKind::Identifier) {
    return "__unknown";
  }
  return token.getPayload<lex::IdentifierTokenPayload>()->getIdentifier();
}

inline auto getName(const prog::Program& prog, prog::sym::TypeId typeId) -> std::string {
  if (!typeId.isConcrete()) {
    return "unknown";
  }
  return prog.getTypeDecl(typeId).getName();
}

inline auto getOperator(const lex::Token& token) -> std::optional<prog::Operator> {
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
  default:
    return std::nullopt;
  }
}

inline auto getText(const prog::Operator& op) -> std::string {
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
  }
  return "__unknown";
}

auto getType(
    const prog::Program& prog, const TypeSubstitutionTable* subTable, const std::string& name)
    -> std::optional<prog::sym::TypeId>;

auto getRetType(
    const Source& src,
    const prog::Program& prog,
    const TypeSubstitutionTable* subTable,
    const parse::FuncDeclStmtNode& n,
    std::vector<Diag>* diags) -> std::optional<prog::sym::TypeId>;

auto getFuncInput(
    const Source& src,
    const prog::Program& prog,
    const TypeSubstitutionTable* subTable,
    const parse::FuncDeclStmtNode& n,
    std::vector<Diag>* diags) -> std::optional<prog::sym::TypeSet>;

auto getTypeParams(
    const Source& src,
    const prog::Program& prog,
    const parse::TypeParamList& paramList,
    std::vector<Diag>* diags) -> std::optional<std::vector<std::string>>;

auto getTypeSet(
    const Source& src,
    const prog::Program& prog,
    const TypeSubstitutionTable* subTable,
    const std::vector<lex::Token>& typeTokens,
    std::vector<Diag>* diags) -> std::optional<prog::sym::TypeSet>;

auto getConstName(
    const Source& src,
    const prog::Program& prog,
    const TypeSubstitutionTable* subTable,
    const prog::sym::ConstDeclTable& consts,
    const lex::Token& nameToken,
    std::vector<Diag>* diags) -> std::optional<std::string>;

} // namespace frontend::internal
