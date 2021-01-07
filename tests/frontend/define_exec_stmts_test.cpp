#include "catch2/catch.hpp"
#include "frontend/diag_defs.hpp"
#include "helpers.hpp"
#include "prog/expr/node_lit_bool.hpp"
#include "prog/expr/node_lit_enum.hpp"
#include "prog/expr/node_lit_int.hpp"
#include "prog/expr/node_lit_string.hpp"

namespace frontend {

TEST_CASE("Analyzing execute statements", "[frontend]") {

  SECTION("Define exec statement") {
    const auto& output = ANALYZE("assert(true, \"hello world\")");
    REQUIRE(output.isSuccess());
    auto execsBegin     = output.getProg().beginExecStmts();
    const auto execsEnd = output.getProg().endExecStmts();

    auto args = std::vector<prog::expr::NodePtr>{};
    args.push_back(prog::expr::litBoolNode(output.getProg(), true));
    args.push_back(prog::expr::litStringNode(output.getProg(), "hello world"));
    auto callExpr = prog::expr::callExprNode(
        output.getProg(),
        GET_FUNC_ID(output, "assert", GET_TYPE_ID(output, "bool"), GET_TYPE_ID(output, "string")),
        std::move(args));

    CHECK(execsBegin->getExpr() == *callExpr);
    REQUIRE(++execsBegin == execsEnd);
  }

  SECTION("Define exec statement with conversion") {
    const auto& output = ANALYZE("enum ConsoleKind = StdIn, StdOut, StdErr "
                                 "consoleOpenStream(ConsoleKind.StdOut)");
    REQUIRE(output.isSuccess());

    auto execsBegin     = output.getProg().beginExecStmts();
    const auto execsEnd = output.getProg().endExecStmts();

    auto args = std::vector<prog::expr::NodePtr>{};
    args.push_back(applyConv(
        output,
        "ConsoleKind",
        "int",
        prog::expr::litEnumNode(output.getProg(), GET_TYPE_ID(output, "ConsoleKind"), "StdOut")));
    auto callExpr = prog::expr::callExprNode(
        output.getProg(),
        GET_FUNC_ID(output, "consoleOpenStream", GET_TYPE_ID(output, "int")),
        std::move(args));

    CHECK(execsBegin->getExpr() == *callExpr);
    REQUIRE(++execsBegin == execsEnd);
  }

  SECTION("Define exec statement with const") {
    const auto& output = ANALYZE("assert(x = 5; x == 1, \"msg\")");
    REQUIRE(output.isSuccess());
    auto execsBegin        = output.getProg().beginExecStmts();
    const auto execsEnd    = output.getProg().endExecStmts();
    const auto& constTable = execsBegin->getConsts();
    const auto x           = constTable.lookup("x");
    REQUIRE(x);
    CHECK(constTable[x.value()].getType() == GET_TYPE_ID(output, "int"));
    REQUIRE(++execsBegin == execsEnd);
  }

  SECTION("Define exec statement to custom action") {
    const auto& output = ANALYZE("act main() "
                                 " assert(true, \"hello \") "
                                 "main()");
    REQUIRE(output.isSuccess());
    auto execsBegin     = output.getProg().beginExecStmts();
    const auto execsEnd = output.getProg().endExecStmts();

    CHECK(
        execsBegin->getExpr() ==
        *prog::expr::callExprNode(output.getProg(), GET_FUNC_ID(output, "main"), {}));
    REQUIRE(++execsBegin == execsEnd);
  }

  SECTION("Diagnostics") {
    CHECK_DIAG("things()", errUndeclaredAction(src, "things", {}, input::Span{0, 7}));
    CHECK_DIAG(
        "assert(1, 1)",
        errUndeclaredAction(
            src, "assert", std::vector<std::string>{"int", "int"}, input::Span{0, 11}));
    CHECK_DIAG(
        "fun f() -> int 1 "
        "f()",
        errUndeclaredAction(src, "f", {}, input::Span{17, 19}));
    CHECK_DIAG("assert(test())", errUndeclaredFuncOrAction(src, "test", {}, input::Span{7, 12}));
  }
}

} // namespace frontend
