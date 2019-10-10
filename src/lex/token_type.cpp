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
  case TokenType::OpQMark:
    out << "qmark";
    break;
  case TokenType::OpColon:
    out << "colon";
    break;
  case TokenType::OpAmp:
    out << "ampersand";
    break;
  case TokenType::OpAmpAmp:
    out << "ampersandampersand";
    break;
  case TokenType::OpPipe:
    out << "pipe";
    break;
  case TokenType::OpPipePipe:
    out << "pipepipe";
    break;
  case TokenType::OpEq:
    out << "equal";
    break;
  case TokenType::OpEqEq:
    out << "equalequal";
    break;
  case TokenType::OpBang:
    out << "bang";
    break;
  case TokenType::OpBangEq:
    out << "bangequal";
    break;
  case TokenType::OpLess:
    out << "less";
    break;
  case TokenType::OpLessEq:
    out << "lessequal";
    break;
  case TokenType::OpGt:
    out << "greater";
    break;
  case TokenType::OpGtEq:
    out << "greaterequal";
    break;
  case TokenType::SepOpenParan:
    out << "openparan";
    break;
  case TokenType::SepCloseParan:
    out << "closeparan";
    break;
  case TokenType::SepComma:
    out << "comma";
    break;
  case TokenType::SepUnderscore:
    out << "underscore";
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
