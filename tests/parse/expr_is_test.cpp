#include "catch2/catch.hpp"
#include "helpers.hpp"
#include "parse/error.hpp"
#include "parse/node_expr_binary.hpp"
#include "parse/node_expr_is.hpp"
#include "parse/node_expr_paren.hpp"
#include "parse/node_expr_unary.hpp"

namespace parse {

TEST_CASE("[parse] Parsing is / as expressions", "parse") {

  CHECK_EXPR("x as int i", isExprNode(ID_EXPR("x"), AS, TYPE("int"), ID("i")));
  CHECK_EXPR("x as int _", isExprNode(ID_EXPR("x"), AS, TYPE("int"), DISCARD));
  CHECK_EXPR("x is int", isExprNode(ID_EXPR("x"), IS, TYPE("int"), std::nullopt));
  CHECK_EXPR(
      "-x as int i", isExprNode(unaryExprNode(MINUS, ID_EXPR("x")), AS, TYPE("int"), ID("i")));
  CHECK_EXPR(
      "(x + y) as int i",
      isExprNode(
          parenExprNode(OPAREN, binaryExprNode(ID_EXPR("x"), PLUS, ID_EXPR("y")), CPAREN),
          AS,
          TYPE("int"),
          ID("i")));
  CHECK_EXPR(
      "x as List{T{Y}} i",
      isExprNode(ID_EXPR("x"), AS, TYPE("List", TYPE("T", TYPE("Y"))), ID("i")));
  CHECK_EXPR(
      "x is List{T{Y}}",
      isExprNode(ID_EXPR("x"), IS, TYPE("List", TYPE("T", TYPE("Y"))), std::nullopt));
  CHECK_EXPR("x as fork f", isExprNode(ID_EXPR("x"), AS, Type(FORK), ID("f")));

  SECTION("Errors") {
    CHECK_EXPR("x as", errInvalidIsExpr(ID_EXPR("x"), AS, Type(END), END));
    CHECK_EXPR("x as int", errInvalidIsExpr(ID_EXPR("x"), AS, TYPE("int"), END));
    CHECK_EXPR("x as int is", errInvalidIsExpr(ID_EXPR("x"), AS, TYPE("int"), IS));
    CHECK_EXPR(
        "x as int{} i",
        errInvalidIsExpr(
            ID_EXPR("x"), AS, Type(ID("int"), TypeParamList(OCURLY, {}, {}, CCURLY)), ID("i")));
  }

  SECTION("Spans") { CHECK_EXPR_SPAN("x as int i", input::Span(0, 9)); }
}

} // namespace parse
