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

    auto args = std::vector<prog::expr::NodePtr>{};
    args.push_back(prog::expr::litFloatNode(output.getProg(), 42.0f));
    auto callExpr = prog::expr::callExprNode(
        output.getProg(),
        GET_INTRINSIC_ID(output, "float_sin", GET_TYPE_ID(output, "float")),
        std::move(args));

    CHECK(func.getExpr() == *callExpr);
  }

  SECTION("Call impure intrinsic expression") {
    const auto& output = ANALYZE("act a() -> int intrinsic{platform_endianness_native}()");
    REQUIRE(output.isSuccess());
    const auto& func = GET_FUNC_DEF(output, "a");

    auto callExpr = prog::expr::callExprNode(
        output.getProg(), GET_INTRINSIC_ID(output, "platform_endianness_native"), {});

    CHECK(func.getExpr() == *callExpr);
  }

  SECTION("Diagnostics") {
    CHECK_DIAG(
        "fun f() "
        "  intrinsic{test}",
        errIntrinsicFuncLiteral(src, input::Span{10, 24}));
    CHECK_DIAG(
        "fun f() -> int "
        "  intrinsic{test}",
        errIntrinsicFuncLiteral(src, input::Span{17, 31}));
    CHECK_DIAG(
        "fun f() "
        "  intrinsic{does_not_exist}()",
        errUnknownIntrinsic(src, "does_not_exist", true, {}, input::Span{10, 36}));
    CHECK_DIAG(
        "act a() "
        "  intrinsic{does_not_exist}()",
        errUnknownIntrinsic(src, "does_not_exist", false, {}, input::Span{10, 36}));
    CHECK_DIAG(
        "fun f() "
        "  intrinsic{does_not_exist}(1)",
        errUnknownIntrinsic(src, "does_not_exist", true, {"int"}, input::Span{10, 37}));
    CHECK_DIAG(
        "act a() "
        "  intrinsic{does_not_exist}(1)",
        errUnknownIntrinsic(src, "does_not_exist", false, {"int"}, input::Span{10, 37}));
    CHECK_DIAG(
        "fun f() "
        "  intrinsic{does_not_exist}(1, false)",
        errUnknownIntrinsic(src, "does_not_exist", true, {"int", "bool"}, input::Span{10, 44}));
    CHECK_DIAG(
        "act a() "
        "  intrinsic{does_not_exist}(1, false)",
        errUnknownIntrinsic(src, "does_not_exist", false, {"int", "bool"}, input::Span{10, 44}));
    CHECK_DIAG(
        "fun f() "
        "  intrinsic{platform_endianness_native}()",
        errUnknownIntrinsic(src, "platform_endianness_native", true, {}, input::Span{10, 48}));
    CHECK_DIAG(
        "fun f() -> future{float} lazy intrinsic{float_sin}(42.0)",
        errLazyNonUserFunc(src, input::Span{25, 55}));
    CHECK_DIAG(
        "fun f() -> future{float} fork intrinsic{float_sin}(42.0)",
        errForkedNonUserFunc(src, input::Span{25, 55}));
  }
}

} // namespace frontend
