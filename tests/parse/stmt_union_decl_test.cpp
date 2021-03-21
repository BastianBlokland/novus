#include "catch2/catch.hpp"
#include "helpers.hpp"
#include "parse/error.hpp"
#include "parse/node_stmt_union_decl.hpp"

namespace parse {

TEST_CASE("[parse] Parsing union declaration statements", "parse") {

  CHECK_STMT(
      "union u = int, float",
      unionDeclStmtNode(UNION, ID("u"), std::nullopt, EQ, {TYPE("int"), TYPE("float")}, COMMAS(1)));
  CHECK_STMT(
      "union u = int, float, bool",
      unionDeclStmtNode(
          UNION, ID("u"), std::nullopt, EQ, {TYPE("int"), TYPE("float"), TYPE("bool")}, COMMAS(2)));
  CHECK_STMT(
      "union u{T, U} = T, U",
      unionDeclStmtNode(
          UNION,
          ID("u"),
          TypeSubstitutionList{OCURLY, {ID("T"), ID("U")}, COMMAS(1), CCURLY},
          EQ,
          {TYPE("T"), TYPE("U")},
          COMMAS(1)));
  CHECK_STMT(
      "union u{T, U} = List{T{U}}, U",
      unionDeclStmtNode(
          UNION,
          ID("u"),
          TypeSubstitutionList{OCURLY, {ID("T"), ID("U")}, COMMAS(1), CCURLY},
          EQ,
          {TYPE("List", TYPE("T", TYPE("U"))), TYPE("U")},
          COMMAS(1)));
  CHECK_STMT(
      "union u = #1, #2",
      unionDeclStmtNode(
          UNION,
          ID("u"),
          std::nullopt,
          EQ,
          {
              STATIC_INT_TYPE(1),
              STATIC_INT_TYPE(2),
          },
          COMMAS(1)));

  SECTION("Errors") {
    CHECK_STMT(
        "union u =", errInvalidStmtUnionDecl(UNION, ID("u"), std::nullopt, EQ, {}, COMMAS(0)));
    CHECK_STMT(
        "union u = int",
        errInvalidStmtUnionDecl(UNION, ID("u"), std::nullopt, EQ, {TYPE("int")}, COMMAS(0)));
    CHECK_STMT(
        "union u = int,",
        errInvalidStmtUnionDecl(UNION, ID("u"), std::nullopt, EQ, {TYPE("int")}, COMMAS(1)));
    CHECK_STMT(
        "union fun = int, float",
        errInvalidStmtUnionDecl(
            UNION, FUN, std::nullopt, EQ, {TYPE("int"), TYPE("float")}, COMMAS(1)));
    CHECK_STMT(
        "union u fun int, float",
        errInvalidStmtUnionDecl(
            UNION, ID("u"), std::nullopt, FUN, {TYPE("int"), TYPE("float")}, COMMAS(1)));
    CHECK_STMT(
        "union u{} = int, float",
        errInvalidStmtUnionDecl(
            UNION,
            ID("u"),
            TypeSubstitutionList{OCURLY, {}, COMMAS(0), CCURLY},
            EQ,
            {TYPE("int"), TYPE("float")},
            COMMAS(1)));
    CHECK_STMT(
        "union u{T, U} = T{}, U",
        errInvalidStmtUnionDecl(
            UNION,
            ID("u"),
            TypeSubstitutionList{OCURLY, {ID("T"), ID("U")}, COMMAS(1), CCURLY},
            EQ,
            {Type(ID("T"), TypeParamList(OCURLY, {}, {}, CCURLY)), TYPE("U")},
            COMMAS(1)));
  }

  SECTION("Spans") {
    CHECK_STMT_SPAN(" union u = int, float ", input::Span(1, 20));
    CHECK_STMT_SPAN(" union u = int, float, bool ", input::Span(1, 26));
  }
}

} // namespace parse
