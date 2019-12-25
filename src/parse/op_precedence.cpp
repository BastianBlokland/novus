#include "op_precedence.hpp"
#include "utilities.hpp"

namespace parse {

auto getLhsOpPrecedence(const lex::Token& token) -> int {
  switch (token.getKind()) {
  case lex::TokenKind::OpPlus:
  case lex::TokenKind::OpMinus:
  case lex::TokenKind::OpBang:
    return unaryPrecedence;
  default:
    return 0;
  }
}

auto getRhsOpPrecedence(const lex::Token& token) -> int {
  switch (token.getKind()) {
  case lex::TokenKind::OpDot:
  case lex::TokenKind::SepOpenSquare:
    return dotPrecendece;
  case lex::TokenKind::OpStar:
  case lex::TokenKind::OpSlash:
  case lex::TokenKind::OpRem:
    return multiplicativePrecedence;
  case lex::TokenKind::OpPlus:
  case lex::TokenKind::OpMinus:
  case lex::TokenKind::OpColonColon:
    return additivePrecedence;
  case lex::TokenKind::OpLe:
  case lex::TokenKind::OpLeEq:
  case lex::TokenKind::OpGt:
  case lex::TokenKind::OpGtEq:
    return relationalPrecedence;
  case lex::TokenKind::OpEqEq:
  case lex::TokenKind::OpBangEq:
    return equalityPrecedence;
  case lex::TokenKind::OpAmpAmp:
    return logicAndPrecedence;
  case lex::TokenKind::OpPipePipe:
    return logicOrPrecedence;
  case lex::TokenKind::OpQMark:
    return conditionalPrecedence;
  case lex::TokenKind::OpSemi:
    return groupingPrecedence;
  case lex::TokenKind::Keyword:
    if (getKw(token) == lex::Keyword::Is) {
      return typeTestPrecedence;
    }
    [[fallthrough]];
  default:
    return 0;
  }
}

auto isRightAssociative(const lex::Token& token) -> bool {
  switch (token.getKind()) {
  case lex::TokenKind::OpColonColon:
    return true;
  default:
    return false;
  }
}

} // namespace parse
