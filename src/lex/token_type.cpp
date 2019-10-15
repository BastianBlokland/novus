#include "lex/token_type.hpp"

namespace lex {

auto operator<<(std::ostream& out, const TokenType& rhs) -> std::ostream& {
  switch (rhs) {
  case TokenType::OpPlus:
    out << "plus";
    break;
  case TokenType::OpMinus:
    out << "minus";
    break;
  case TokenType::OpStar:
    out << "star";
    break;
  case TokenType::OpSlash:
    out << "slash";
    break;
  case TokenType::OpAmp:
    out << "ampersand";
    break;
  case TokenType::OpAmpAmp:
    out << "ampersand-ampersand";
    break;
  case TokenType::OpPipe:
    out << "pipe";
    break;
  case TokenType::OpPipePipe:
    out << "pipe-pipe";
    break;
  case TokenType::OpEq:
    out << "equal";
    break;
  case TokenType::OpEqEq:
    out << "equal-equal";
    break;
  case TokenType::OpBang:
    out << "bang";
    break;
  case TokenType::OpBangEq:
    out << "bang-equal";
    break;
  case TokenType::OpLess:
    out << "less";
    break;
  case TokenType::OpLessEq:
    out << "less-equal";
    break;
  case TokenType::OpGt:
    out << "greater";
    break;
  case TokenType::OpGtEq:
    out << "greater-equal";
    break;
  case TokenType::SepOpenParen:
    out << "open-paren";
    break;
  case TokenType::SepCloseParen:
    out << "close-paren";
    break;
  case TokenType::SepComma:
    out << "comma";
    break;
  case TokenType::SepArrow:
    out << "arrow";
    break;
  case TokenType::LitInt:
    out << "int";
    break;
  case TokenType::LitBool:
    out << "bool";
    break;
  case TokenType::LitStr:
    out << "string";
    break;
  case TokenType::Keyword:
    out << "keyword";
    break;
  case TokenType::Identifier:
    out << "identifier";
    break;
  case TokenType::Error:
    out << "error";
    break;
  case TokenType::End:
    out << "end";
    break;
  }
  return out;
}

} // namespace lex
