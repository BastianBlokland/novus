#include "op_precedence.hpp"
#include "lex/token_type.hpp"

namespace parse {

auto getUnaryOpPrecedence(const lex::Token& token) -> int {
  switch (token.getType()) {
  case lex::TokenType::OpPlus:
  case lex::TokenType::OpMinus:
  case lex::TokenType::OpBang:
    return unaryPrecedence;
  default:
    return 0;
  }
}

auto getBinaryOpPrecedence(const lex::Token& token) -> int {
  switch (token.getType()) {
  case lex::TokenType::OpStar:
  case lex::TokenType::OpSlash:
    return multiplicativePrecedence;
  case lex::TokenType::OpPlus:
  case lex::TokenType::OpMinus:
    return additivePrecedence;
  case lex::TokenType::OpLess:
  case lex::TokenType::OpLessEq:
  case lex::TokenType::OpGt:
  case lex::TokenType::OpGtEq:
    return relationalPrecedence;
  case lex::TokenType::OpEqEq:
  case lex::TokenType::OpBangEq:
    return equalityPrecedence;
  case lex::TokenType::OpAmpAmp:
    return logicAndPrecedence;
  case lex::TokenType::OpPipePipe:
    return logicOrPrecedence;
  case lex::TokenType::OpSemi:
    return groupingPrecedence;
  default:
    return 0;
  }
}

} // namespace parse
