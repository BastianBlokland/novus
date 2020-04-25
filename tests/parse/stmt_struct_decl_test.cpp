#include "catch2/catch.hpp"
#include "helpers.hpp"
#include "parse/error.hpp"
#include "parse/node_stmt_struct_decl.hpp"

namespace parse {

TEST_CASE("Parsing struct declaration statements", "[parse]") {

  CHECK_STMT(
      "struct s = int a",
      structDeclStmtNode(
          STRUCT,
          ID("s"),
          std::nullopt,
          EQ,
          {StructDeclStmtNode::FieldSpec(TYPE("int"), ID("a"))},
          COMMAS(0)));
  CHECK_STMT("struct s", structDeclStmtNode(STRUCT, ID("s"), std::nullopt, std::nullopt, {}, {}));
  CHECK_STMT(
      "struct s = int a, bool b",
      structDeclStmtNode(
          STRUCT,
          ID("s"),
          std::nullopt,
          EQ,
          {StructDeclStmtNode::FieldSpec(TYPE("int"), ID("a")),
           StructDeclStmtNode::FieldSpec(TYPE("bool"), ID("b"))},
          COMMAS(1)));
  CHECK_STMT(
      "struct s = int a, bool b, string c",
      structDeclStmtNode(
          STRUCT,
          ID("s"),
          std::nullopt,
          EQ,
          {StructDeclStmtNode::FieldSpec(TYPE("int"), ID("a")),
           StructDeclStmtNode::FieldSpec(TYPE("bool"), ID("b")),
           StructDeclStmtNode::FieldSpec(TYPE("string"), ID("c"))},
          COMMAS(2)));
  CHECK_STMT(
      "struct s{T, U} = T a, U b",
      structDeclStmtNode(
          STRUCT,
          ID("s"),
          TypeSubstitutionList{OCURLY, {ID("T"), ID("U")}, COMMAS(1), CCURLY},
          EQ,
          {StructDeclStmtNode::FieldSpec(TYPE("T"), ID("a")),
           StructDeclStmtNode::FieldSpec(TYPE("U"), ID("b"))},
          COMMAS(1)));
  CHECK_STMT(
      "struct s{T, U} = List{T{U}} a",
      structDeclStmtNode(
          STRUCT,
          ID("s"),
          TypeSubstitutionList{OCURLY, {ID("T"), ID("U")}, COMMAS(1), CCURLY},
          EQ,
          {StructDeclStmtNode::FieldSpec(TYPE("List", TYPE("T", TYPE("U"))), ID("a"))},
          COMMAS(0)));
  CHECK_STMT(
      "struct s = fork f",
      structDeclStmtNode(
          STRUCT,
          ID("s"),
          std::nullopt,
          EQ,
          {StructDeclStmtNode::FieldSpec(Type(FORK), ID("f"))},
          COMMAS(0)));

  SECTION("Errors") {
    CHECK_STMT(
        "struct s =", errInvalidStmtStructDecl(STRUCT, ID("s"), std::nullopt, EQ, {}, COMMAS(0)));
    CHECK_STMT(
        "struct s = int a,",
        errInvalidStmtStructDecl(
            STRUCT,
            ID("s"),
            std::nullopt,
            EQ,
            {StructDeclStmtNode::FieldSpec(TYPE("int"), ID("a"))},
            COMMAS(1)));
    CHECK_STMT(
        "struct fun = int a",
        errInvalidStmtStructDecl(
            STRUCT,
            FUN,
            std::nullopt,
            EQ,
            {StructDeclStmtNode::FieldSpec(TYPE("int"), ID("a"))},
            COMMAS(0)));
    CHECK_STMT(
        "struct s = int fun",
        errInvalidStmtStructDecl(
            STRUCT,
            ID("s"),
            std::nullopt,
            EQ,
            {StructDeclStmtNode::FieldSpec(TYPE("int"), FUN)},
            COMMAS(0)));
    CHECK_STMT(
        "struct s{} = int a",
        errInvalidStmtStructDecl(
            STRUCT,
            ID("s"),
            TypeSubstitutionList{OCURLY, {}, COMMAS(0), CCURLY},
            EQ,
            {StructDeclStmtNode::FieldSpec(TYPE("int"), ID("a"))},
            COMMAS(0)));
    CHECK_STMT(
        "struct s{T, U} = T{} a",
        errInvalidStmtStructDecl(
            STRUCT,
            ID("s"),
            TypeSubstitutionList{OCURLY, {ID("T"), ID("U")}, COMMAS(1), CCURLY},
            EQ,
            {StructDeclStmtNode::FieldSpec(
                Type(ID("T"), TypeParamList(OCURLY, {}, {}, CCURLY)), ID("a"))},
            COMMAS(0)));
  }

  SECTION("Spans") {
    CHECK_STMT_SPAN(" struct user = int a ", input::Span(1, 19));
    CHECK_STMT_SPAN(" struct user = int a, bool b ", input::Span(1, 27));
  }
}

} // namespace parse
