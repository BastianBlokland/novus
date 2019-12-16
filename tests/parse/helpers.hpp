#pragma once
#include "catch2/catch.hpp"
#include "lex/lexer.hpp"
#include "lex/token.hpp"
#include "parse/node_expr_binary.hpp"
#include "parse/node_expr_const.hpp"
#include "parse/node_expr_const_decl.hpp"
#include "parse/node_expr_group.hpp"
#include "parse/node_expr_lit.hpp"
#include "parse/node_expr_paren.hpp"
#include "parse/node_expr_unary.hpp"
#include "parse/parser.hpp"
#include <array>
#include <string>

namespace parse {

#define NUM_ARGS(...) std::tuple_size<decltype(std::make_tuple(__VA_ARGS__))>::value

#define MINUS lex::basicToken(lex::TokenKind::OpMinus)
#define PLUS lex::basicToken(lex::TokenKind::OpPlus)
#define STAR lex::basicToken(lex::TokenKind::OpStar)
#define BANG lex::basicToken(lex::TokenKind::OpBang)
#define AMP lex::basicToken(lex::TokenKind::OpAmp)
#define AMPAMP lex::basicToken(lex::TokenKind::OpAmpAmp)
#define PIPE lex::basicToken(lex::TokenKind::OpPipe)
#define PIPEPIPE lex::basicToken(lex::TokenKind::OpPipePipe)
#define GT lex::basicToken(lex::TokenKind::OpGt)
#define EQ lex::basicToken(lex::TokenKind::OpEq)
#define EQEQ lex::basicToken(lex::TokenKind::OpEqEq)
#define SEMI lex::basicToken(lex::TokenKind::OpSemi)
#define QMARK lex::basicToken(lex::TokenKind::OpQMark)
#define DOT lex::basicToken(lex::TokenKind::OpDot)
#define SEMIS(COUNT) std::vector<lex::Token>(COUNT, lex::basicToken(lex::TokenKind::OpSemi))
#define OPAREN lex::basicToken(lex::TokenKind::SepOpenParen)
#define CPAREN lex::basicToken(lex::TokenKind::SepCloseParen)
#define OCURLY lex::basicToken(lex::TokenKind::SepOpenCurly)
#define CCURLY lex::basicToken(lex::TokenKind::SepCloseCurly)
#define COMMA lex::basicToken(lex::TokenKind::SepComma)
#define COLON lex::basicToken(lex::TokenKind::SepColon)
#define COMMAS(COUNT) std::vector<lex::Token>(COUNT, lex::basicToken(lex::TokenKind::SepComma))
#define ARROW lex::basicToken(lex::TokenKind::SepArrow)
#define DISCARD lex::basicToken(lex::TokenKind::Discard)
#define FUN lex::keywordToken(lex::Keyword::Fun)
#define STRUCT lex::keywordToken(lex::Keyword::Struct)
#define UNION lex::keywordToken(lex::Keyword::Union)
#define IF lex::keywordToken(lex::Keyword::If)
#define ELSE lex::keywordToken(lex::Keyword::Else)
#define IS lex::keywordToken(lex::Keyword::Is)
#define END lex::endToken()

#define ID(ID) lex::identiferToken(ID)
#define TYPE(NAME, ...)                                                                            \
  NUM_ARGS(__VA_ARGS__) == 0                                                                       \
      ? Type(ID(NAME))                                                                             \
      : Type(                                                                                      \
            ID(NAME),                                                                              \
            TypeParamList(                                                                         \
                OCURLY,                                                                            \
                {__VA_ARGS__},                                                                     \
                COMMAS(NUM_ARGS(__VA_ARGS__) == 0 ? 0 : (NUM_ARGS(__VA_ARGS__) - 1)),              \
                CCURLY))

#define INT(VAL) litExprNode(lex::litIntToken(VAL))
#define STR(VAL) litExprNode(lex::litStrToken(VAL))
#define BOOL(VAL) litExprNode(lex::litBoolToken(VAL))
#define CONST(ID) constExprNode(lex::identiferToken(ID))
#define CONSTDECL(ID, EXPR) constDeclExprNode(lex::identiferToken(ID), EQ, EXPR)

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

#define CHECK_STMT(INPUT, ...)                                                                     \
  {                                                                                                \
    std::string input  = INPUT;                                                                    \
    auto lexer         = lex::Lexer{input.begin(), input.end()};                                   \
    auto parser        = parse::Parser{lexer.begin(), lexer.end()};                                \
    auto expectedNodes = {__VA_ARGS__};                                                            \
    for (const auto& expectedNode : expectedNodes) {                                               \
      auto stmt = parser.nextStmt();                                                               \
      if (stmt)                                                                                    \
        CHECK(*stmt == *expectedNode);                                                             \
      else                                                                                         \
        REQUIRE(false);                                                                            \
    }                                                                                              \
    REQUIRE(parser.nextStmt() == nullptr);                                                         \
  }

#define CHECK_EXPR_SPAN(INPUT, SPAN)                                                               \
  {                                                                                                \
    std::string input = INPUT;                                                                     \
    auto lexer        = lex::Lexer{input.begin(), input.end()};                                    \
    auto parser       = parse::Parser{lexer.begin(), lexer.end()};                                 \
    CHECK(parser.nextExpr()->getSpan() == (SPAN));                                                 \
  }

#define CHECK_STMT_SPAN(INPUT, SPAN)                                                               \
  {                                                                                                \
    std::string input = INPUT;                                                                     \
    auto lexer        = lex::Lexer{input.begin(), input.end()};                                    \
    auto parser       = parse::Parser{lexer.begin(), lexer.end()};                                 \
    CHECK(parser.nextStmt()->getSpan() == (SPAN));                                                 \
  }

template <typename Array>
inline auto arrayMoveToVec(Array c) {
  auto result = std::vector<typename Array::value_type>{};
  for (auto& elem : c) {
    result.push_back(std::move(elem));
  }
  return result;
}

#define NODES(...) arrayMoveToVec<std::array<NodePtr, NUM_ARGS(__VA_ARGS__)>>({__VA_ARGS__})

} // namespace parse
