#include "catch2/catch.hpp"
#include "helpers.hpp"
#include "parse/node_stmt_print.hpp"

namespace parse {

TEST_CASE("Parsing print statements", "[parse]") {

  CHECK_STMT("print 1", printStmtNode(PRINT, INT(1)));

  SECTION("Errors") { CHECK_STMT("print", printStmtNode(PRINT, errInvalidPrimaryExpr(END))); }
}

} // namespace parse
