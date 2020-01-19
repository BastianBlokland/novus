#include "catch2/catch.hpp"
#include "frontend/diag_defs.hpp"
#include "helpers.hpp"
#include "prog/expr/node_lit_int.hpp"
#include "prog/expr/node_lit_string.hpp"

namespace frontend {

TEST_CASE("Analyzing execute statements", "[frontend]") {

  SECTION("Define exec statement") {
    const auto& output = ANALYZE("print(\"hello world\")");
    REQUIRE(output.isSuccess());
    auto execsBegin     = output.getProg().beginExecStmts();
    const auto execsEnd = output.getProg().endExecStmts();

    auto args = std::vector<prog::expr::NodePtr>{};
    args.push_back(prog::expr::litStringNode(output.getProg(), "hello world"));

    CHECK(
        execsBegin->getExpr() ==
        *prog::expr::callExprNode(
            output.getProg(),
            GET_FUNC_ID(output, "print", GET_TYPE_ID(output, "string")),
            std::move(args)));
    REQUIRE(++execsBegin == execsEnd);
  }

  SECTION("Define exec statement with conversion") {
    const auto& output = ANALYZE("print(2)");
    REQUIRE(output.isSuccess());

    auto execsBegin     = output.getProg().beginExecStmts();
    const auto execsEnd = output.getProg().endExecStmts();

    auto args = std::vector<prog::expr::NodePtr>{};
    args.push_back(applyConv(output, "int", "string", prog::expr::litIntNode(output.getProg(), 2)));

    CHECK(
        execsBegin->getExpr() ==
        *prog::expr::callExprNode(
            output.getProg(),
            GET_FUNC_ID(output, "print", GET_TYPE_ID(output, "string")),
            std::move(args)));
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
    CHECK_DIAG("things()", errInvalidExecStmt(src, "things", {}, input::Span{0, 7}));
    CHECK_DIAG(
        "print(1, 1)",
        errInvalidExecStmt(
            src, "print", std::vector<std::string>{"int", "int"}, input::Span{0, 10}));
    CHECK_DIAG(
        "fun f() -> int 1 "
        "f()",
        errInvalidExecStmt(src, "f", {}, input::Span{17, 19}));
    CHECK_DIAG("print(test())", errUndeclaredFuncOrAction(src, "test", {}, input::Span{6, 11}));
  }
}

} // namespace frontend
