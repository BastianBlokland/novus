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

// Trick to get the number of variable args.
#define NUM_ARGS(...) std::tuple_size<decltype(std::make_tuple(__VA_ARGS__))>::value

#define INT(VAL) litExprNode(lex::litIntToken(VAL))
#define STR(VAL) litExprNode(lex::litStrToken(VAL))
#define BOOL(VAL) litExprNode(lex::litBoolToken(VAL))
#define CONST(ID) constExprNode(lex::identiferToken(ID))

#define MINUS lex::basicToken(lex::TokenType::OpMinus)
#define PLUS lex::basicToken(lex::TokenType::OpPlus)
#define STAR lex::basicToken(lex::TokenType::OpStar)
#define BANG lex::basicToken(lex::TokenType::OpBang)
#define AMP lex::basicToken(lex::TokenType::OpAmp)
#define AMPAMP lex::basicToken(lex::TokenType::OpAmpAmp)
#define PIPE lex::basicToken(lex::TokenType::OpPipe)
#define PIPEPIPE lex::basicToken(lex::TokenType::OpPipePipe)
#define GT lex::basicToken(lex::TokenType::OpGt)
#define EQEQ lex::basicToken(lex::TokenType::OpEqEq)
#define SEMI lex::basicToken(lex::TokenType::OpSemi)
#define OPAREN lex::basicToken(lex::TokenType::SepOpenParen)
#define CPAREN lex::basicToken(lex::TokenType::SepCloseParen)
#define COMMA lex::basicToken(lex::TokenType::SepComma)
#define END lex::endToken()

#define GROUP_EXPR(...) groupExprNode<std::array<NodePtr, NUM_ARGS(__VA_ARGS__)>>({__VA_ARGS__})

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
