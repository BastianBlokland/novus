#include "catch2/catch.hpp"
#include "helpers.hpp"
#include "parse/error.hpp"
#include "parse/node_expr_binary.hpp"
#include "parse/node_expr_is.hpp"
#include "parse/node_expr_paren.hpp"
#include "parse/node_expr_unary.hpp"

namespace parse {

TEST_CASE("Parsing is expressions", "[parse]") {

  CHECK_EXPR("x is int i", isExprNode(ID_EXPR("x"), IS, TYPE("int"), ID("i")));
  CHECK_EXPR("x is int _", isExprNode(ID_EXPR("x"), IS, TYPE("int"), DISCARD));
  CHECK_EXPR(
      "-x is int i", isExprNode(unaryExprNode(MINUS, ID_EXPR("x")), IS, TYPE("int"), ID("i")));
  CHECK_EXPR(
      "(x + y) is int i",
      isExprNode(
          parenExprNode(OPAREN, binaryExprNode(ID_EXPR("x"), PLUS, ID_EXPR("y")), CPAREN),
          IS,
          TYPE("int"),
          ID("i")));
  CHECK_EXPR(
      "x is List{T{Y}} i",
      isExprNode(ID_EXPR("x"), IS, TYPE("List", TYPE("T", TYPE("Y"))), ID("i")));

  SECTION("Errors") {
    CHECK_EXPR("x is", errInvalidIsExpr(ID_EXPR("x"), IS, Type(END), END));
    CHECK_EXPR("x is int", errInvalidIsExpr(ID_EXPR("x"), IS, TYPE("int"), END));
    CHECK_EXPR("x is is i", errInvalidIsExpr(ID_EXPR("x"), IS, Type(IS), ID("i")));
    CHECK_EXPR("x is int is", errInvalidIsExpr(ID_EXPR("x"), IS, TYPE("int"), IS));
    CHECK_EXPR(
        "x is int{} i",
        errInvalidIsExpr(
            ID_EXPR("x"), IS, Type(ID("int"), TypeParamList(OCURLY, {}, {}, CCURLY)), ID("i")));
  }

  SECTION("Spans") { CHECK_EXPR_SPAN("x is int i", input::Span(0, 9)); }
}

} // namespace parse
