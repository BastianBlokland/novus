#include "catch2/catch.hpp"
#include "frontend/diag_defs.hpp"
#include "helpers.hpp"
#include "prog/expr/node_lit_float.hpp"

namespace frontend {

TEST_CASE("[frontend] Analyzing intrinsic expressions", "frontend") {

  SECTION("Call intrinsic expression") {
    const auto& output = ANALYZE("fun f() -> float intrinsic{float_sin}(42.0)");
    REQUIRE(output.isSuccess());
    const auto& func = GET_FUNC_DEF(output, "f");

    auto callExpr = prog::expr::callExprNode(
        output.getProg(),
        GET_INTRINSIC_ID(output, "float_sin", GET_TYPE_ID(output, "float")),
        EXPRS(prog::expr::litFloatNode(output.getProg(), 42.0f)));

    CHECK(func.getBody() == *callExpr);
  }

  SECTION("Call impure intrinsic expression") {
    const auto& output = ANALYZE("act a() -> int intrinsic{platform_endianness_native}()");
    REQUIRE(output.isSuccess());
    const auto& func = GET_FUNC_DEF(output, "a");

    auto callExpr = prog::expr::callExprNode(
        output.getProg(), GET_INTRINSIC_ID(output, "platform_endianness_native"), {});

    CHECK(func.getBody() == *callExpr);
  }

  SECTION("Diagnostics") {
    CHECK_DIAG(
        "fun f() "
        "  intrinsic{test}",
        errIntrinsicFuncLiteral(NO_SRC));
    CHECK_DIAG(
        "fun f() -> int "
        "  intrinsic{test}",
        errIntrinsicFuncLiteral(NO_SRC));
    CHECK_DIAG(
        "fun f() "
        "  intrinsic{does_not_exist}()",
        errUnknownIntrinsic(NO_SRC, "does_not_exist", true, {}));
    CHECK_DIAG(
        "act a() "
        "  intrinsic{does_not_exist}()",
        errUnknownIntrinsic(NO_SRC, "does_not_exist", false, {}));
    CHECK_DIAG(
        "fun f() "
        "  intrinsic{does_not_exist}(1)",
        errUnknownIntrinsic(NO_SRC, "does_not_exist", true, {"int"}));
    CHECK_DIAG(
        "act a() "
        "  intrinsic{does_not_exist}(1)",
        errUnknownIntrinsic(NO_SRC, "does_not_exist", false, {"int"}));
    CHECK_DIAG(
        "fun f() "
        "  intrinsic{does_not_exist}(1, false)",
        errUnknownIntrinsic(NO_SRC, "does_not_exist", true, {"int", "bool"}));
    CHECK_DIAG(
        "act a() "
        "  intrinsic{does_not_exist}(1, false)",
        errUnknownIntrinsic(NO_SRC, "does_not_exist", false, {"int", "bool"}));
    CHECK_DIAG(
        "fun f() "
        "  intrinsic{platform_endianness_native}()",
        errUnknownIntrinsic(NO_SRC, "platform_endianness_native", true, {}));
    CHECK_DIAG(
        "fun f() -> future{float} lazy intrinsic{float_sin}(42.0)", errLazyNonUserFunc(NO_SRC));
    CHECK_DIAG(
        "fun f() -> future{float} fork intrinsic{float_sin}(42.0)", errForkedNonUserFunc(NO_SRC));
  }
}

} // namespace frontend
