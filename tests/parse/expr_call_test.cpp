#include "catch2/catch.hpp"
#include "helpers.hpp"
#include "parse/error.hpp"
#include "parse/node_expr_call.hpp"
#include "parse/node_expr_group.hpp"
#include "parse/type_param_list.hpp"
#include <optional>

namespace parse {

TEST_CASE("Parsing call expressions", "[parse]") {

  CHECK_EXPR("a()", callExprNode(ID("a"), std::nullopt, OPAREN, NODES(), COMMAS(0), CPAREN));
  CHECK_EXPR(
      "a(1,2)",
      callExprNode(ID("a"), std::nullopt, OPAREN, NODES(INT(1), INT(2)), COMMAS(1), CPAREN));
  CHECK_EXPR(
      "a(1,2,3+4*5)",
      callExprNode(
          ID("a"),
          std::nullopt,
          OPAREN,
          NODES(INT(1), INT(2), binaryExprNode(INT(3), PLUS, binaryExprNode(INT(4), STAR, INT(5)))),
          COMMAS(2),
          CPAREN));
  CHECK_EXPR(
      "a(1;2,3;4)",
      callExprNode(
          ID("a"),
          std::nullopt,
          OPAREN,
          NODES(
              groupExprNode(NODES(INT(1), INT(2)), SEMIS(1)),
              groupExprNode(NODES(INT(3), INT(4)), SEMIS(1))),
          COMMAS(1),
          CPAREN));
  CHECK_EXPR(
      "a(b())",
      callExprNode(
          ID("a"),
          std::nullopt,
          OPAREN,
          NODES(callExprNode(ID("b"), std::nullopt, OPAREN, NODES(), COMMAS(0), CPAREN)),
          COMMAS(0),
          CPAREN));
  CHECK_EXPR(
      "a{T}()",
      callExprNode(
          ID("a"),
          TypeParamList{OCURLY, {TYPE("T")}, COMMAS(0), CCURLY},
          OPAREN,
          NODES(),
          COMMAS(0),
          CPAREN));
  CHECK_EXPR(
      "a{T}(1,2)",
      callExprNode(
          ID("a"),
          TypeParamList{OCURLY, {TYPE("T")}, COMMAS(0), CCURLY},
          OPAREN,
          NODES(INT(1), INT(2)),
          COMMAS(1),
          CPAREN));
  CHECK_EXPR(
      "a{T, Y}()",
      callExprNode(
          ID("a"),
          TypeParamList{OCURLY, {TYPE("T"), TYPE("Y")}, COMMAS(1), CCURLY},
          OPAREN,
          NODES(),
          COMMAS(0),
          CPAREN));
  CHECK_EXPR(
      "a{T, U, W}()",
      callExprNode(
          ID("a"),
          TypeParamList{OCURLY, {TYPE("T"), TYPE("U"), TYPE("W")}, COMMAS(2), CCURLY},
          OPAREN,
          NODES(),
          COMMAS(0),
          CPAREN));

  SECTION("Errors") {
    CHECK_EXPR(
        "a(1 1)",
        errInvalidCallExpr(
            ID("a"), std::nullopt, OPAREN, NODES(INT(1), INT(1)), COMMAS(0), CPAREN));
    CHECK_EXPR("a(", errInvalidCallExpr(ID("a"), std::nullopt, OPAREN, NODES(), COMMAS(0), END));
    CHECK_EXPR(
        "a{",
        errInvalidCallExpr(
            ID("a"), TypeParamList{OCURLY, {}, COMMAS(0), END}, END, NODES(), COMMAS(0), END));
    CHECK_EXPR(
        "a{}()",
        errInvalidCallExpr(
            ID("a"),
            TypeParamList{OCURLY, {}, COMMAS(0), END},
            PARENPAREN,
            NODES(),
            COMMAS(0),
            PARENPAREN));
    CHECK_EXPR(
        "a{T U}()",
        errInvalidCallExpr(
            ID("a"),
            TypeParamList{OCURLY, {TYPE("T"), TYPE("U")}, COMMAS(0), END},
            PARENPAREN,
            NODES(),
            COMMAS(0),
            PARENPAREN));
    CHECK_EXPR(
        "a(,",
        errInvalidCallExpr(
            ID("a"), std::nullopt, OPAREN, NODES(errInvalidPrimaryExpr(COMMA)), COMMAS(0), END));
    CHECK_EXPR(
        "a(,)",
        callExprNode(
            ID("a"), std::nullopt, OPAREN, NODES(errInvalidPrimaryExpr(COMMA)), COMMAS(0), CPAREN));
  }

  SECTION("Spans") {
    CHECK_EXPR_SPAN("a(1,2,3+4*5)", input::Span(0, 11));
    CHECK_EXPR_SPAN("a{T,Y}(1)", input::Span(0, 8));
  }
}

} // namespace parse
