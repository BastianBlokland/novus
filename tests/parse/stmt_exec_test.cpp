#include "catch2/catch.hpp"
#include "helpers.hpp"
#include "parse/error.hpp"
#include "parse/node_expr_call.hpp"
#include "parse/node_expr_group.hpp"
#include "parse/node_stmt_exec.hpp"

namespace parse {

TEST_CASE("Parsing execute statements", "[parse]") {

  CHECK_STMT("print()", execStmtNode(ID("print"), OPAREN, NODES(), COMMAS(0), CPAREN));
  CHECK_STMT(
      "print(1,2)", execStmtNode(ID("print"), OPAREN, NODES(INT(1), INT(2)), COMMAS(1), CPAREN));
  CHECK_STMT(
      "print(1,2,3+4*5)",
      execStmtNode(
          ID("print"),
          OPAREN,
          NODES(INT(1), INT(2), binaryExprNode(INT(3), PLUS, binaryExprNode(INT(4), STAR, INT(5)))),
          COMMAS(2),
          CPAREN));
  CHECK_STMT(
      "print(1;2,3;4)",
      execStmtNode(
          ID("print"),
          OPAREN,
          NODES(
              groupExprNode(NODES(INT(1), INT(2)), SEMIS(1)),
              groupExprNode(NODES(INT(3), INT(4)), SEMIS(1))),
          COMMAS(1),
          CPAREN));
  CHECK_STMT(
      "print(a())",
      execStmtNode(
          ID("print"),
          OPAREN,
          NODES(callExprNode(ID_EXPR("a"), std::nullopt, OPAREN, NODES(), COMMAS(0), CPAREN)),
          COMMAS(0),
          CPAREN));

  SECTION("Errors") {
    CHECK_STMT(
        "a(1 1)", errInvalidStmtExec(ID("a"), OPAREN, NODES(INT(1), INT(1)), COMMAS(0), CPAREN));
    CHECK_STMT("a(", errInvalidStmtExec(ID("a"), OPAREN, NODES(), COMMAS(0), END));
    CHECK_STMT(
        "a(,",
        errInvalidStmtExec(ID("a"), OPAREN, NODES(errInvalidPrimaryExpr(COMMA)), COMMAS(0), END));
    CHECK_STMT(
        "a(,)",
        execStmtNode(ID("a"), OPAREN, NODES(errInvalidPrimaryExpr(COMMA)), COMMAS(0), CPAREN));
  }

  SECTION("Spans") { CHECK_STMT_SPAN("print(1,2,3+4*5)", input::Span(0, 15)); }
}

} // namespace parse
