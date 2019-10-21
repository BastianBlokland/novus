#include "catch2/catch.hpp"
#include "helpers.hpp"
#include "parse/error.hpp"
#include "parse/node_stmt_print.hpp"

namespace parse {

TEST_CASE("Parsing print statements", "[parse]") {

  CHECK_STMT("print 1", printStmtNode(PRINT, INT(1)));
  CHECK_STMT("print 1 print 2", printStmtNode(PRINT, INT(1)), printStmtNode(PRINT, INT(2)));

  SECTION("Errors") { CHECK_STMT("print", printStmtNode(PRINT, errInvalidPrimaryExpr(END))); }

  SECTION("Spans") { CHECK_STMT_SPAN(" print  1 + 2", lex::SourceSpan(1, 12)); }
}

} // namespace parse
