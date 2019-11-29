#include "catch2/catch.hpp"
#include "helpers.hpp"
#include "parse/error.hpp"
#include "parse/node_stmt_struct_decl.hpp"

namespace parse {

TEST_CASE("Parsing struct declaration statements", "[parse]") {

  CHECK_STMT(
      "struct s = int a",
      structDeclStmtNode(
          STRUCT, ID("s"), EQ, {StructDeclStmtNode::FieldSpec(ID("int"), ID("a"))}, COMMAS(0)));
  CHECK_STMT(
      "struct s = int a, bool b",
      structDeclStmtNode(
          STRUCT,
          ID("s"),
          EQ,
          {StructDeclStmtNode::FieldSpec(ID("int"), ID("a")),
           StructDeclStmtNode::FieldSpec(ID("bool"), ID("b"))},
          COMMAS(1)));
  CHECK_STMT(
      "struct s = int a, bool b, string c",
      structDeclStmtNode(
          STRUCT,
          ID("s"),
          EQ,
          {StructDeclStmtNode::FieldSpec(ID("int"), ID("a")),
           StructDeclStmtNode::FieldSpec(ID("bool"), ID("b")),
           StructDeclStmtNode::FieldSpec(ID("string"), ID("c"))},
          COMMAS(2)));

  SECTION("Errors") {
    CHECK_STMT("struct s =", errInvalidStmtStructDecl(STRUCT, ID("s"), EQ, {}, COMMAS(0)));
    CHECK_STMT(
        "struct s = int a,",
        errInvalidStmtStructDecl(
            STRUCT, ID("s"), EQ, {StructDeclStmtNode::FieldSpec(ID("int"), ID("a"))}, COMMAS(1)));
    CHECK_STMT(
        "struct fun = int a",
        errInvalidStmtStructDecl(
            STRUCT, FUN, EQ, {StructDeclStmtNode::FieldSpec(ID("int"), ID("a"))}, COMMAS(0)));
    CHECK_STMT(
        "struct s fun int a",
        errInvalidStmtStructDecl(
            STRUCT, ID("s"), FUN, {StructDeclStmtNode::FieldSpec(ID("int"), ID("a"))}, COMMAS(0)));
  }

  SECTION("Spans") {
    CHECK_STMT_SPAN(" struct user = int a ", input::Span(1, 19));
    CHECK_STMT_SPAN(" struct user = int a, bool b ", input::Span(1, 27));
  }
}

} // namespace parse
