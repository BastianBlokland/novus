#include "catch2/catch.hpp"
#include "helpers.hpp"
#include "parse/error.hpp"
#include "parse/node_expr_anon_func.hpp"

namespace parse {

TEST_CASE("Parsing anonymous functions", "[parse]") {

  CHECK_EXPR(
      "fun () 1", anonFuncExprNode(FUN, ArgumentListDecl(OPAREN, {}, COMMAS(0), CPAREN), INT(1)));
  CHECK_EXPR(
      "fun (int x) 1",
      anonFuncExprNode(
          FUN,
          ArgumentListDecl(
              OPAREN, {ArgumentListDecl::ArgSpec(TYPE("int"), ID("x"))}, COMMAS(0), CPAREN),
          INT(1)));
  CHECK_EXPR(
      "fun (int x, int y) 1",
      anonFuncExprNode(
          FUN,
          ArgumentListDecl(
              OPAREN,
              {ArgumentListDecl::ArgSpec(TYPE("int"), ID("x")),
               ArgumentListDecl::ArgSpec(TYPE("int"), ID("y"))},
              COMMAS(1),
              CPAREN),
          INT(1)));
  CHECK_EXPR(
      "fun (int x, int y, bool z) x * y",
      anonFuncExprNode(
          FUN,
          ArgumentListDecl(
              OPAREN,
              {ArgumentListDecl::ArgSpec(TYPE("int"), ID("x")),
               ArgumentListDecl::ArgSpec(TYPE("int"), ID("y")),
               ArgumentListDecl::ArgSpec(TYPE("bool"), ID("z"))},
              COMMAS(2),
              CPAREN),
          binaryExprNode(ID_EXPR("x"), STAR, ID_EXPR("y"))));

  SECTION("Errors") {
    CHECK_EXPR(
        "fun ()",
        anonFuncExprNode(
            FUN, ArgumentListDecl(OPAREN, {}, COMMAS(0), CPAREN), errInvalidPrimaryExpr(END)));
    CHECK_EXPR(
        "fun",
        errInvalidAnonFuncExpr(
            FUN, ArgumentListDecl(END, {}, COMMAS(0), END), errInvalidPrimaryExpr(END)));
    CHECK_EXPR(
        "fun (",
        errInvalidAnonFuncExpr(
            FUN, ArgumentListDecl(OPAREN, {}, COMMAS(0), END), errInvalidPrimaryExpr(END)));
    CHECK_EXPR(
        "fun (int x",
        errInvalidAnonFuncExpr(
            FUN,
            ArgumentListDecl(
                OPAREN, {ArgumentListDecl::ArgSpec(TYPE("int"), ID("x"))}, COMMAS(0), END),
            errInvalidPrimaryExpr(END)));
    CHECK_EXPR(
        "fun (int",
        errInvalidAnonFuncExpr(
            FUN,
            ArgumentListDecl(OPAREN, {ArgumentListDecl::ArgSpec(TYPE("int"), END)}, COMMAS(0), END),
            errInvalidPrimaryExpr(END)));
    CHECK_EXPR(
        "fun (int x int y)",
        errInvalidAnonFuncExpr(
            FUN,
            ArgumentListDecl(
                OPAREN,
                {ArgumentListDecl::ArgSpec(TYPE("int"), ID("x")),
                 ArgumentListDecl::ArgSpec(TYPE("int"), ID("y"))},
                COMMAS(0),
                CPAREN),
            errInvalidPrimaryExpr(END)));
    CHECK_EXPR(
        "fun (int x,,)",
        errInvalidAnonFuncExpr(
            FUN,
            ArgumentListDecl(
                OPAREN,
                {ArgumentListDecl::ArgSpec(TYPE("int"), ID("x")),
                 ArgumentListDecl::ArgSpec(Type(COMMA), CPAREN)},
                COMMAS(1),
                END),
            errInvalidPrimaryExpr(END)));
  }

  SECTION("Spans") {
    CHECK_EXPR_SPAN(" fun a() 1 + 2", input::Span(1, 13));
    CHECK_EXPR_SPAN("fun a() 1", input::Span(0, 8));
  }
}

} // namespace parse
