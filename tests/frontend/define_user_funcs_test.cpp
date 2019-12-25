#include "catch2/catch.hpp"
#include "frontend/diag_defs.hpp"
#include "helpers.hpp"
#include "prog/expr/node_const.hpp"
#include "prog/expr/node_lit_int.hpp"

namespace frontend {

TEST_CASE("Analyzing user-function definitions", "[frontend]") {

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

  SECTION("Define function with conversion") {
    const auto& output = ANALYZE("fun f() -> string 2");
    REQUIRE(output.isSuccess());
    const auto& funcDef = GET_FUNC_DEF(output, "f");

    CHECK(
        funcDef.getExpr() ==
        *applyConv(output, "int", "string", prog::expr::litIntNode(output.getProg(), 2)));
  }

  SECTION("Define templated function") {
    const auto& output = ANALYZE("fun f{T}(T a) -> T a "
                                 "fun f1() f{int}(42)");
    REQUIRE(output.isSuccess());
    const auto& funcDef = GET_FUNC_DEF(output, "f__int", GET_TYPE_ID(output, "int"));
    const auto& consts  = funcDef.getConsts();
    auto a              = consts.lookup("a");
    REQUIRE(a);
    CHECK(consts[a.value()].getKind() == prog::sym::ConstKind::Input);
    CHECK(consts[a.value()].getType() == GET_TYPE_ID(output, "int"));
    CHECK(funcDef.getExpr() == *prog::expr::constExprNode(consts, *a));
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
    CHECK_DIAG(
        "fun f2() -> int f{int}(1)", errUndeclaredFunc(src, "f", {"int"}, input::Span{16, 24}));
    CHECK_DIAG(
        "fun f{T}(T t) t "
        "fun f2() -> int f{int, float}(1)",
        errUndeclaredFunc(src, "f", {"int"}, input::Span{32, 47}));
    CHECK_DIAG(
        "fun f{T}(T T) -> T T "
        "fun f2() -> int f{int}(1)",
        errConstNameConflictsWithTypeSubstitution(src, "T", input::Span{11, 11}),
        errInvalidFuncInstantiation(src, input::Span{37, 37}));
    CHECK_DIAG(
        "fun f{T}(T T) -> T T "
        "fun f2() f{int}(1)",
        errConstNameConflictsWithTypeSubstitution(src, "T", input::Span{11, 11}),
        errInvalidFuncInstantiation(src, input::Span{30, 30}));
    CHECK_DIAG(
        "fun f{T}(T i) -> T "
        "  T = i * 2; i "
        "fun f2() -> int f{int}(1)",
        errConstNameConflictsWithTypeSubstitution(src, "T", input::Span{21, 21}),
        errInvalidFuncInstantiation(src, input::Span{50, 50}));
  }
}

} // namespace frontend
