#include "lex/tokentype.hpp"

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
  case TokenType::SepOpenParan:
    out << "openparan";
    break;
  case TokenType::SepCloseParan:
    out << "closeparan";
    break;
  case TokenType::SepComma:
    out << "comma";
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
