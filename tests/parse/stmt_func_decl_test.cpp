#include "catch2/catch.hpp"
#include "helpers.hpp"
#include "parse/error.hpp"
#include "parse/node_expr_binary.hpp"
#include "parse/node_expr_call.hpp"
#include "parse/node_expr_group.hpp"
#include "parse/node_stmt_func_decl.hpp"
#include <utility>

namespace parse {

TEST_CASE("Parsing function declaration statements", "[parse]") {

  CHECK_STMT(
      "int a() 1", funcDeclStmtNode(ID("int"), ID("a"), OPAREN, {}, COMMAS(0), CPAREN, INT(1)));
  CHECK_STMT(
      "int a(int x) 1",
      funcDeclStmtNode(
          ID("int"),
          ID("a"),
          OPAREN,
          {std::make_pair(ID("int"), ID("x"))},
          COMMAS(0),
          CPAREN,
          INT(1)));
  CHECK_STMT(
      "int a(int x, int y) 1",
      funcDeclStmtNode(
          ID("int"),
          ID("a"),
          OPAREN,
          {std::make_pair(ID("int"), ID("x")), std::make_pair(ID("int"), ID("y"))},
          COMMAS(1),
          CPAREN,
          INT(1)));
  CHECK_STMT(
      "int a(int x, int y, bool z) x * y",
      funcDeclStmtNode(
          ID("int"),
          ID("a"),
          OPAREN,
          {std::make_pair(ID("int"), ID("x")),
           std::make_pair(ID("int"), ID("y")),
           std::make_pair(ID("bool"), ID("z"))},
          COMMAS(2),
          CPAREN,
          binaryExprNode(CONST("x"), STAR, CONST("y"))));

  SECTION("Errors") {
    CHECK_STMT(
        "int a()",
        funcDeclStmtNode(
            ID("int"), ID("a"), OPAREN, {}, COMMAS(0), CPAREN, errInvalidPrimaryExpr(END)));
    CHECK_STMT(
        "int a(",
        errInvalidStmtFuncDecl(
            ID("int"), ID("a"), OPAREN, {}, COMMAS(0), END, errInvalidPrimaryExpr(END)));
    CHECK_STMT(
        "int a(int x",
        errInvalidStmtFuncDecl(
            ID("int"),
            ID("a"),
            OPAREN,
            {std::make_pair(ID("int"), ID("x"))},
            COMMAS(0),
            END,
            errInvalidPrimaryExpr(END)));
    CHECK_STMT(
        "int a(int",
        errInvalidStmtFuncDecl(
            ID("int"),
            ID("a"),
            OPAREN,
            {std::make_pair(ID("int"), END)},
            COMMAS(0),
            END,
            errInvalidPrimaryExpr(END)));
    CHECK_STMT(
        "int a(int x int y) 1",
        errInvalidStmtFuncDecl(
            ID("int"),
            ID("a"),
            OPAREN,
            {std::make_pair(ID("int"), ID("x")), std::make_pair(ID("int"), ID("y"))},
            COMMAS(0),
            CPAREN,
            INT(1)));
    CHECK_STMT(
        "int a(int x,) 1",
        errInvalidStmtFuncDecl(
            ID("int"),
            ID("a"),
            OPAREN,
            {std::make_pair(ID("int"), ID("x"))},
            COMMAS(1),
            CPAREN,
            INT(1)));
    CHECK_STMT(
        "int a(int x,,) 1",
        errInvalidStmtFuncDecl(
            ID("int"),
            ID("a"),
            OPAREN,
            {std::make_pair(ID("int"), ID("x")),
             std::make_pair(COMMA, CPAREN),
             std::make_pair(lex::litIntToken(1), END)},
            COMMAS(1),
            END,
            errInvalidPrimaryExpr(END)));
  }
}

} // namespace parse
