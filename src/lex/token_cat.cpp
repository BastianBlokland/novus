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
  case TokenCat::Error:
    out << "error";
    break;
  case TokenCat::Unknown:
    out << "unknown";
    break;
  }
  return out;
}

auto lookupCat(const TokenType type) -> TokenCat {
  switch (type) {
  case TokenType::OpPlus:
  case TokenType::OpMinus:
  case TokenType::OpStar:
  case TokenType::OpSlash:
  case TokenType::OpQMark:
  case TokenType::OpColon:
  case TokenType::OpAmp:
  case TokenType::OpAmpAmp:
  case TokenType::OpPipe:
  case TokenType::OpPipePipe:
  case TokenType::OpEq:
  case TokenType::OpEqEq:
  case TokenType::OpBang:
  case TokenType::OpBangEq:
  case TokenType::OpLess:
  case TokenType::OpLessEq:
  case TokenType::OpGt:
  case TokenType::OpGtEq:
    return TokenCat::Operator;
  case TokenType::SepOpenParan:
  case TokenType::SepCloseParan:
  case TokenType::SepComma:
  case TokenType::SepUnderscore:
  case TokenType::SepArrow:
    return TokenCat::Seperator;
  case TokenType::LitInt:
  case TokenType::LitBool:
  case TokenType::LitStr:
    return TokenCat::Literal;
  case TokenType::Keyword:
    return TokenCat::Keyword;
  case TokenType::Identifier:
    return TokenCat::Identifier;
  case TokenType::Error:
    return TokenCat::Error;
  case TokenType::End:
    return TokenCat::Unknown;
  }
  return TokenCat::Unknown;
}

} // namespace lex
