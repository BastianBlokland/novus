#include "lex/token_kind.hpp"

namespace lex {

auto operator<<(std::ostream& out, const TokenKind& rhs) -> std::ostream& {
  switch (rhs) {
  case TokenKind::OpPlus:
    out << "plus";
    break;
  case TokenKind::OpMinus:
    out << "minus";
    break;
  case TokenKind::OpStar:
    out << "star";
    break;
  case TokenKind::OpSlash:
    out << "slash";
    break;
  case TokenKind::OpRem:
    out << "remainder";
    break;
  case TokenKind::OpAmp:
    out << "ampersand";
    break;
  case TokenKind::OpAmpAmp:
    out << "ampersand-ampersand";
    break;
  case TokenKind::OpPipe:
    out << "pipe";
    break;
  case TokenKind::OpPipePipe:
    out << "pipe-pipe";
    break;
  case TokenKind::OpEq:
    out << "equal";
    break;
  case TokenKind::OpEqEq:
    out << "equal-equal";
    break;
  case TokenKind::OpBang:
    out << "bang";
    break;
  case TokenKind::OpBangEq:
    out << "bang-equal";
    break;
  case TokenKind::OpLe:
    out << "less";
    break;
  case TokenKind::OpLeEq:
    out << "less-equal";
    break;
  case TokenKind::OpGt:
    out << "greater";
    break;
  case TokenKind::OpGtEq:
    out << "greater-equal";
    break;
  case TokenKind::OpSemi:
    out << "semicolon";
    break;
  case TokenKind::OpQMark:
    out << "qmark";
    break;
  case TokenKind::OpDot:
    out << "dot";
    break;
  case TokenKind::SepOpenParen:
    out << "open-paren";
    break;
  case TokenKind::SepCloseParen:
    out << "close-paren";
    break;
  case TokenKind::SepComma:
    out << "comma";
    break;
  case TokenKind::SepColon:
    out << "colon";
    break;
  case TokenKind::SepArrow:
    out << "arrow";
    break;
  case TokenKind::LitInt:
    out << "int";
    break;
  case TokenKind::LitFloat:
    out << "float";
    break;
  case TokenKind::LitBool:
    out << "bool";
    break;
  case TokenKind::LitString:
    out << "string";
    break;
  case TokenKind::Keyword:
    out << "keyword";
    break;
  case TokenKind::Identifier:
    out << "identifier";
    break;
  case TokenKind::Error:
    out << "error";
    break;
  case TokenKind::End:
    out << "end";
    break;
  }
  return out;
}

} // namespace lex
