#include "op_precedence.hpp"
#include "utilities.hpp"

namespace parse {

auto getLhsOpPrecedence(const lex::Token& token) -> int {
  switch (token.getKind()) {
  case lex::TokenKind::OpPlus:
  case lex::TokenKind::OpPlusPlus:
  case lex::TokenKind::OpMinus:
  case lex::TokenKind::OpMinusMinus:
  case lex::TokenKind::OpBang:
    return unaryPrecedence;
  default:
    return 0;
  }
}

auto getRhsOpPrecedence(const lex::Token& token) -> int {
  switch (token.getKind()) {
  case lex::TokenKind::OpParenParen:
  case lex::TokenKind::SepOpenParen:
  case lex::TokenKind::SepOpenSquare:
  case lex::TokenKind::OpQMarkQMark:
  case lex::TokenKind::OpDot:
    return callPrecedence;
  case lex::TokenKind::OpStar:
  case lex::TokenKind::OpSlash:
  case lex::TokenKind::OpRem:
    return multiplicativePrecedence;
  case lex::TokenKind::OpPlus:
  case lex::TokenKind::OpMinus:
  case lex::TokenKind::OpColonColon:
    return additivePrecedence;
  case lex::TokenKind::OpShiftL:
  case lex::TokenKind::OpShiftR:
    return shiftPrecedence;
  case lex::TokenKind::OpLe:
  case lex::TokenKind::OpLeEq:
  case lex::TokenKind::OpGt:
  case lex::TokenKind::OpGtEq:
    return relationalPrecedence;
  case lex::TokenKind::OpEqEq:
  case lex::TokenKind::OpBangEq:
    return equalityPrecedence;
  case lex::TokenKind::OpAmp:
  case lex::TokenKind::OpAmpAmp:
    return andPrecedence;
  case lex::TokenKind::OpHat:
    return xorPrecedence;
  case lex::TokenKind::OpPipe:
  case lex::TokenKind::OpPipePipe:
    return orPrecedence;
  case lex::TokenKind::OpQMark:
    return conditionalPrecedence;
  case lex::TokenKind::OpSemi:
    return groupingPrecedence;
  case lex::TokenKind::Keyword:
    switch (*getKw(token)) {
    case lex::Keyword::Is:
    case lex::Keyword::As:
      return typeTestPrecedence;
    default:
      break;
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
