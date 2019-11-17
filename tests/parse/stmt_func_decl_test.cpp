#include "catch2/catch.hpp"
#include "helpers.hpp"
#include "parse/error.hpp"
#include "parse/node_expr_binary.hpp"
#include "parse/node_expr_const.hpp"
#include "parse/node_stmt_func_decl.hpp"

namespace parse {

TEST_CASE("Parsing function declaration statements", "[parse]") {

  CHECK_STMT(
      "fun a() 1",
      funcDeclStmtNode(FUN, ID("a"), OPAREN, {}, COMMAS(0), CPAREN, std::nullopt, INT(1)));
  CHECK_STMT(
      "fun a() -> int 1",
      funcDeclStmtNode(
          FUN,
          ID("a"),
          OPAREN,
          {},
          COMMAS(0),
          CPAREN,
          FuncDeclStmtNode::RetTypeSpec{ARROW, ID("int")},
          INT(1)));
  CHECK_STMT(
      "fun a(int x) -> int 1",
      funcDeclStmtNode(
          FUN,
          ID("a"),
          OPAREN,
          {FuncDeclStmtNode::ArgSpec(ID("int"), ID("x"))},
          COMMAS(0),
          CPAREN,
          FuncDeclStmtNode::RetTypeSpec{ARROW, ID("int")},
          INT(1)));
  CHECK_STMT(
      "fun a(int x, int y) -> int 1",
      funcDeclStmtNode(
          FUN,
          ID("a"),
          OPAREN,
          {FuncDeclStmtNode::ArgSpec(ID("int"), ID("x")),
           FuncDeclStmtNode::ArgSpec(ID("int"), ID("y"))},
          COMMAS(1),
          CPAREN,
          FuncDeclStmtNode::RetTypeSpec{ARROW, ID("int")},
          INT(1)));
  CHECK_STMT(
      "fun a(int x, int y, bool z) -> int x * y",
      funcDeclStmtNode(
          FUN,
          ID("a"),
          OPAREN,
          {FuncDeclStmtNode::ArgSpec(ID("int"), ID("x")),
           FuncDeclStmtNode::ArgSpec(ID("int"), ID("y")),
           FuncDeclStmtNode::ArgSpec(ID("bool"), ID("z"))},
          COMMAS(2),
          CPAREN,
          FuncDeclStmtNode::RetTypeSpec{ARROW, ID("int")},
          binaryExprNode(CONST("x"), STAR, CONST("y"))));

  SECTION("Errors") {
    CHECK_STMT(
        "fun a() -> int",
        funcDeclStmtNode(
            FUN,
            ID("a"),
            OPAREN,
            {},
            COMMAS(0),
            CPAREN,
            FuncDeclStmtNode::RetTypeSpec{ARROW, ID("int")},
            errInvalidPrimaryExpr(END)));
    CHECK_STMT(
        "fun",
        errInvalidStmtFuncDecl(
            FUN, END, END, {}, COMMAS(0), END, std::nullopt, errInvalidPrimaryExpr(END)));
    CHECK_STMT(
        "fun a(",
        errInvalidStmtFuncDecl(
            FUN, ID("a"), OPAREN, {}, COMMAS(0), END, std::nullopt, errInvalidPrimaryExpr(END)));
    CHECK_STMT(
        "fun a(int x",
        errInvalidStmtFuncDecl(
            FUN,
            ID("a"),
            OPAREN,
            {FuncDeclStmtNode::ArgSpec(ID("int"), ID("x"))},
            COMMAS(0),
            END,
            std::nullopt,
            errInvalidPrimaryExpr(END)));
    CHECK_STMT(
        "fun a(int",
        errInvalidStmtFuncDecl(
            FUN,
            ID("a"),
            OPAREN,
            {FuncDeclStmtNode::ArgSpec(ID("int"), END)},
            COMMAS(0),
            END,
            std::nullopt,
            errInvalidPrimaryExpr(END)));
    CHECK_STMT(
        "fun a(int x int y) -> x",
        errInvalidStmtFuncDecl(
            FUN,
            ID("a"),
            OPAREN,
            {FuncDeclStmtNode::ArgSpec(ID("int"), ID("x")),
             FuncDeclStmtNode::ArgSpec(ID("int"), ID("y"))},
            COMMAS(0),
            CPAREN,
            FuncDeclStmtNode::RetTypeSpec{ARROW, ID("x")},
            errInvalidPrimaryExpr(END)));
    CHECK_STMT(
        "fun a(int x int y) -> int 1",
        errInvalidStmtFuncDecl(
            FUN,
            ID("a"),
            OPAREN,
            {FuncDeclStmtNode::ArgSpec(ID("int"), ID("x")),
             FuncDeclStmtNode::ArgSpec(ID("int"), ID("y"))},
            COMMAS(0),
            CPAREN,
            FuncDeclStmtNode::RetTypeSpec{ARROW, ID("int")},
            INT(1)));
    CHECK_STMT(
        "fun a(int x,) -> int 1",
        errInvalidStmtFuncDecl(
            FUN,
            ID("a"),
            OPAREN,
            {FuncDeclStmtNode::ArgSpec(ID("int"), ID("x"))},
            COMMAS(1),
            CPAREN,
            FuncDeclStmtNode::RetTypeSpec{ARROW, ID("int")},
            INT(1)));
    CHECK_STMT(
        "fun a(int x,,) -> int",
        errInvalidStmtFuncDecl(
            FUN,
            ID("a"),
            OPAREN,
            {FuncDeclStmtNode::ArgSpec(ID("int"), ID("x")),
             FuncDeclStmtNode::ArgSpec(COMMA, CPAREN)},
            COMMAS(1),
            ARROW,
            std::nullopt,
            constExprNode(ID("int"))));
  }

  SECTION("Spans") { CHECK_STMT_SPAN(" fun a() -> int 1 + 2", input::Span(1, 20)); }
}

} // namespace parse
