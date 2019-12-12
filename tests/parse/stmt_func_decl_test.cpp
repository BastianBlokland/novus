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
      funcDeclStmtNode(
          FUN, ID("a"), std::nullopt, OPAREN, {}, COMMAS(0), CPAREN, std::nullopt, INT(1)));
  CHECK_STMT(
      "fun a() -> int 1",
      funcDeclStmtNode(
          FUN,
          ID("a"),
          std::nullopt,
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
          std::nullopt,
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
          std::nullopt,
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
          std::nullopt,
          OPAREN,
          {FuncDeclStmtNode::ArgSpec(ID("int"), ID("x")),
           FuncDeclStmtNode::ArgSpec(ID("int"), ID("y")),
           FuncDeclStmtNode::ArgSpec(ID("bool"), ID("z"))},
          COMMAS(2),
          CPAREN,
          FuncDeclStmtNode::RetTypeSpec{ARROW, ID("int")},
          binaryExprNode(CONST("x"), STAR, CONST("y"))));
  CHECK_STMT(
      "fun a{T}() 1",
      funcDeclStmtNode(
          FUN,
          ID("a"),
          TypeParamList{OCURLY, {ID("T")}, COMMAS(0), CCURLY},
          OPAREN,
          {},
          COMMAS(0),
          CPAREN,
          std::nullopt,
          INT(1)));
  CHECK_STMT(
      "fun a{T, U}() 1",
      funcDeclStmtNode(
          FUN,
          ID("a"),
          TypeParamList{OCURLY, {ID("T"), ID("U")}, COMMAS(1), CCURLY},
          OPAREN,
          {},
          COMMAS(0),
          CPAREN,
          std::nullopt,
          INT(1)));
  CHECK_STMT(
      "fun a{T, U, W}(T x) -> T x",
      funcDeclStmtNode(
          FUN,
          ID("a"),
          TypeParamList{OCURLY, {ID("T"), ID("U"), ID("W")}, COMMAS(2), CCURLY},
          OPAREN,
          {FuncDeclStmtNode::ArgSpec(ID("T"), ID("x"))},
          COMMAS(0),
          CPAREN,
          FuncDeclStmtNode::RetTypeSpec{ARROW, ID("T")},
          CONST("x")));

  SECTION("Errors") {
    CHECK_STMT(
        "fun a() -> int",
        funcDeclStmtNode(
            FUN,
            ID("a"),
            std::nullopt,
            OPAREN,
            {},
            COMMAS(0),
            CPAREN,
            FuncDeclStmtNode::RetTypeSpec{ARROW, ID("int")},
            errInvalidPrimaryExpr(END)));
    CHECK_STMT(
        "fun",
        errInvalidStmtFuncDecl(
            FUN,
            END,
            std::nullopt,
            END,
            {},
            COMMAS(0),
            END,
            std::nullopt,
            errInvalidPrimaryExpr(END)));
    CHECK_STMT(
        "fun a{",
        errInvalidStmtFuncDecl(
            FUN,
            ID("a"),
            TypeParamList{OCURLY, {}, COMMAS(0), END},
            END,
            {},
            COMMAS(0),
            END,
            std::nullopt,
            errInvalidPrimaryExpr(END)));
    CHECK_STMT(
        "fun a{}(int x) -> int 1",
        errInvalidStmtFuncDecl(
            FUN,
            ID("a"),
            TypeParamList{OCURLY, {}, COMMAS(0), END},
            OPAREN,
            {FuncDeclStmtNode::ArgSpec(ID("int"), ID("x"))},
            COMMAS(0),
            CPAREN,
            FuncDeclStmtNode::RetTypeSpec{ARROW, ID("int")},
            INT(1)));
    CHECK_STMT(
        "fun a{T U}(int x) -> int 1",
        errInvalidStmtFuncDecl(
            FUN,
            ID("a"),
            TypeParamList{OCURLY, {ID("T"), ID("U")}, COMMAS(0), CCURLY},
            OPAREN,
            {FuncDeclStmtNode::ArgSpec(ID("int"), ID("x"))},
            COMMAS(0),
            CPAREN,
            FuncDeclStmtNode::RetTypeSpec{ARROW, ID("int")},
            INT(1)));
    CHECK_STMT(
        "fun _() 1",
        errInvalidStmtFuncDecl(
            FUN, DISCARD, std::nullopt, OPAREN, {}, COMMAS(0), CPAREN, std::nullopt, INT(1)));
    CHECK_STMT(
        "fun a(",
        errInvalidStmtFuncDecl(
            FUN,
            ID("a"),
            std::nullopt,
            OPAREN,
            {},
            COMMAS(0),
            END,
            std::nullopt,
            errInvalidPrimaryExpr(END)));
    CHECK_STMT(
        "fun a(int x",
        errInvalidStmtFuncDecl(
            FUN,
            ID("a"),
            std::nullopt,
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
            std::nullopt,
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
            std::nullopt,
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
            std::nullopt,
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
            std::nullopt,
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
            std::nullopt,
            OPAREN,
            {FuncDeclStmtNode::ArgSpec(ID("int"), ID("x")),
             FuncDeclStmtNode::ArgSpec(COMMA, CPAREN)},
            COMMAS(1),
            ARROW,
            std::nullopt,
            constExprNode(ID("int"))));
  }

  SECTION("Spans") {
    CHECK_STMT_SPAN(" fun a() -> int 1 + 2", input::Span(1, 20));
    CHECK_STMT_SPAN("fun a{T, U}() -> int 1", input::Span(0, 21));
  }
}

} // namespace parse
