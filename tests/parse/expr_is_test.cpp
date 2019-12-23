#include "catch2/catch.hpp"
#include "helpers.hpp"
#include "parse/error.hpp"
#include "parse/node_expr_binary.hpp"
#include "parse/node_expr_is.hpp"
#include "parse/node_expr_paren.hpp"
#include "parse/node_expr_unary.hpp"

namespace parse {

TEST_CASE("Parsing is expressions", "[parse]") {

  CHECK_EXPR("x is int i", isExprNode(CONST("x"), IS, TYPE("int"), ID("i")));
  CHECK_EXPR("x is int _", isExprNode(CONST("x"), IS, TYPE("int"), DISCARD));
  CHECK_EXPR("-x is int i", isExprNode(unaryExprNode(MINUS, CONST("x")), IS, TYPE("int"), ID("i")));
  CHECK_EXPR(
      "(x + y) is int i",
      isExprNode(
          parenExprNode(OPAREN, binaryExprNode(CONST("x"), PLUS, CONST("y")), CPAREN),
          IS,
          TYPE("int"),
          ID("i")));
  CHECK_EXPR(
      "x is List{T{Y}} i", isExprNode(CONST("x"), IS, TYPE("List", TYPE("T", TYPE("Y"))), ID("i")));

  SECTION("Errors") {
    CHECK_EXPR("x is", errInvalidIsExpr(CONST("x"), IS, Type(END), END));
    CHECK_EXPR("x is int", errInvalidIsExpr(CONST("x"), IS, TYPE("int"), END));
    CHECK_EXPR("x is is i", errInvalidIsExpr(CONST("x"), IS, Type(IS), ID("i")));
    CHECK_EXPR("x is int is", errInvalidIsExpr(CONST("x"), IS, TYPE("int"), IS));
    CHECK_EXPR(
        "x is int{} i",
        errInvalidIsExpr(
            CONST("x"), IS, Type(ID("int"), TypeParamList(OCURLY, {}, {}, CCURLY)), ID("i")));
  }

  SECTION("Spans") { CHECK_EXPR_SPAN("x is int i", input::Span(0, 9)); }
}

} // namespace parse
