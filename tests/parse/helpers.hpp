#pragma once
#include "catch2/catch.hpp"
#include "lex/lexer.hpp"
#include "lex/token.hpp"
#include "parse/node_expr_const.hpp"
#include "parse/node_expr_lit.hpp"
#include "parse/parser.hpp"
#include <array>
#include <string>

namespace parse {

#define INT(VAL) litExprNode(lex::litIntToken(VAL))
#define STR(VAL) litExprNode(lex::litStrToken(VAL))
#define BOOL(VAL) litExprNode(lex::litBoolToken(VAL))
#define CONST(ID) constExprNode(lex::identiferToken(ID))

#define MINUS lex::basicToken(lex::TokenType::OpMinus)
#define PLUS lex::basicToken(lex::TokenType::OpPlus)
#define BANG lex::basicToken(lex::TokenType::OpBang)
#define AMP lex::basicToken(lex::TokenType::OpAmp)
#define END lex::endToken()

#define CHECK_EXPR(INPUT, ...)                                                                     \
  {                                                                                                \
    std::string input  = INPUT;                                                                    \
    auto lexer         = lex::Lexer{input.begin(), input.end()};                                   \
    auto parser        = parse::Parser{lexer.begin(), lexer.end()};                                \
    auto expectedNodes = {__VA_ARGS__};                                                            \
    for (const auto& expectedNode : expectedNodes) {                                               \
      auto expr = parser.nextExpr();                                                               \
      if (expr)                                                                                    \
        CHECK(*expr == *expectedNode);                                                             \
      else                                                                                         \
        REQUIRE(false);                                                                            \
    }                                                                                              \
    REQUIRE(parser.nextExpr() == nullptr);                                                         \
  }

} // namespace parse
