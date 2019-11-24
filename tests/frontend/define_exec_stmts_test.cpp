#include "catch2/catch.hpp"
#include "frontend/diag_defs.hpp"
#include "helpers.hpp"
#include "prog/expr/node_lit_int.hpp"
#include "prog/expr/node_lit_string.hpp"
#include "prog/sym/input.hpp"

namespace frontend {

TEST_CASE("Define execute statements", "[frontend]") {

  SECTION("Define exec statement") {
    const auto& output = ANALYZE("print(\"hello world\")");
    REQUIRE(output.isSuccess());
    auto execsBegin     = output.getProg().beginExecStmts();
    const auto execsEnd = output.getProg().endExecStmts();

    CHECK(
        execsBegin->getActionId() == GET_ACTION_ID(output, "print", GET_TYPE_ID(output, "string")));
    CHECK(*execsBegin->getArgs()[0] == *prog::expr::litStringNode(output.getProg(), "hello world"));
    REQUIRE(++execsBegin == execsEnd);
  }

  SECTION("Define exec statement with const") {
    const auto& output = ANALYZE("print(string(x = 5; x + 1))");
    REQUIRE(output.isSuccess());
    auto execsBegin        = output.getProg().beginExecStmts();
    const auto execsEnd    = output.getProg().endExecStmts();
    const auto& constTable = execsBegin->getConsts();
    const auto x           = constTable.lookup("x");
    REQUIRE(x);
    CHECK(constTable[x.value()].getType() == GET_TYPE_ID(output, "int"));
    REQUIRE(++execsBegin == execsEnd);
  }

  SECTION("Diagnostics") {
    CHECK_DIAG("things()", errUndeclaredAction(src, "things", input::Span{0, 5}));
    CHECK_DIAG(
        "print(1, 1)",
        errUndeclaredActionOverload(
            src, "print", std::vector<std::string>{"int", "int"}, input::Span{0, 10}));
  }
}

} // namespace frontend
