#include "catch2/catch.hpp"
#include "helpers.hpp"
#include "parse/error.hpp"
#include "parse/node_expr_binary.hpp"
#include "parse/node_expr_is.hpp"
#include "parse/node_expr_paren.hpp"
#include "parse/node_expr_unary.hpp"

namespace parse {

TEST_CASE("Parsing is expressions", "[parse]") {

  CHECK_EXPR("x is int i", isExprNode(CONST("x"), IS, ID("int"), ID("i")));
  CHECK_EXPR("x is int _", isExprNode(CONST("x"), IS, ID("int"), DISCARD));
  CHECK_EXPR("-x is int i", isExprNode(unaryExprNode(MINUS, CONST("x")), IS, ID("int"), ID("i")));
  CHECK_EXPR(
      "(x + y) is int i",
      isExprNode(
          parenExprNode(OPAREN, binaryExprNode(CONST("x"), PLUS, CONST("y")), CPAREN),
          IS,
          ID("int"),
          ID("i")));

  SECTION("Errors") {
    CHECK_EXPR("x is", errInvalidIsExpr(CONST("x"), IS, END, END));
    CHECK_EXPR("x is int", errInvalidIsExpr(CONST("x"), IS, ID("int"), END));
    CHECK_EXPR("x is is i", errInvalidIsExpr(CONST("x"), IS, IS, ID("i")));
    CHECK_EXPR("x is int is", errInvalidIsExpr(CONST("x"), IS, ID("int"), IS));
  }

  SECTION("Spans") { CHECK_EXPR_SPAN("x is int i", input::Span(0, 9)); }
}

} // namespace parse
