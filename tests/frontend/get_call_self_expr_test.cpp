#include "catch2/catch.hpp"
#include "frontend/diag_defs.hpp"
#include "helpers.hpp"
#include "prog/expr/node_call_self.hpp"
#include "prog/expr/node_lit_bool.hpp"
#include "prog/expr/node_lit_int.hpp"
#include "prog/expr/node_switch.hpp"

namespace frontend {

TEST_CASE("[frontend] Analyzing self call expressions", "frontend") {

  SECTION("Get basic self call") {
    const auto& output = ANALYZE("fun f() false ? self() : 1 ");
    REQUIRE(output.isSuccess());

    auto switchExpr = prog::expr::switchExprNode(
        output.getProg(),
        EXPRS(prog::expr::litBoolNode(output.getProg(), false)),
        EXPRS(
            prog::expr::callSelfExprNode(GET_TYPE_ID(output, "int"), {}),
            prog::expr::litIntNode(output.getProg(), 1)));

    CHECK(GET_FUNC_DEF(output, "f").getBody() == *switchExpr);
  }

  SECTION("Get anonymous function with self call") {
    const auto& output = ANALYZE("fun f() lambda () false ? self() : 1 ");
    REQUIRE(output.isSuccess());

    auto switchExpr = prog::expr::switchExprNode(
        output.getProg(),
        EXPRS(prog::expr::litBoolNode(output.getProg(), false)),
        EXPRS(
            prog::expr::callSelfExprNode(GET_TYPE_ID(output, "int"), {}),
            prog::expr::litIntNode(output.getProg(), 1)));

    CHECK(findAnonFuncDef(output, 0).getBody() == *switchExpr);
  }

  SECTION("Diagnostics") {
    CHECK_DIAG("conWrite(self())", errSelfCallInNonFunc(NO_SRC));
    CHECK_DIAG(
        "fun f(int i) intrinsic{int_le_int}(i, 0) ? fork self(0) : i", errForkedSelfCall(NO_SRC));
    CHECK_DIAG(
        "fun f(int i) intrinsic{int_le_int}(i, 0) ? lazy self(0) : i", errLazySelfCall(NO_SRC));
    CHECK_DIAG("fun f() self()", errSelfCallWithoutInferredRetType(NO_SRC));
    CHECK_DIAG(
        "fun f(int i) intrinsic{int_le_int}(i, 0) ? self() : i",
        errIncorrectNumArgsInSelfCall(NO_SRC, 1, 0));
    CHECK_DIAG(
        "fun f(int i) intrinsic{int_le_int}(i, 0) ? self(i, 1) : i",
        errIncorrectNumArgsInSelfCall(NO_SRC, 1, 2));
    CHECK_DIAG(
        "fun f(int i) intrinsic{int_le_int}(i, 0) ? self(\"hello\") : i",
        errNoImplicitConversionFound(NO_SRC, "string", "int"));
  }
}

} // namespace frontend
