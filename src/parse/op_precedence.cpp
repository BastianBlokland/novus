#include "op_precedence.hpp"
#include "lex/token_type.hpp"

namespace parse {

const static int unaryPrecedence          = 7;
const static int multiplicativePrecedence = 6;
const static int additivePrecedence       = 5;
const static int relationalPrecedence     = 4;
const static int equalityPrecedence       = 3;
const static int logicAndPrecedence       = 2;
const static int logicOrPrecedence        = 1;

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
  default:
    return 0;
  }
}

} // namespace parse
