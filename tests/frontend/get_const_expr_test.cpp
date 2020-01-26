#include "catch2/catch.hpp"
#include "frontend/diag_defs.hpp"
#include "helpers.hpp"
#include "prog/expr/node_assign.hpp"
#include "prog/expr/node_const.hpp"
#include "prog/expr/node_group.hpp"
#include "prog/expr/node_lit_int.hpp"

namespace frontend {

TEST_CASE("Analyzing constant expressions", "[frontend]") {

  SECTION("Access function input") {
    const auto& output = ANALYZE("fun f(int a) a");
    REQUIRE(output.isSuccess());
    const auto& funcDef = GET_FUNC_DEF(output, "f", GET_TYPE_ID(output, "int"));
    const auto& consts  = funcDef.getConsts();
    CHECK(funcDef.getExpr() == *prog::expr::constExprNode(consts, consts.lookup("a").value()));
  }

  SECTION("Declare and access local const") {
    const auto& output = ANALYZE("fun f() a = 1; b = a; b");
    REQUIRE(output.isSuccess());
    const auto& funcDef = GET_FUNC_DEF(output, "f");
    const auto& consts  = funcDef.getConsts();

    const auto a = consts.lookup("a");
    REQUIRE(a);
    const auto b = consts.lookup("b");
    REQUIRE(b);

    auto exprs = std::vector<prog::expr::NodePtr>{};
    exprs.push_back(
        prog::expr::assignExprNode(consts, a.value(), prog::expr::litIntNode(output.getProg(), 1)));
    exprs.push_back(prog::expr::assignExprNode(
        consts, b.value(), prog::expr::constExprNode(consts, a.value())));
    exprs.push_back(prog::expr::constExprNode(consts, b.value()));

    CHECK(funcDef.getExpr() == *prog::expr::groupExprNode(std::move(exprs)));
  }

  SECTION("Access const in anonymous function") {
    const auto& output = ANALYZE("fun f() lambda () x = 1; x");
    REQUIRE(output.isSuccess());
    const auto& anonDef = findAnonFuncDef(output, 0);
    const auto& consts  = anonDef.getConsts();

    const auto x = consts.lookup("x");
    REQUIRE(x);

    auto exprs = std::vector<prog::expr::NodePtr>{};
    exprs.push_back(
        prog::expr::assignExprNode(consts, x.value(), prog::expr::litIntNode(output.getProg(), 1)));
    exprs.push_back(prog::expr::constExprNode(consts, x.value()));

    CHECK(anonDef.getExpr() == *prog::expr::groupExprNode(std::move(exprs)));
  }

  SECTION("Diagnostics") {
    CHECK_DIAG("fun f() -> int x", errUndeclaredConst(src, "x", input::Span{15, 15}));
    CHECK_DIAG(
        "fun f() -> int bool = 42",
        errConstNameConflictsWithType(src, "bool", input::Span{15, 18}));
    CHECK_DIAG(
        "fun f(int a) -> int a = 42",
        errConstNameConflictsWithConst(src, "a", input::Span{20, 20}));
    CHECK_DIAG(
        "fun f(int a) -> int "
        "if a > 5  -> b = 1 "
        "else      -> b = 2",
        errConstNameConflictsWithConst(src, "b", input::Span{52, 52}));
    CHECK_DIAG(
        "fun f(int a) -> int "
        "if a > 5  -> b = 1 "
        "else      -> b + 1",
        errUninitializedConst(src, "b", input::Span{52, 52}));
    CHECK_DIAG(
        "fun f(int a) -> int "
        "if b = a * a; b > 5  -> b "
        "else                 -> b + 1",
        errUninitializedConst(src, "b", input::Span{70, 70}));
    CHECK_DIAG(
        "fun f() -> int (true && (x = 5; false)); x",
        errUninitializedConst(src, "x", input::Span{41, 41}));
  }
}

} // namespace frontend
