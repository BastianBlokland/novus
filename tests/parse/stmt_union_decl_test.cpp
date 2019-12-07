#include "catch2/catch.hpp"
#include "helpers.hpp"
#include "parse/error.hpp"
#include "parse/node_stmt_union_decl.hpp"

namespace parse {

TEST_CASE("Parsing union declaration statements", "[parse]") {

  CHECK_STMT(
      "union u = int, float",
      unionDeclStmtNode(UNION, ID("u"), EQ, {ID("int"), ID("float")}, COMMAS(1)));
  CHECK_STMT(
      "union u = int, float, bool",
      unionDeclStmtNode(UNION, ID("u"), EQ, {ID("int"), ID("float"), ID("bool")}, COMMAS(2)));

  SECTION("Errors") {
    CHECK_STMT("union u =", errInvalidStmtUnionDecl(UNION, ID("u"), EQ, {}, COMMAS(0)));
    CHECK_STMT(
        "union u = int", errInvalidStmtUnionDecl(UNION, ID("u"), EQ, {ID("int")}, COMMAS(0)));
    CHECK_STMT(
        "union u = int,", errInvalidStmtUnionDecl(UNION, ID("u"), EQ, {ID("int")}, COMMAS(1)));
    CHECK_STMT(
        "union fun = int, float",
        errInvalidStmtUnionDecl(UNION, FUN, EQ, {ID("int"), ID("float")}, COMMAS(1)));
    CHECK_STMT(
        "union u fun int, float",
        errInvalidStmtUnionDecl(UNION, ID("u"), FUN, {ID("int"), ID("float")}, COMMAS(1)));
  }

  SECTION("Spans") {
    CHECK_STMT_SPAN(" union u = int, float ", input::Span(1, 20));
    CHECK_STMT_SPAN(" union u = int, float, bool ", input::Span(1, 26));
  }
}

} // namespace parse
