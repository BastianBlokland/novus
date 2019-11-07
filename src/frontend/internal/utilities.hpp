#pragma once
#include "lex/token.hpp"
#include "lex/token_payload_id.hpp"
#include "prog/operator.hpp"

namespace frontend::internal {

inline auto getName(const lex::Token& token) -> std::string {
  if (token.getKind() != lex::TokenKind::Identifier) {
    return "__unknown";
  }
  return token.getPayload<lex::IdentifierTokenPayload>()->getIdentifier();
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
  case lex::TokenKind::OpAmp:
    return prog::Operator::Amp;
  case lex::TokenKind::OpAmpAmp:
    return prog::Operator::AmpAmp;
  case lex::TokenKind::OpPipe:
    return prog::Operator::Pipe;
  case lex::TokenKind::OpPipePipe:
    return prog::Operator::PipePipe;
  case lex::TokenKind::OpEqEq:
    return prog::Operator::EqEq;
  case lex::TokenKind::OpBang:
    return prog::Operator::Bang;
  case lex::TokenKind::OpBangEq:
    return prog::Operator::BangEq;
  case lex::TokenKind::OpLess:
    return prog::Operator::Less;
  case lex::TokenKind::OpLessEq:
    return prog::Operator::LessEq;
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
  case prog::Operator::Amp:
    return "&";
  case prog::Operator::AmpAmp:
    return "&&";
  case prog::Operator::Pipe:
    return "|";
  case prog::Operator::PipePipe:
    return "||";
  case prog::Operator::EqEq:
    return "==";
  case prog::Operator::Bang:
    return "!";
  case prog::Operator::BangEq:
    return "!=";
  case prog::Operator::Less:
    return "<";
  case prog::Operator::LessEq:
    return "<=";
  case prog::Operator::Gt:
    return ">";
  case prog::Operator::GtEq:
    return ">=";
  }
  return "__unkown";
}

} // namespace frontend::internal
