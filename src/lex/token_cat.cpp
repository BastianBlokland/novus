#include "lex/token_cat.hpp"

namespace lex {

auto operator<<(std::ostream& out, const TokenCat& rhs) -> std::ostream& {
  switch (rhs) {
  case TokenCat::Operator:
    out << "operator";
    break;
  case TokenCat::Seperator:
    out << "seperator";
    break;
  case TokenCat::Literal:
    out << "literal";
    break;
  case TokenCat::Keyword:
    out << "keyword";
    break;
  case TokenCat::Identifier:
    out << "identifier";
    break;
  case TokenCat::Comment:
    out << "comment";
    break;
  case TokenCat::Error:
    out << "error";
    break;
  case TokenCat::Unknown:
    out << "unknown";
    break;
  }
  return out;
}

auto lookupCat(const TokenKind kind) -> TokenCat {
  switch (kind) {
  case TokenKind::OpPlus:
  case TokenKind::OpPlusPlus:
  case TokenKind::OpMinus:
  case TokenKind::OpMinusMinus:
  case TokenKind::OpStar:
  case TokenKind::OpSlash:
  case TokenKind::OpRem:
  case TokenKind::OpAmp:
  case TokenKind::OpAmpAmp:
  case TokenKind::OpPipe:
  case TokenKind::OpPipePipe:
  case TokenKind::OpHat:
  case TokenKind::OpTilde:
  case TokenKind::OpShiftL:
  case TokenKind::OpShiftR:
  case TokenKind::OpEq:
  case TokenKind::OpEqEq:
  case TokenKind::OpBang:
  case TokenKind::OpBangEq:
  case TokenKind::OpLe:
  case TokenKind::OpLeEq:
  case TokenKind::OpGt:
  case TokenKind::OpGtEq:
  case TokenKind::OpSemi:
  case TokenKind::OpQMark:
  case TokenKind::OpQMarkQMark:
  case TokenKind::OpDot:
  case TokenKind::OpColonColon:
  case TokenKind::OpSquareSquare:
  case TokenKind::OpParenParen:
    return TokenCat::Operator;
  case TokenKind::SepOpenParen:
  case TokenKind::SepCloseParen:
  case TokenKind::SepOpenCurly:
  case TokenKind::SepCloseCurly:
  case TokenKind::SepOpenSquare:
  case TokenKind::SepCloseSquare:
  case TokenKind::SepComma:
  case TokenKind::SepColon:
  case TokenKind::SepArrow:
    return TokenCat::Seperator;
  case TokenKind::LitInt:
  case TokenKind::LitFloat:
  case TokenKind::LitBool:
  case TokenKind::LitString:
  case TokenKind::LitChar:
    return TokenCat::Literal;
  case TokenKind::Keyword:
    return TokenCat::Keyword;
  case TokenKind::Identifier:
  case TokenKind::Discard:
    return TokenCat::Identifier;
  case TokenKind::LineComment:
    return TokenCat::Comment;
  case TokenKind::Error:
    return TokenCat::Error;
  case TokenKind::End:
    return TokenCat::Unknown;
  }
  return TokenCat::Unknown;
}

} // namespace lex
