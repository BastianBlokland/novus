#include "catch2/catch.hpp"
#include "helpers.hpp"
#include "parse/error.hpp"
#include "parse/node_expr_call.hpp"
#include "parse/node_expr_group.hpp"
#include "parse/node_expr_intrinsic.hpp"
#include "parse/type_param_list.hpp"
#include <optional>

namespace parse {

TEST_CASE("[parse] Parsing call expressions", "parse") {

  CHECK_EXPR("a()", callExprNode({}, ID_EXPR("a"), OPAREN, NO_NODES, COMMAS(0), CPAREN));
  CHECK_EXPR("1()", callExprNode({}, INT(1), OPAREN, NO_NODES, COMMAS(0), CPAREN));
  CHECK_EXPR("self()", callExprNode({}, SELF_ID_EXPR, OPAREN, NO_NODES, COMMAS(0), CPAREN));
  CHECK_EXPR(
      "1()()",
      callExprNode(
          {},
          callExprNode({}, INT(1), OPAREN, NO_NODES, COMMAS(0), CPAREN),
          OPAREN,
          NO_NODES,
          COMMAS(0),
          CPAREN));
  CHECK_EXPR(
      "(-1)()",
      callExprNode(
          {},
          parenExprNode(OPAREN, unaryExprNode(MINUS, INT(1)), CPAREN),
          OPAREN,
          NO_NODES,
          COMMAS(0),
          CPAREN));
  CHECK_EXPR(
      "-1()", unaryExprNode(MINUS, callExprNode({}, INT(1), OPAREN, NO_NODES, COMMAS(0), CPAREN)));
  CHECK_EXPR(
      "a(1,2)", callExprNode({}, ID_EXPR("a"), OPAREN, NODES(INT(1), INT(2)), COMMAS(1), CPAREN));
  CHECK_EXPR(
      "a(1,2,3+4*5)",
      callExprNode(
          {},
          ID_EXPR("a"),
          OPAREN,
          NODES(INT(1), INT(2), binaryExprNode(INT(3), PLUS, binaryExprNode(INT(4), STAR, INT(5)))),
          COMMAS(2),
          CPAREN));
  CHECK_EXPR(
      "a(1;2,3;4)",
      callExprNode(
          {},
          ID_EXPR("a"),
          OPAREN,
          NODES(
              groupExprNode(NODES(INT(1), INT(2)), SEMIS(1)),
              groupExprNode(NODES(INT(3), INT(4)), SEMIS(1))),
          COMMAS(1),
          CPAREN));
  CHECK_EXPR(
      "a(b())",
      callExprNode(
          {},
          ID_EXPR("a"),
          OPAREN,
          NODES(callExprNode({}, ID_EXPR("b"), OPAREN, NO_NODES, COMMAS(0), CPAREN)),
          COMMAS(0),
          CPAREN));
  CHECK_EXPR(
      "a{T}()",
      callExprNode(
          {},
          ID_EXPR_PARAM("a", TypeParamList(OCURLY, {TYPE("T")}, COMMAS(0), CCURLY)),
          OPAREN,
          NO_NODES,
          COMMAS(0),
          CPAREN));
  CHECK_EXPR(
      "a{T}(1,2)",
      callExprNode(
          {},
          ID_EXPR_PARAM("a", TypeParamList(OCURLY, {TYPE("T")}, COMMAS(0), CCURLY)),
          OPAREN,
          NODES(INT(1), INT(2)),
          COMMAS(1),
          CPAREN));
  CHECK_EXPR(
      "a{T, Y}()",
      callExprNode(
          {},
          ID_EXPR_PARAM("a", TypeParamList(OCURLY, {TYPE("T"), TYPE("Y")}, COMMAS(1), CCURLY)),
          OPAREN,
          NO_NODES,
          COMMAS(0),
          CPAREN));
  CHECK_EXPR(
      "a{T, U, W}()",
      callExprNode(
          {},
          ID_EXPR_PARAM(
              "a", TypeParamList(OCURLY, {TYPE("T"), TYPE("U"), TYPE("W")}, COMMAS(2), CCURLY)),
          OPAREN,
          NO_NODES,
          COMMAS(0),
          CPAREN));
  CHECK_EXPR(
      "intrinsic{a}()",
      callExprNode(
          {},
          intrinsicExprNode(INTRINSIC, OCURLY, ID("a"), CCURLY, std::nullopt),
          OPAREN,
          NO_NODES,
          COMMAS(0),
          CPAREN));

  CHECK_EXPR(
      "a{#1, #2}()",
      callExprNode(
          {},
          ID_EXPR_PARAM(
              "a",
              TypeParamList(OCURLY, {STATIC_INT_TYPE(1), STATIC_INT_TYPE(2)}, COMMAS(1), CCURLY)),
          OPAREN,
          NO_NODES,
          COMMAS(0),
          CPAREN));

  CHECK_EXPR(
      "a{A{#42}}()",
      callExprNode(
          {},
          ID_EXPR_PARAM(
              "a", TypeParamList(OCURLY, {TYPE("A", STATIC_INT_TYPE(42))}, COMMAS(0), CCURLY)),
          OPAREN,
          NO_NODES,
          COMMAS(0),
          CPAREN));

  CHECK_EXPR("fork a()", callExprNode({FORK}, ID_EXPR("a"), OPAREN, NO_NODES, COMMAS(0), CPAREN));

  CHECK_EXPR(
      "fork (a)()",
      callExprNode(
          {FORK},
          parenExprNode(OPAREN, ID_EXPR("a"), CPAREN),
          OPAREN,
          NO_NODES,
          COMMAS(0),
          CPAREN));

  SECTION("Errors") {
    CHECK_EXPR(
        "a(1 1)",
        errInvalidCallExpr(
            {}, ID_EXPR("a"), OPAREN, NODES(INT(1), INT(1)), COMMAS(0), CPAREN, true));
    CHECK_EXPR(
        "a(1 1,)",
        errInvalidCallExpr(
            {}, ID_EXPR("a"), OPAREN, NODES(INT(1), INT(1)), COMMAS(1), CPAREN, true));
    CHECK_EXPR("a(", errInvalidCallExpr({}, ID_EXPR("a"), OPAREN, NO_NODES, COMMAS(0), END, false));
    CHECK_EXPR("a{", errInvalidIdExpr(ID("a"), TypeParamList(OCURLY, {}, COMMAS(0), END)));
    CHECK_EXPR(
        "a{}()",
        callExprNode(
            {},
            errInvalidIdExpr(ID("a"), TypeParamList(OCURLY, {}, COMMAS(0), CCURLY)),
            PARENPAREN,
            NO_NODES,
            COMMAS(0),
            PARENPAREN));
    CHECK_EXPR(
        "a{T U}()",
        callExprNode(
            {},
            errInvalidIdExpr(
                ID("a"), TypeParamList(OCURLY, {TYPE("T"), TYPE("U")}, COMMAS(0), CCURLY)),
            PARENPAREN,
            NO_NODES,
            COMMAS(0),
            PARENPAREN));
    CHECK_EXPR(
        "a(,",
        errInvalidCallExpr(
            {}, ID_EXPR("a"), OPAREN, NODES(errInvalidPrimaryExpr(COMMA)), COMMAS(0), END, true));
    CHECK_EXPR(
        "a(,)",
        callExprNode(
            {}, ID_EXPR("a"), OPAREN, NODES(errInvalidPrimaryExpr(COMMA)), COMMAS(0), CPAREN));
  }

  SECTION("Spans") {
    CHECK_EXPR_SPAN("a(1,2,3+4*5)", input::Span(0, 11));
    CHECK_EXPR_SPAN("a{T,Y}(1)", input::Span(0, 8));
    CHECK_EXPR_SPAN("fork a{T,Y}(1)", input::Span(0, 13));
  }
}

} // namespace parse
