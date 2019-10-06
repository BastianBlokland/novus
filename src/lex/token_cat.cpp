#include "lex/token_cat.hpp"
#include <unordered_map>

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
  static const std::unordered_map<TokenType, TokenCat> catTable = {
      {TokenType::OpPlus, TokenCat::Operator},
      {TokenType::OpMinus, TokenCat::Operator},
      {TokenType::OpStar, TokenCat::Operator},
      {TokenType::OpSlash, TokenCat::Operator},
      {TokenType::OpQMark, TokenCat::Operator},
      {TokenType::OpColon, TokenCat::Operator},
      {TokenType::OpAmp, TokenCat::Operator},
      {TokenType::OpAmpAmp, TokenCat::Operator},
      {TokenType::OpPipe, TokenCat::Operator},
      {TokenType::OpPipePipe, TokenCat::Operator},
      {TokenType::OpEq, TokenCat::Operator},
      {TokenType::OpEqEq, TokenCat::Operator},
      {TokenType::OpBang, TokenCat::Operator},
      {TokenType::OpBangEq, TokenCat::Operator},
      {TokenType::OpLess, TokenCat::Operator},
      {TokenType::OpLessEq, TokenCat::Operator},
      {TokenType::OpGt, TokenCat::Operator},
      {TokenType::OpGtEq, TokenCat::Operator},
      {TokenType::SepOpenParan, TokenCat::Seperator},
      {TokenType::SepCloseParan, TokenCat::Seperator},
      {TokenType::SepComma, TokenCat::Seperator},
      {TokenType::SepUnderscore, TokenCat::Seperator},
      {TokenType::LitInt, TokenCat::Literal},
      {TokenType::LitBool, TokenCat::Literal},
      {TokenType::LitStr, TokenCat::Literal},
      {TokenType::Keyword, TokenCat::Keyword},
      {TokenType::Identifier, TokenCat::Identifier},
      {TokenType::Error, TokenCat::Error}};

  const auto catSearch = catTable.find(type);
  if (catSearch != catTable.end()) {
    return catSearch->second;
  }

  return TokenCat::Unknown;
}

} // namespace lex
