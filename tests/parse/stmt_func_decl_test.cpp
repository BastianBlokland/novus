#include "catch2/catch.hpp"
#include "helpers.hpp"
#include "parse/argument_list_decl.hpp"
#include "parse/error.hpp"
#include "parse/node_expr_binary.hpp"
#include "parse/node_expr_call.hpp"
#include "parse/node_expr_id.hpp"
#include "parse/node_stmt_func_decl.hpp"

namespace parse {

TEST_CASE("[parse] Parsing function declaration statements", "parse") {

  SECTION("Pure functions") {
    CHECK_STMT(
        "fun a() 1",
        funcDeclStmtNode(
            FUN,
            {},
            ID("a"),
            std::nullopt,
            ArgumentListDecl(OPAREN, NO_ARGS, COMMAS(0), CPAREN),
            std::nullopt,
            INT(1)));
    CHECK_STMT(
        "fun a() -> int 1",
        funcDeclStmtNode(
            FUN,
            {},
            ID("a"),
            std::nullopt,
            ArgumentListDecl(OPAREN, NO_ARGS, COMMAS(0), CPAREN),
            RetTypeSpec{ARROW, TYPE("int")},
            INT(1)));
    CHECK_STMT(
        "fun a(int x) -> int 1",
        funcDeclStmtNode(
            FUN,
            {},
            ID("a"),
            std::nullopt,
            ArgumentListDecl(
                OPAREN, ARGS(ArgumentListDecl::ArgSpec(TYPE("int"), ID("x"))), COMMAS(0), CPAREN),
            RetTypeSpec{ARROW, TYPE("int")},
            INT(1)));
    CHECK_STMT(
        "fun a(int x = 0) -> int 1",
        funcDeclStmtNode(
            FUN,
            {},
            ID("a"),
            std::nullopt,
            ArgumentListDecl(
                OPAREN,
                ARGS(ArgumentListDecl::ArgSpec(
                    TYPE("int"), ID("x"), ArgumentListDecl::ArgInitializer(EQ, INT(0)))),
                COMMAS(0),
                CPAREN),
            RetTypeSpec{ARROW, TYPE("int")},
            INT(1)));
    CHECK_STMT(
        "fun a(int x = true, int y = 42) -> int 1",
        funcDeclStmtNode(
            FUN,
            {},
            ID("a"),
            std::nullopt,
            ArgumentListDecl(
                OPAREN,
                ARGS(
                    ArgumentListDecl::ArgSpec(
                        TYPE("int"), ID("x"), ArgumentListDecl::ArgInitializer(EQ, BOOL(true))),
                    ArgumentListDecl::ArgSpec(
                        TYPE("int"), ID("y"), ArgumentListDecl::ArgInitializer(EQ, INT(42)))),
                COMMAS(1),
                CPAREN),
            RetTypeSpec{ARROW, TYPE("int")},
            INT(1)));
    CHECK_STMT(
        "fun a(int x, int y, bool z) -> int x * y",
        funcDeclStmtNode(
            FUN,
            {},
            ID("a"),
            std::nullopt,
            ArgumentListDecl(
                OPAREN,
                ARGS(
                    ArgumentListDecl::ArgSpec(TYPE("int"), ID("x")),
                    ArgumentListDecl::ArgSpec(TYPE("int"), ID("y")),
                    ArgumentListDecl::ArgSpec(TYPE("bool"), ID("z"))),
                COMMAS(2),
                CPAREN),
            RetTypeSpec{ARROW, TYPE("int")},
            binaryExprNode(ID_EXPR("x"), STAR, ID_EXPR("y"))));
    CHECK_STMT(
        "fun a{T}() 1",
        funcDeclStmtNode(
            FUN,
            {},
            ID("a"),
            TypeSubstitutionList{OCURLY, {ID("T")}, COMMAS(0), CCURLY},
            ArgumentListDecl(OPAREN, NO_ARGS, COMMAS(0), CPAREN),
            std::nullopt,
            INT(1)));
    CHECK_STMT(
        "fun a{T, U}() 1",
        funcDeclStmtNode(
            FUN,
            {},
            ID("a"),
            TypeSubstitutionList{OCURLY, {ID("T"), ID("U")}, COMMAS(1), CCURLY},
            ArgumentListDecl(OPAREN, NO_ARGS, COMMAS(0), CPAREN),
            std::nullopt,
            INT(1)));
    CHECK_STMT(
        "fun a{T, U, W}(T x) -> T x",
        funcDeclStmtNode(
            FUN,
            {},
            ID("a"),
            TypeSubstitutionList{OCURLY, {ID("T"), ID("U"), ID("W")}, COMMAS(2), CCURLY},
            ArgumentListDecl(
                OPAREN, ARGS(ArgumentListDecl::ArgSpec(TYPE("T"), ID("x"))), COMMAS(0), CPAREN),
            RetTypeSpec{ARROW, TYPE("T")},
            ID_EXPR("x")));
    CHECK_STMT(
        "fun a() -> List{T{Y}} 1",
        funcDeclStmtNode(
            FUN,
            {},
            ID("a"),
            std::nullopt,
            ArgumentListDecl(OPAREN, NO_ARGS, COMMAS(0), CPAREN),
            RetTypeSpec{ARROW, TYPE("List", TYPE("T", TYPE("Y")))},
            INT(1)));
    CHECK_STMT(
        "fun a{T, U, W}(T x) -> T x",
        funcDeclStmtNode(
            FUN,
            {},
            ID("a"),
            TypeSubstitutionList{OCURLY, {ID("T"), ID("U"), ID("W")}, COMMAS(2), CCURLY},
            ArgumentListDecl(
                OPAREN, ARGS(ArgumentListDecl::ArgSpec(TYPE("T"), ID("x"))), COMMAS(0), CPAREN),
            RetTypeSpec{ARROW, TYPE("T")},
            ID_EXPR("x")));
    CHECK_STMT(
        "fun a{T, U}(T{U} x) x",
        funcDeclStmtNode(
            FUN,
            {},
            ID("a"),
            TypeSubstitutionList{OCURLY, {ID("T"), ID("U")}, COMMAS(1), CCURLY},
            ArgumentListDecl(
                OPAREN,
                ARGS(ArgumentListDecl::ArgSpec(TYPE("T", TYPE("U")), ID("x"))),
                COMMAS(0),
                CPAREN),
            std::nullopt,
            ID_EXPR("x")));
    CHECK_STMT(
        "fun a{T, U}(T{U} x = T{U}()) x",
        funcDeclStmtNode(
            FUN,
            {},
            ID("a"),
            TypeSubstitutionList{OCURLY, {ID("T"), ID("U")}, COMMAS(1), CCURLY},
            ArgumentListDecl(
                OPAREN,
                ARGS(ArgumentListDecl::ArgSpec(
                    TYPE("T", TYPE("U")),
                    ID("x"),
                    ArgumentListDecl::ArgInitializer(
                        EQ,
                        callExprNode(
                            {},
                            ID_EXPR_PARAM(
                                "T", TypeParamList(OCURLY, {TYPE("U")}, COMMAS(1), CCURLY)),
                            OPAREN,
                            NO_NODES,
                            COMMAS(0),
                            CPAREN)))),
                COMMAS(0),
                CPAREN),
            std::nullopt,
            ID_EXPR("x")));
    CHECK_STMT(
        "fun noinline a() 1",
        funcDeclStmtNode(
            FUN,
            {NOINLINE},
            ID("a"),
            std::nullopt,
            ArgumentListDecl(OPAREN, NO_ARGS, COMMAS(0), CPAREN),
            std::nullopt,
            INT(1)));
    CHECK_STMT(
        "fun implicit a() 1",
        funcDeclStmtNode(
            FUN,
            {IMPLICIT},
            ID("a"),
            std::nullopt,
            ArgumentListDecl(OPAREN, NO_ARGS, COMMAS(0), CPAREN),
            std::nullopt,
            INT(1)));
    CHECK_STMT(
        "fun noinline implicit a() 1",
        funcDeclStmtNode(
            FUN,
            {NOINLINE, IMPLICIT},
            ID("a"),
            std::nullopt,
            ArgumentListDecl(OPAREN, NO_ARGS, COMMAS(0), CPAREN),
            std::nullopt,
            INT(1)));
    CHECK_STMT(
        "fun a(#1 one) -> #42 42",
        funcDeclStmtNode(
            FUN,
            {},
            ID("a"),
            std::nullopt,
            ArgumentListDecl(
                OPAREN,
                ARGS(ArgumentListDecl::ArgSpec(STATIC_INT_TYPE(1), ID("one"))),
                COMMAS(0),
                CPAREN),
            RetTypeSpec{ARROW, STATIC_INT_TYPE(42)},
            INT(42)));
  }

  SECTION("Actions") {
    CHECK_STMT(
        "act a() 1",
        funcDeclStmtNode(
            ACT,
            {},
            ID("a"),
            std::nullopt,
            ArgumentListDecl(OPAREN, NO_ARGS, COMMAS(0), CPAREN),
            std::nullopt,
            INT(1)));
    CHECK_STMT(
        "act a(int x) -> int 1",
        funcDeclStmtNode(
            ACT,
            {},
            ID("a"),
            std::nullopt,
            ArgumentListDecl(
                OPAREN, ARGS(ArgumentListDecl::ArgSpec(TYPE("int"), ID("x"))), COMMAS(0), CPAREN),
            RetTypeSpec{ARROW, TYPE("int")},
            INT(1)));
    CHECK_STMT(
        "act a(int x = 0) -> int 1",
        funcDeclStmtNode(
            ACT,
            {},
            ID("a"),
            std::nullopt,
            ArgumentListDecl(
                OPAREN,
                ARGS(ArgumentListDecl::ArgSpec(
                    TYPE("int"), ID("x"), ArgumentListDecl::ArgInitializer(EQ, INT(0)))),
                COMMAS(0),
                CPAREN),
            RetTypeSpec{ARROW, TYPE("int")},
            INT(1)));
    CHECK_STMT(
        "act a{T, U}(T{U} x) x",
        funcDeclStmtNode(
            ACT,
            {},
            ID("a"),
            TypeSubstitutionList{OCURLY, {ID("T"), ID("U")}, COMMAS(1), CCURLY},
            ArgumentListDecl(
                OPAREN,
                ARGS(ArgumentListDecl::ArgSpec(TYPE("T", TYPE("U")), ID("x"))),
                COMMAS(0),
                CPAREN),
            std::nullopt,
            ID_EXPR("x")));
    CHECK_STMT(
        "act noinline a() 1",
        funcDeclStmtNode(
            ACT,
            {NOINLINE},
            ID("a"),
            std::nullopt,
            ArgumentListDecl(OPAREN, NO_ARGS, COMMAS(0), CPAREN),
            std::nullopt,
            INT(1)));
  }

  SECTION("Errors") {
    CHECK_STMT(
        "fun a() -> int",
        funcDeclStmtNode(
            FUN,
            {},
            ID("a"),
            std::nullopt,
            ArgumentListDecl(OPAREN, NO_ARGS, COMMAS(0), CPAREN),
            RetTypeSpec{ARROW, TYPE("int")},
            errInvalidPrimaryExpr(END)));
    CHECK_STMT(
        "fun",
        errInvalidStmtFuncDecl(
            FUN,
            {},
            END,
            std::nullopt,
            ArgumentListDecl(END, NO_ARGS, COMMAS(0), END),
            std::nullopt,
            errInvalidPrimaryExpr(END)));
    CHECK_STMT(
        "fun a{",
        errInvalidStmtFuncDecl(
            FUN,
            {},
            ID("a"),
            TypeSubstitutionList{OCURLY, {}, COMMAS(0), END},
            ArgumentListDecl(END, NO_ARGS, COMMAS(0), END),
            std::nullopt,
            errInvalidPrimaryExpr(END)));
    CHECK_STMT(
        "fun a{}(int x) -> int 1",
        errInvalidStmtFuncDecl(
            FUN,
            {},
            ID("a"),
            TypeSubstitutionList{OCURLY, {}, COMMAS(0), CCURLY},
            ArgumentListDecl(
                OPAREN, ARGS(ArgumentListDecl::ArgSpec(TYPE("int"), ID("x"))), COMMAS(0), CPAREN),
            RetTypeSpec{ARROW, TYPE("int")},
            INT(1)));
    CHECK_STMT(
        "fun a{T U}(int x) -> int 1",
        errInvalidStmtFuncDecl(
            FUN,
            {},
            ID("a"),
            TypeSubstitutionList{OCURLY, {ID("T"), ID("U")}, COMMAS(0), CCURLY},
            ArgumentListDecl(
                OPAREN, ARGS(ArgumentListDecl::ArgSpec(TYPE("int"), ID("x"))), COMMAS(0), CPAREN),
            RetTypeSpec{ARROW, TYPE("int")},
            INT(1)));
    CHECK_STMT(
        "fun _() 1",
        errInvalidStmtFuncDecl(
            FUN,
            {},
            DISCARD,
            std::nullopt,
            ArgumentListDecl(PARENPAREN, NO_ARGS, COMMAS(0), PARENPAREN),
            std::nullopt,
            INT(1)));
    CHECK_STMT(
        "fun a(",
        errInvalidStmtFuncDecl(
            FUN,
            {},
            ID("a"),
            std::nullopt,
            ArgumentListDecl(OPAREN, NO_ARGS, COMMAS(0), END),
            std::nullopt,
            errInvalidPrimaryExpr(END)));
    CHECK_STMT(
        "fun a(int x",
        errInvalidStmtFuncDecl(
            FUN,
            {},
            ID("a"),
            std::nullopt,
            ArgumentListDecl(
                OPAREN, ARGS(ArgumentListDecl::ArgSpec(TYPE("int"), ID("x"))), COMMAS(0), END),
            std::nullopt,
            errInvalidPrimaryExpr(END)));
    CHECK_STMT(
        "fun a(int",
        errInvalidStmtFuncDecl(
            FUN,
            {},
            ID("a"),
            std::nullopt,
            ArgumentListDecl(
                OPAREN, ARGS(ArgumentListDecl::ArgSpec(TYPE("int"), END)), COMMAS(0), END),
            std::nullopt,
            errInvalidPrimaryExpr(END)));
    CHECK_STMT(
        "fun a(int x int y) -> x",
        errInvalidStmtFuncDecl(
            FUN,
            {},
            ID("a"),
            std::nullopt,
            ArgumentListDecl(
                OPAREN,
                ARGS(
                    ArgumentListDecl::ArgSpec(TYPE("int"), ID("x")),
                    ArgumentListDecl::ArgSpec(TYPE("int"), ID("y"))),
                COMMAS(0),
                CPAREN,
                true),
            RetTypeSpec{ARROW, TYPE("x")},
            errInvalidPrimaryExpr(END)));
    CHECK_STMT(
        "fun a(int x int y) -> int 1",
        errInvalidStmtFuncDecl(
            FUN,
            {},
            ID("a"),
            std::nullopt,
            ArgumentListDecl(
                OPAREN,
                ARGS(
                    ArgumentListDecl::ArgSpec(TYPE("int"), ID("x")),
                    ArgumentListDecl::ArgSpec(TYPE("int"), ID("y"))),
                COMMAS(0),
                CPAREN,
                true),
            RetTypeSpec{ARROW, TYPE("int")},
            INT(1)));
    CHECK_STMT(
        "fun a(int x int y,) -> int 1",
        errInvalidStmtFuncDecl(
            FUN,
            {},
            ID("a"),
            std::nullopt,
            ArgumentListDecl(
                OPAREN,
                ARGS(
                    ArgumentListDecl::ArgSpec(TYPE("int"), ID("x")),
                    ArgumentListDecl::ArgSpec(TYPE("int"), ID("y"))),
                COMMAS(1),
                CPAREN,
                true),
            RetTypeSpec{ARROW, TYPE("int")},
            INT(1)));
    CHECK_STMT(
        "fun a(int x,) -> int 1",
        errInvalidStmtFuncDecl(
            FUN,
            {},
            ID("a"),
            std::nullopt,
            ArgumentListDecl(
                OPAREN, ARGS(ArgumentListDecl::ArgSpec(TYPE("int"), ID("x"))), COMMAS(1), CPAREN),
            RetTypeSpec{ARROW, TYPE("int")},
            INT(1)));
    CHECK_STMT(
        "fun a(int x,,) -> int",
        errInvalidStmtFuncDecl(
            FUN,
            {},
            ID("a"),
            std::nullopt,
            ArgumentListDecl(
                OPAREN,
                ARGS(
                    ArgumentListDecl::ArgSpec(TYPE("int"), ID("x")),
                    ArgumentListDecl::ArgSpec(Type(COMMA), CPAREN)),
                COMMAS(1),
                ARROW),
            std::nullopt,
            ID_EXPR("int")));
    CHECK_STMT(
        "fun a() -> T{} 1",
        errInvalidStmtFuncDecl(
            FUN,
            {},
            ID("a"),
            std::nullopt,
            ArgumentListDecl(PARENPAREN, NO_ARGS, COMMAS(0), PARENPAREN),
            RetTypeSpec{ARROW, Type(ID("T"), TypeParamList(OCURLY, {}, {}, CCURLY))},
            INT(1)));
    CHECK_STMT(
        "fun a{T, U}(T{} x) x",
        errInvalidStmtFuncDecl(
            FUN,
            {},
            ID("a"),
            TypeSubstitutionList{OCURLY, {ID("T"), ID("U")}, COMMAS(1), CCURLY},
            ArgumentListDecl(
                OPAREN,
                ARGS(ArgumentListDecl::ArgSpec(
                    Type(ID("T"), TypeParamList(OCURLY, {}, {}, CCURLY)), ID("x"))),
                COMMAS(0),
                CPAREN),
            std::nullopt,
            ID_EXPR("x")));
    CHECK_STMT(
        "fun a(int x =) -> int 1",
        errInvalidStmtFuncDecl(
            FUN,
            {},
            ID("a"),
            std::nullopt,
            ArgumentListDecl(
                OPAREN,
                ARGS(ArgumentListDecl::ArgSpec(
                    TYPE("int"), ID("x"), ArgumentListDecl::ArgInitializer(EQ, nullptr))),
                COMMAS(0),
                CPAREN),
            RetTypeSpec{ARROW, TYPE("int")},
            INT(1)));
  }

  SECTION("Spans") {
    CHECK_STMT_SPAN(" fun a() -> int 1 + 2", input::Span(1, 20));
    CHECK_STMT_SPAN("fun a{T, U}() -> int 1", input::Span(0, 21));
  }
}

} // namespace parse
