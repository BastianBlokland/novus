#include "catch2/catch.hpp"
#include "helpers.hpp"
#include "parse/error.hpp"
#include "parse/node_expr_anon_func.hpp"

namespace parse {

TEST_CASE("[parse] Parsing anonymous functions", "parse") {

  CHECK_EXPR(
      "lambda () 1",
      anonFuncExprNode(
          {}, LAMBDA, ArgumentListDecl(OPAREN, ARGS(), COMMAS(0), CPAREN), std::nullopt, INT(1)));
  CHECK_EXPR(
      "lambda () -> int 1",
      anonFuncExprNode(
          {},
          LAMBDA,
          ArgumentListDecl(OPAREN, ARGS(), COMMAS(0), CPAREN),
          RetTypeSpec{ARROW, TYPE("int")},
          INT(1)));
  CHECK_EXPR(
      "impure lambda () 1",
      anonFuncExprNode(
          {IMPURE},
          LAMBDA,
          ArgumentListDecl(OPAREN, ARGS(), COMMAS(0), CPAREN),
          std::nullopt,
          INT(1)));
  CHECK_EXPR(
      "lambda (int x) 1",
      anonFuncExprNode(
          {},
          LAMBDA,
          ArgumentListDecl(
              OPAREN, ARGS(ArgumentListDecl::ArgSpec(TYPE("int"), ID("x"))), COMMAS(0), CPAREN),
          std::nullopt,
          INT(1)));
  CHECK_EXPR(
      "lambda (int x, int y) 1",
      anonFuncExprNode(
          {},
          LAMBDA,
          ArgumentListDecl(
              OPAREN,
              ARGS(
                  ArgumentListDecl::ArgSpec(TYPE("int"), ID("x")),
                  ArgumentListDecl::ArgSpec(TYPE("int"), ID("y"))),
              COMMAS(1),
              CPAREN),
          std::nullopt,
          INT(1)));
  CHECK_EXPR(
      "lambda (int x, int y, bool z) x * y",
      anonFuncExprNode(
          {},
          LAMBDA,
          ArgumentListDecl(
              OPAREN,
              ARGS(
                  ArgumentListDecl::ArgSpec(TYPE("int"), ID("x")),
                  ArgumentListDecl::ArgSpec(TYPE("int"), ID("y")),
                  ArgumentListDecl::ArgSpec(TYPE("bool"), ID("z"))),
              COMMAS(2),
              CPAREN),
          std::nullopt,
          binaryExprNode(ID_EXPR("x"), STAR, ID_EXPR("y"))));
  CHECK_EXPR(
      "lambda (int x = 1) 1",
      anonFuncExprNode(
          {},
          LAMBDA,
          ArgumentListDecl(
              OPAREN,
              ARGS(ArgumentListDecl::ArgSpec(
                  TYPE("int"), ID("x"), ArgumentListDecl::ArgInitializer(EQ, INT(1)))),
              COMMAS(0),
              CPAREN),
          std::nullopt,
          INT(1)));

  SECTION("Errors") {
    CHECK_EXPR(
        "lambda ()",
        anonFuncExprNode(
            {},
            LAMBDA,
            ArgumentListDecl(PARENPAREN, ARGS(), COMMAS(0), PARENPAREN),
            std::nullopt,
            errInvalidPrimaryExpr(END)));
    CHECK_EXPR(
        "lambda",
        errInvalidAnonFuncExpr(
            {},
            LAMBDA,
            ArgumentListDecl(END, ARGS(), COMMAS(0), END),
            std::nullopt,
            errInvalidPrimaryExpr(END)));
    CHECK_EXPR(
        "lambda (",
        errInvalidAnonFuncExpr(
            {},
            LAMBDA,
            ArgumentListDecl(OPAREN, ARGS(), COMMAS(0), END),
            std::nullopt,
            errInvalidPrimaryExpr(END)));
    CHECK_EXPR(
        "lambda (int x",
        errInvalidAnonFuncExpr(
            {},
            LAMBDA,
            ArgumentListDecl(
                OPAREN, ARGS(ArgumentListDecl::ArgSpec(TYPE("int"), ID("x"))), COMMAS(0), END),
            std::nullopt,
            errInvalidPrimaryExpr(END)));
    CHECK_EXPR(
        "lambda (int",
        errInvalidAnonFuncExpr(
            {},
            LAMBDA,
            ArgumentListDecl(
                OPAREN, ARGS(ArgumentListDecl::ArgSpec(TYPE("int"), END)), COMMAS(0), END),
            std::nullopt,
            errInvalidPrimaryExpr(END)));
    CHECK_EXPR(
        "lambda (int x int y)",
        errInvalidAnonFuncExpr(
            {},
            LAMBDA,
            ArgumentListDecl(
                OPAREN,
                ARGS(
                    ArgumentListDecl::ArgSpec(TYPE("int"), ID("x")),
                    ArgumentListDecl::ArgSpec(TYPE("int"), ID("y"))),
                COMMAS(0),
                CPAREN),
            std::nullopt,
            errInvalidPrimaryExpr(END)));
    CHECK_EXPR(
        "lambda (int x,,)",
        errInvalidAnonFuncExpr(
            {},
            LAMBDA,
            ArgumentListDecl(
                OPAREN,
                ARGS(
                    ArgumentListDecl::ArgSpec(TYPE("int"), ID("x")),
                    ArgumentListDecl::ArgSpec(Type(COMMA), CPAREN)),
                COMMAS(1),
                END),
            std::nullopt,
            errInvalidPrimaryExpr(END)));
    CHECK_EXPR(
        "impure function () 1",
        errInvalidAnonFuncExpr(
            {IMPURE},
            ID("function"),
            ArgumentListDecl(PARENPAREN, ARGS(), COMMAS(0), PARENPAREN),
            std::nullopt,
            INT(1)));
    CHECK_EXPR(
        "impure action () 1",
        errInvalidAnonFuncExpr(
            {IMPURE},
            ID("action"),
            ArgumentListDecl(PARENPAREN, ARGS(), COMMAS(0), PARENPAREN),
            std::nullopt,
            INT(1)));
    CHECK_EXPR(
        "lambda () ->",
        errInvalidAnonFuncExpr(
            {},
            LAMBDA,
            ArgumentListDecl(PARENPAREN, ARGS(), COMMAS(0), PARENPAREN),
            RetTypeSpec{ARROW, parse::Type{END}},
            errInvalidPrimaryExpr(END)));
    CHECK_EXPR(
        "lambda () -> 1",
        errInvalidAnonFuncExpr(
            {},
            LAMBDA,
            ArgumentListDecl(PARENPAREN, ARGS(), COMMAS(0), PARENPAREN),
            RetTypeSpec{ARROW, parse::Type{INT_TOK(1)}},
            errInvalidPrimaryExpr(END)));
    CHECK_EXPR(
        "lambda (int x =) -> int 1",
        errInvalidAnonFuncExpr(
            {},
            LAMBDA,
            ArgumentListDecl(
                OPAREN,
                ARGS(ArgumentListDecl::ArgSpec(
                    TYPE("int"), ID("x"), ArgumentListDecl::ArgInitializer(EQ, nullptr))),
                COMMAS(0),
                CPAREN),
            RetTypeSpec{ARROW, TYPE("int")},
            INT(1)));
  }

  SECTION("Spans") {
    CHECK_EXPR_SPAN(" lambda () 1 + 2", input::Span(1, 15));
    CHECK_EXPR_SPAN("lambda (int i) i", input::Span(0, 15));
    CHECK_EXPR_SPAN("impure lambda (int i) i", input::Span(0, 22));
  }
}

} // namespace parse
