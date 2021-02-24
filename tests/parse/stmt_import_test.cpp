#include "catch2/catch.hpp"
#include "helpers.hpp"
#include "parse/error.hpp"
#include "parse/node_stmt_import.hpp"

namespace parse {

TEST_CASE("[parse] Parsing import statements", "parse") {

  CHECK_STMT("import \"test.ns\"", importStmtNode(IMPORT, STR_TOK("test.ns")));
  CHECK_STMT("import \"std/test.ns\"", importStmtNode(IMPORT, STR_TOK("std/test.ns")));
  CHECK_STMT("import \"../test.ns\"", importStmtNode(IMPORT, STR_TOK("../test.ns")));
  CHECK_STMT("import\"test.ns\"", importStmtNode(IMPORT, STR_TOK("test.ns")));

  SECTION("Errors") {
    CHECK_STMT("import", errInvalidStmtImport(IMPORT, END));
    CHECK_STMT("import test", errInvalidStmtImport(IMPORT, ID("test")));
  }

  SECTION("Spans") { CHECK_STMT_SPAN(" import \"test.ns\"", input::Span(1, 16)); }
}

} // namespace parse
