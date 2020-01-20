#include "catch2/catch.hpp"
#include "helpers.hpp"
#include "parse/error.hpp"
#include "parse/node_stmt_import.hpp"

namespace parse {

TEST_CASE("Parsing import statements", "[parse]") {

  CHECK_STMT("import \"test.nov\"", importStmtNode(IMPORT, STR_TOK("test.nov")));
  CHECK_STMT("import \"std/test.nov\"", importStmtNode(IMPORT, STR_TOK("std/test.nov")));
  CHECK_STMT("import \"../test.nov\"", importStmtNode(IMPORT, STR_TOK("../test.nov")));
  CHECK_STMT("import\"test.nov\"", importStmtNode(IMPORT, STR_TOK("test.nov")));

  SECTION("Errors") {
    CHECK_STMT("import", errInvalidStmtImport(IMPORT, END));
    CHECK_STMT("import test", errInvalidStmtImport(IMPORT, ID("test")));
  }

  SECTION("Spans") { CHECK_STMT_SPAN(" import \"test.nov\"", input::Span(1, 17)); }
}

} // namespace parse
