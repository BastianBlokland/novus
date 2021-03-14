#include "catch2/catch.hpp"
#include "helpers.hpp"
#include "parse/error.hpp"
#include "parse/node_expr_call.hpp"
#include "parse/node_expr_group.hpp"
#include "parse/node_expr_intrinsic.hpp"
#include "parse/node_stmt_exec.hpp"

namespace parse {

TEST_CASE("[parse] Parsing execute statements", "parse") {

  CHECK_STMT(
      "exec()",
      execStmtNode(callExprNode({}, ID_EXPR("exec"), OPAREN, NO_NODES, COMMAS(0), CPAREN)));
  CHECK_STMT(
      "intrinsic{exec}()",
      execStmtNode(callExprNode(
          {},
          intrinsicExprNode(INTRINSIC, OCURLY, ID("exec"), CCURLY, std::nullopt),
          OPAREN,
          NO_NODES,
          COMMAS(0),
          CPAREN)));
  CHECK_STMT(
      "exec(1,2)",
      execStmtNode(
          callExprNode({}, ID_EXPR("exec"), OPAREN, NODES(INT(1), INT(2)), COMMAS(1), CPAREN)));
  CHECK_STMT(
      "exec(1,2,3+4*5)",
      execStmtNode(callExprNode(
          {},
          ID_EXPR("exec"),
          OPAREN,
          NODES(INT(1), INT(2), binaryExprNode(INT(3), PLUS, binaryExprNode(INT(4), STAR, INT(5)))),
          COMMAS(2),
          CPAREN)));
  CHECK_STMT(
      "exec(1;2,3;4)",
      execStmtNode(callExprNode(
          {},
          ID_EXPR("exec"),
          OPAREN,
          NODES(
              groupExprNode(NODES(INT(1), INT(2)), SEMIS(1)),
              groupExprNode(NODES(INT(3), INT(4)), SEMIS(1))),
          COMMAS(1),
          CPAREN)));
  CHECK_STMT(
      "exec(a())",
      execStmtNode(callExprNode(
          {},
          ID_EXPR("exec"),
          OPAREN,
          NODES(callExprNode({}, ID_EXPR("a"), OPAREN, NO_NODES, COMMAS(0), CPAREN)),
          COMMAS(0),
          CPAREN)));
  CHECK_STMT(
      "exec{int}()",
      execStmtNode(callExprNode(
          {},
          ID_EXPR_PARAM("exec", TypeParamList(OCURLY, {TYPE("int")}, COMMAS(0), CCURLY)),
          OPAREN,
          NO_NODES,
          COMMAS(0),
          CPAREN)));
  CHECK_STMT(
      "(exec)()",
      execStmtNode(callExprNode(
          {},
          parenExprNode(OPAREN, ID_EXPR("exec"), CPAREN),
          OPAREN,
          NO_NODES,
          COMMAS(0),
          CPAREN)));
  CHECK_STMT("1()", execStmtNode(callExprNode({}, INT(1), OPAREN, NO_NODES, COMMAS(0), CPAREN)));

  SECTION("Errors") {
    CHECK_STMT(
        "a(1 1)",
        execStmtNode(errInvalidCallExpr(
            {}, ID_EXPR("a"), OPAREN, NODES(INT(1), INT(1)), COMMAS(0), CPAREN, true)));
    CHECK_STMT(
        "a(1 1,)",
        execStmtNode(errInvalidCallExpr(
            {}, ID_EXPR("a"), OPAREN, NODES(INT(1), INT(1)), COMMAS(1), CPAREN, true)));
    CHECK_STMT(
        "a(",
        execStmtNode(
            errInvalidCallExpr({}, ID_EXPR("a"), OPAREN, NO_NODES, COMMAS(0), END, false)));
    CHECK_STMT(
        "a(,",
        execStmtNode(errInvalidCallExpr(
            {}, ID_EXPR("a"), OPAREN, NODES(errInvalidPrimaryExpr(COMMA)), COMMAS(0), END, true)));
    CHECK_STMT(
        "a(,)",
        execStmtNode(callExprNode(
            {}, ID_EXPR("a"), OPAREN, NODES(errInvalidPrimaryExpr(COMMA)), COMMAS(0), CPAREN)));
  }

  SECTION("Spans") { CHECK_STMT_SPAN("exec(1,2,3+4*5)", input::Span(0, 14)); }
}

} // namespace parse
