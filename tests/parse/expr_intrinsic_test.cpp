#include "catch2/catch.hpp"
#include "helpers.hpp"
#include "parse/error.hpp"
#include "parse/node_expr_call.hpp"
#include "parse/node_expr_intrinsic.hpp"

namespace parse {

TEST_CASE("[parse] Parsing intrinsic expressions", "parse") {

  CHECK_EXPR(
      "intrinsic{test}", intrinsicExprNode(INTRINSIC, OCURLY, ID("test"), CCURLY, std::nullopt));
  CHECK_EXPR(
      "intrinsic {test}", intrinsicExprNode(INTRINSIC, OCURLY, ID("test"), CCURLY, std::nullopt));
  CHECK_EXPR(
      "intrinsic{test}{X}",
      intrinsicExprNode(
          INTRINSIC,
          OCURLY,
          ID("test"),
          CCURLY,
          TypeParamList(OCURLY, {TYPE("X")}, COMMAS(0), CCURLY)));
  CHECK_EXPR(
      "intrinsic {test} {X, Y}",
      intrinsicExprNode(
          INTRINSIC,
          OCURLY,
          ID("test"),
          CCURLY,
          TypeParamList(OCURLY, {TYPE("T"), TYPE("Y")}, COMMAS(1), CCURLY)));
  CHECK_EXPR(
      "intrinsic{test}()",
      callExprNode(
          {},
          intrinsicExprNode(INTRINSIC, OCURLY, ID("test"), CCURLY, std::nullopt),
          OPAREN,
          NO_NODES,
          COMMAS(0),
          CPAREN));
  CHECK_EXPR(
      "intrinsic{test}(1,2)",
      callExprNode(
          {},
          intrinsicExprNode(INTRINSIC, OCURLY, ID("test"), CCURLY, std::nullopt),
          OPAREN,
          NODES(INT(1), INT(2)),
          COMMAS(1),
          CPAREN));
  CHECK_EXPR(
      "intrinsic {test} (1,2)",
      callExprNode(
          {},
          intrinsicExprNode(INTRINSIC, OCURLY, ID("test"), CCURLY, std::nullopt),
          OPAREN,
          NODES(INT(1), INT(2)),
          COMMAS(1),
          CPAREN));

  SECTION("Errors") {
    CHECK_EXPR(
        "intrinsic{}", errInvalidIntrinsicExpr(INTRINSIC, OCURLY, CCURLY, END, std::nullopt));
    CHECK_EXPR("intrinsic", errInvalidIntrinsicExpr(INTRINSIC, END, END, END, std::nullopt));
    CHECK_EXPR(
        "intrinsic{test",
        errInvalidIntrinsicExpr(INTRINSIC, OCURLY, ID("test"), END, std::nullopt));
    CHECK_EXPR(
        "intrinsic{a,", errInvalidIntrinsicExpr(INTRINSIC, OCURLY, ID("a"), COMMA, std::nullopt));
    CHECK_EXPR(
        "intrinsic(test)",
        errInvalidIntrinsicExpr(INTRINSIC, OPAREN, ID("test"), CPAREN, std::nullopt));
    CHECK_EXPR(
        "intrinsic{1}",
        errInvalidIntrinsicExpr(INTRINSIC, OCURLY, INT_TOK(1), CCURLY, std::nullopt));
    CHECK_EXPR(
        "intrinsic{a}{",
        errInvalidIntrinsicExpr(
            INTRINSIC, OCURLY, ID("a"), CCURLY, TypeParamList(OCURLY, {}, COMMAS(0), END)));
    CHECK_EXPR(
        "intrinsic{a}{}",
        errInvalidIntrinsicExpr(
            INTRINSIC, OCURLY, ID("a"), CCURLY, TypeParamList(OCURLY, {}, COMMAS(0), CCURLY)));
    CHECK_EXPR(
        "intrinsic{a}{T U}",
        errInvalidIntrinsicExpr(
            INTRINSIC,
            OCURLY,
            ID("a"),
            CCURLY,
            TypeParamList(OCURLY, {TYPE("T"), TYPE("U")}, COMMAS(0), CCURLY)));
  }

  SECTION("Spans") {
    CHECK_EXPR_SPAN("intrinsic{test}", input::Span(0, 14));
    CHECK_EXPR_SPAN("intrinsic {test}", input::Span(0, 15));
    CHECK_EXPR_SPAN("intrinsic {test} {x,y}", input::Span(0, 21));
  }
}

} // namespace parse
