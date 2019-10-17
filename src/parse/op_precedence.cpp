#include "op_precedence.hpp"
#include "lex/token_type.hpp"

namespace parse {

const static int unaryPrecedence          = 8;
const static int multiplicativePrecedence = 7;
const static int additivePrecedence       = 6;
const static int relationalPrecedence     = 5;
const static int equalityPrecedence       = 4;
const static int logicAndPrecedence       = 3;
const static int logicOrPrecedence        = 2;
const static int groupingPrecedence       = 1;

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
