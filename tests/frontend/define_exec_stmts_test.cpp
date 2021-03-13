#include "catch2/catch.hpp"
#include "frontend/diag_defs.hpp"
#include "helpers.hpp"
#include "prog/expr/node_lit_bool.hpp"
#include "prog/expr/node_lit_enum.hpp"
#include "prog/expr/node_lit_int.hpp"
#include "prog/expr/node_lit_string.hpp"

namespace frontend {

TEST_CASE("[frontend] Analyzing execute statements", "frontend") {

  SECTION("Define exec statement") {
    const auto& output = ANALYZE("fun fa(bool b, string s) b "
                                 "fa(true, \"hello world\")");
    REQUIRE(output.isSuccess());
    auto execsBegin     = output.getProg().beginExecStmts();
    const auto execsEnd = output.getProg().endExecStmts();

    auto callExpr = prog::expr::callExprNode(
        output.getProg(),
        GET_FUNC_ID(output, "fa", GET_TYPE_ID(output, "bool"), GET_TYPE_ID(output, "string")),
        EXPRS(
            prog::expr::litBoolNode(output.getProg(), true),
            prog::expr::litStringNode(output.getProg(), "hello world")));

    CHECK(execsBegin->getExpr() == *callExpr);
    REQUIRE(++execsBegin == execsEnd);
  }

  SECTION("Define exec statement with conversion") {
    const auto& output = ANALYZE("enum ConsoleKind = StdIn, StdOut, StdErr "
                                 "act openConsole(int i) -> sys_stream "
                                 "  intrinsic{console_openstream}(i)"
                                 "openConsole(ConsoleKind.StdOut)");
    REQUIRE(output.isSuccess());

    auto execsBegin     = output.getProg().beginExecStmts();
    const auto execsEnd = output.getProg().endExecStmts();

    auto callExpr = prog::expr::callExprNode(
        output.getProg(),
        GET_FUNC_ID(output, "openConsole", GET_TYPE_ID(output, "int")),
        EXPRS(applyConv(
            output,
            "ConsoleKind",
            "int",
            prog::expr::litEnumNode(
                output.getProg(), GET_TYPE_ID(output, "ConsoleKind"), "StdOut"))));

    CHECK(execsBegin->getExpr() == *callExpr);
    REQUIRE(++execsBegin == execsEnd);
  }

  SECTION("Define exec statement with const") {
    const auto& output = ANALYZE("fun ==(int x, int y) -> bool intrinsic{int_eq_int}(x, y) "
                                 "fun fa(bool b, string s) b "
                                 "fa(x = 5; x == 1, \"msg\")");
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
    CHECK_DIAG("things()", errUndeclaredFuncOrAction(NO_SRC, "things", {}));
    CHECK_DIAG(
        "fa(1, 1)",
        errUndeclaredFuncOrAction(NO_SRC, "fa", std::vector<std::string>{"int", "int"}));
    CHECK_DIAG(
        "fun fa(bool b) b "
        "fa(test())",
        errUndeclaredFuncOrAction(NO_SRC, "test", {}));
  }
}

} // namespace frontend
