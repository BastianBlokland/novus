#include "catch2/catch.hpp"
#include "helpers.hpp"
#include "parse/argument_list_decl.hpp"
#include "parse/error.hpp"
#include "parse/node_expr_binary.hpp"
#include "parse/node_expr_id.hpp"
#include "parse/node_stmt_func_decl.hpp"

namespace parse {

TEST_CASE("Parsing function declaration statements", "[parse]") {

  CHECK_STMT(
      "fun a() 1",
      funcDeclStmtNode(
          FUN,
          ID("a"),
          std::nullopt,
          ArgumentListDecl(OPAREN, {}, COMMAS(0), CPAREN),
          std::nullopt,
          INT(1)));
  CHECK_STMT(
      "fun a() -> int 1",
      funcDeclStmtNode(
          FUN,
          ID("a"),
          std::nullopt,
          ArgumentListDecl(OPAREN, {}, COMMAS(0), CPAREN),
          FuncDeclStmtNode::RetTypeSpec{ARROW, TYPE("int")},
          INT(1)));
  CHECK_STMT(
      "fun a(int x) -> int 1",
      funcDeclStmtNode(
          FUN,
          ID("a"),
          std::nullopt,
          ArgumentListDecl(
              OPAREN, {ArgumentListDecl::ArgSpec(TYPE("int"), ID("x"))}, COMMAS(0), CPAREN),
          FuncDeclStmtNode::RetTypeSpec{ARROW, TYPE("int")},
          INT(1)));
  CHECK_STMT(
      "fun a(int x, int y) -> int 1",
      funcDeclStmtNode(
          FUN,
          ID("a"),
          std::nullopt,
          ArgumentListDecl(
              OPAREN,
              {ArgumentListDecl::ArgSpec(TYPE("int"), ID("x")),
               ArgumentListDecl::ArgSpec(TYPE("int"), ID("y"))},
              COMMAS(1),
              CPAREN),
          FuncDeclStmtNode::RetTypeSpec{ARROW, TYPE("int")},
          INT(1)));
  CHECK_STMT(
      "fun a(int x, int y, bool z) -> int x * y",
      funcDeclStmtNode(
          FUN,
          ID("a"),
          std::nullopt,
          ArgumentListDecl(
              OPAREN,
              {ArgumentListDecl::ArgSpec(TYPE("int"), ID("x")),
               ArgumentListDecl::ArgSpec(TYPE("int"), ID("y")),
               ArgumentListDecl::ArgSpec(TYPE("bool"), ID("z"))},
              COMMAS(2),
              CPAREN),
          FuncDeclStmtNode::RetTypeSpec{ARROW, TYPE("int")},
          binaryExprNode(ID_EXPR("x"), STAR, ID_EXPR("y"))));
  CHECK_STMT(
      "fun a{T}() 1",
      funcDeclStmtNode(
          FUN,
          ID("a"),
          TypeSubstitutionList{OCURLY, {ID("T")}, COMMAS(0), CCURLY},
          ArgumentListDecl(OPAREN, {}, COMMAS(0), CPAREN),
          std::nullopt,
          INT(1)));
  CHECK_STMT(
      "fun a{T, U}() 1",
      funcDeclStmtNode(
          FUN,
          ID("a"),
          TypeSubstitutionList{OCURLY, {ID("T"), ID("U")}, COMMAS(1), CCURLY},
          ArgumentListDecl(OPAREN, {}, COMMAS(0), CPAREN),
          std::nullopt,
          INT(1)));
  CHECK_STMT(
      "fun a{T, U, W}(T x) -> T x",
      funcDeclStmtNode(
          FUN,
          ID("a"),
          TypeSubstitutionList{OCURLY, {ID("T"), ID("U"), ID("W")}, COMMAS(2), CCURLY},
          ArgumentListDecl(
              OPAREN, {ArgumentListDecl::ArgSpec(TYPE("T"), ID("x"))}, COMMAS(0), CPAREN),
          FuncDeclStmtNode::RetTypeSpec{ARROW, TYPE("T")},
          ID_EXPR("x")));
  CHECK_STMT(
      "fun a() -> List{T{Y}} 1",
      funcDeclStmtNode(
          FUN,
          ID("a"),
          std::nullopt,
          ArgumentListDecl(OPAREN, {}, COMMAS(0), CPAREN),
          FuncDeclStmtNode::RetTypeSpec{ARROW, TYPE("List", TYPE("T", TYPE("Y")))},
          INT(1)));
  CHECK_STMT(
      "fun a{T, U, W}(T x) -> T x",
      funcDeclStmtNode(
          FUN,
          ID("a"),
          TypeSubstitutionList{OCURLY, {ID("T"), ID("U"), ID("W")}, COMMAS(2), CCURLY},
          ArgumentListDecl(
              OPAREN, {ArgumentListDecl::ArgSpec(TYPE("T"), ID("x"))}, COMMAS(0), CPAREN),
          FuncDeclStmtNode::RetTypeSpec{ARROW, TYPE("T")},
          ID_EXPR("x")));
  CHECK_STMT(
      "fun a{T, U}(T{U} x) x",
      funcDeclStmtNode(
          FUN,
          ID("a"),
          TypeSubstitutionList{OCURLY, {ID("T"), ID("U")}, COMMAS(1), CCURLY},
          ArgumentListDecl(
              OPAREN,
              {ArgumentListDecl::ArgSpec(TYPE("T", TYPE("U")), ID("x"))},
              COMMAS(0),
              CPAREN),
          std::nullopt,
          ID_EXPR("x")));

  SECTION("Errors") {
    CHECK_STMT(
        "fun a() -> int",
        funcDeclStmtNode(
            FUN,
            ID("a"),
            std::nullopt,
            ArgumentListDecl(OPAREN, {}, COMMAS(0), CPAREN),
            FuncDeclStmtNode::RetTypeSpec{ARROW, TYPE("int")},
            errInvalidPrimaryExpr(END)));
    CHECK_STMT(
        "fun",
        errInvalidStmtFuncDecl(
            FUN,
            END,
            std::nullopt,
            ArgumentListDecl(END, {}, COMMAS(0), END),
            std::nullopt,
            errInvalidPrimaryExpr(END)));
    CHECK_STMT(
        "fun a{",
        errInvalidStmtFuncDecl(
            FUN,
            ID("a"),
            TypeSubstitutionList{OCURLY, {}, COMMAS(0), END},
            ArgumentListDecl(END, {}, COMMAS(0), END),
            std::nullopt,
            errInvalidPrimaryExpr(END)));
    CHECK_STMT(
        "fun a{}(int x) -> int 1",
        errInvalidStmtFuncDecl(
            FUN,
            ID("a"),
            TypeSubstitutionList{OCURLY, {}, COMMAS(0), CCURLY},
            ArgumentListDecl(
                OPAREN, {ArgumentListDecl::ArgSpec(TYPE("int"), ID("x"))}, COMMAS(0), CPAREN),
            FuncDeclStmtNode::RetTypeSpec{ARROW, TYPE("int")},
            INT(1)));
    CHECK_STMT(
        "fun a{T U}(int x) -> int 1",
        errInvalidStmtFuncDecl(
            FUN,
            ID("a"),
            TypeSubstitutionList{OCURLY, {ID("T"), ID("U")}, COMMAS(0), CCURLY},
            ArgumentListDecl(
                OPAREN, {ArgumentListDecl::ArgSpec(TYPE("int"), ID("x"))}, COMMAS(0), CPAREN),
            FuncDeclStmtNode::RetTypeSpec{ARROW, TYPE("int")},
            INT(1)));
    CHECK_STMT(
        "fun _() 1",
        errInvalidStmtFuncDecl(
            FUN,
            DISCARD,
            std::nullopt,
            ArgumentListDecl(PARENPAREN, {}, COMMAS(0), PARENPAREN),
            std::nullopt,
            INT(1)));
    CHECK_STMT(
        "fun a(",
        errInvalidStmtFuncDecl(
            FUN,
            ID("a"),
            std::nullopt,
            ArgumentListDecl(OPAREN, {}, COMMAS(0), END),
            std::nullopt,
            errInvalidPrimaryExpr(END)));
    CHECK_STMT(
        "fun a(int x",
        errInvalidStmtFuncDecl(
            FUN,
            ID("a"),
            std::nullopt,
            ArgumentListDecl(
                OPAREN, {ArgumentListDecl::ArgSpec(TYPE("int"), ID("x"))}, COMMAS(0), END),
            std::nullopt,
            errInvalidPrimaryExpr(END)));
    CHECK_STMT(
        "fun a(int",
        errInvalidStmtFuncDecl(
            FUN,
            ID("a"),
            std::nullopt,
            ArgumentListDecl(OPAREN, {ArgumentListDecl::ArgSpec(TYPE("int"), END)}, COMMAS(0), END),
            std::nullopt,
            errInvalidPrimaryExpr(END)));
    CHECK_STMT(
        "fun a(int x int y) -> x",
        errInvalidStmtFuncDecl(
            FUN,
            ID("a"),
            std::nullopt,
            ArgumentListDecl(
                OPAREN,
                {ArgumentListDecl::ArgSpec(TYPE("int"), ID("x")),
                 ArgumentListDecl::ArgSpec(TYPE("int"), ID("y"))},
                COMMAS(0),
                CPAREN),
            FuncDeclStmtNode::RetTypeSpec{ARROW, TYPE("x")},
            errInvalidPrimaryExpr(END)));
    CHECK_STMT(
        "fun a(int x int y) -> int 1",
        errInvalidStmtFuncDecl(
            FUN,
            ID("a"),
            std::nullopt,
            ArgumentListDecl(
                OPAREN,
                {ArgumentListDecl::ArgSpec(TYPE("int"), ID("x")),
                 ArgumentListDecl::ArgSpec(TYPE("int"), ID("y"))},
                COMMAS(0),
                CPAREN),
            FuncDeclStmtNode::RetTypeSpec{ARROW, TYPE("int")},
            INT(1)));
    CHECK_STMT(
        "fun a(int x,) -> int 1",
        errInvalidStmtFuncDecl(
            FUN,
            ID("a"),
            std::nullopt,
            ArgumentListDecl(
                OPAREN, {ArgumentListDecl::ArgSpec(TYPE("int"), ID("x"))}, COMMAS(1), CPAREN),
            FuncDeclStmtNode::RetTypeSpec{ARROW, TYPE("int")},
            INT(1)));
    CHECK_STMT(
        "fun a(int x,,) -> int",
        errInvalidStmtFuncDecl(
            FUN,
            ID("a"),
            std::nullopt,
            ArgumentListDecl(
                OPAREN,
                {ArgumentListDecl::ArgSpec(TYPE("int"), ID("x")),
                 ArgumentListDecl::ArgSpec(Type(COMMA), CPAREN)},
                COMMAS(1),
                ARROW),
            std::nullopt,
            ID_EXPR("int")));
    CHECK_STMT(
        "fun a() -> T{} 1",
        errInvalidStmtFuncDecl(
            FUN,
            ID("a"),
            std::nullopt,
            ArgumentListDecl(PARENPAREN, {}, COMMAS(0), PARENPAREN),
            FuncDeclStmtNode::RetTypeSpec{ARROW,
                                          Type(ID("T"), TypeParamList(OCURLY, {}, {}, CCURLY))},
            INT(1)));
    CHECK_STMT(
        "fun a{T, U}(T{} x) x",
        errInvalidStmtFuncDecl(
            FUN,
            ID("a"),
            TypeSubstitutionList{OCURLY, {ID("T"), ID("U")}, COMMAS(1), CCURLY},
            ArgumentListDecl(
                OPAREN,
                {ArgumentListDecl::ArgSpec(
                    Type(ID("T"), TypeParamList(OCURLY, {}, {}, CCURLY)), ID("x"))},
                COMMAS(0),
                CPAREN),
            std::nullopt,
            ID_EXPR("x")));
  }

  SECTION("Spans") {
    CHECK_STMT_SPAN(" fun a() -> int 1 + 2", input::Span(1, 20));
    CHECK_STMT_SPAN("fun a{T, U}() -> int 1", input::Span(0, 21));
  }
}

} // namespace parse
