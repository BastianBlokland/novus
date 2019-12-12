#include "catch2/catch.hpp"
#include "helpers.hpp"
#include "parse/error.hpp"
#include "parse/node_expr_binary.hpp"
#include "parse/node_expr_call.hpp"
#include "parse/node_expr_switch.hpp"
#include "parse/node_expr_switch_else.hpp"
#include "parse/node_expr_switch_if.hpp"

namespace parse {

TEST_CASE("Parsing switch expressions", "[parse]") {

  CHECK_EXPR(
      "if x -> 1 "
      "else -> 2",
      switchExprNode(
          NODES(switchExprIfNode(IF, CONST("x"), ARROW, INT(1))),
          switchExprElseNode(ELSE, ARROW, INT(2))));
  CHECK_EXPR(
      "if x -> 1", switchExprNode(NODES(switchExprIfNode(IF, CONST("x"), ARROW, INT(1))), nullptr));
  CHECK_EXPR(
      "if x -> 1 "
      "if y -> 2",
      switchExprNode(
          NODES(
              switchExprIfNode(IF, CONST("x"), ARROW, INT(1)),
              switchExprIfNode(IF, CONST("y"), ARROW, INT(2))),
          nullptr));
  CHECK_EXPR(
      "if y > 5 -> x + 1 "
      "else -> x * 2",
      switchExprNode(
          NODES(switchExprIfNode(
              IF,
              binaryExprNode(CONST("y"), GT, INT(5)),
              ARROW,
              binaryExprNode(CONST("x"), PLUS, INT(1)))),
          switchExprElseNode(ELSE, ARROW, binaryExprNode(CONST("x"), STAR, INT(2)))));
  CHECK_EXPR(
      "if x -> 1 "
      "if y -> 2 "
      "if z -> 3 "
      "else -> 4",
      switchExprNode(
          NODES(
              switchExprIfNode(IF, CONST("x"), ARROW, INT(1)),
              switchExprIfNode(IF, CONST("y"), ARROW, INT(2)),
              switchExprIfNode(IF, CONST("z"), ARROW, INT(3))),
          switchExprElseNode(ELSE, ARROW, INT(4))));
  CHECK_EXPR(
      "a(if x -> 1 else -> 2, 3)",
      callExprNode(
          ID("a"),
          std::nullopt,
          OPAREN,
          NODES(
              switchExprNode(
                  NODES(switchExprIfNode(IF, CONST("x"), ARROW, INT(1))),
                  switchExprElseNode(ELSE, ARROW, INT(2))),
              INT(3)),
          COMMAS(1),
          CPAREN));

  SECTION("Errors") {
    CHECK_EXPR(
        "if",
        switchExprNode(
            NODES(errInvalidSwitchIf(
                IF, errInvalidPrimaryExpr(END), END, errInvalidPrimaryExpr(END))),
            nullptr));
    CHECK_EXPR(
        "if x -> 1 else",
        switchExprNode(
            NODES(switchExprIfNode(IF, CONST("x"), ARROW, INT(1))),
            errInvalidSwitchElse(ELSE, END, errInvalidPrimaryExpr(END))));
  }

  SECTION("Spans") {
    CHECK_EXPR_SPAN("if x -> 1", input::Span(0, 8));
    CHECK_EXPR_SPAN(
        "if x -> 1 "
        "else -> 2",
        input::Span(0, 18));
    CHECK_EXPR_SPAN(
        "if x -> 1 "
        "if 1 + 2 == y -> 2 "
        "else -> 3",
        input::Span(0, 37));
  }
}

} // namespace parse
