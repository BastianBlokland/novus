#include "catch2/catch.hpp"
#include "frontend/diag_defs.hpp"
#include "helpers.hpp"
#include "prog/expr/node_lit_int.hpp"
#include "prog/sym/input.hpp"

namespace frontend {

TEST_CASE("Define user functions", "[frontend]") {

  SECTION("Define basic function") {
    const auto& output = ANALYZE("fun f(int a) -> int 42");
    REQUIRE(output.isSuccess());
    const auto& funcDef = GET_FUNC_DEF(output, "f", GET_TYPE_ID(output, "int"));
    const auto& consts  = funcDef.getConsts();
    auto a              = consts.lookup("a");
    REQUIRE(a);
    CHECK(consts[a.value()].getKind() == prog::sym::ConstKind::Input);
    CHECK(consts[a.value()].getType() == GET_TYPE_ID(output, "int"));
    CHECK(funcDef.getExpr() == *prog::expr::litIntNode(output.getProg(), 42));
  }

  SECTION("Diagnostics") {
    CHECK_DIAG(
        "fun f() -> int true",
        errNonMatchingFuncReturnType(src, "f", "int", "bool", input::Span{15, 18}));
    CHECK_DIAG(
        "fun f(int int) -> int true",
        errConstNameConflictsWithType(src, "int", input::Span{10, 12}));
    CHECK_DIAG(
        "fun f(int f) -> int true",
        errConstNameConflictsWithFunction(src, "f", input::Span{10, 10}));
    CHECK_DIAG(
        "fun f(int print) -> int true",
        errConstNameConflictsWithAction(src, "print", input::Span{10, 14}));
    CHECK_DIAG(
        "fun f(int a, int a) -> int true",
        errConstNameConflictsWithConst(src, "a", input::Span{17, 17}));
  }
}

} // namespace frontend
