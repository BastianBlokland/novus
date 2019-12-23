#include "catch2/catch.hpp"
#include "frontend/diag_defs.hpp"
#include "helpers.hpp"
#include "prog/expr/node_lit_int.hpp"

namespace frontend {

TEST_CASE("Analyzing call expressions", "[frontend]") {

  SECTION("Get basic call") {
    const auto& output = ANALYZE("fun f1() -> int 1 "
                                 "fun f2() -> int f1()");
    REQUIRE(output.isSuccess());
    CHECK(
        GET_FUNC_DEF(output, "f2").getExpr() ==
        *prog::expr::callExprNode(output.getProg(), GET_FUNC_ID(output, "f1"), {}));
  }

  SECTION("Get call with arg") {
    const auto& output = ANALYZE("fun f1(int a) -> int a + 1 "
                                 "fun f2() -> int f1(1)");
    REQUIRE(output.isSuccess());

    auto args = std::vector<prog::expr::NodePtr>{};
    args.push_back(prog::expr::litIntNode(output.getProg(), 1));

    CHECK(
        GET_FUNC_DEF(output, "f2").getExpr() ==
        *prog::expr::callExprNode(
            output.getProg(),
            GET_FUNC_ID(output, "f1", GET_TYPE_ID(output, "int")),
            std::move(args)));
  }

  SECTION("Get recursive call") {
    const auto& output = ANALYZE("fun f() -> int f()");
    REQUIRE(output.isSuccess());
    CHECK(
        GET_FUNC_DEF(output, "f").getExpr() ==
        *prog::expr::callExprNode(output.getProg(), GET_FUNC_ID(output, "f"), {}));
  }

  SECTION("Get call with conversion") {
    const auto& output = ANALYZE("fun f1(float a) a "
                                 "fun f2() f1(1)");
    REQUIRE(output.isSuccess());

    auto args = std::vector<prog::expr::NodePtr>{};
    args.push_back(applyConv(output, "int", "float", prog::expr::litIntNode(output.getProg(), 1)));

    CHECK(
        GET_FUNC_DEF(output, "f2").getExpr() ==
        *prog::expr::callExprNode(
            output.getProg(),
            GET_FUNC_ID(output, "f1", GET_TYPE_ID(output, "float")),
            std::move(args)));
  }

  SECTION("Get templated call") {
    const auto& output = ANALYZE("fun f1{T}(T t) -> T t "
                                 "fun f2() -> int f1{int}(1)");
    REQUIRE(output.isSuccess());

    auto args = std::vector<prog::expr::NodePtr>{};
    args.push_back(prog::expr::litIntNode(output.getProg(), 1));
    CHECK(
        GET_FUNC_DEF(output, "f2").getExpr() ==
        *prog::expr::callExprNode(
            output.getProg(),
            GET_FUNC_ID(output, "f1__int", GET_TYPE_ID(output, "int")),
            std::move(args)));
  }

  SECTION("Diagnostics") {
    CHECK_DIAG(
        "fun f1() -> int 1 "
        "fun f2() -> int f3()",
        errUndeclaredFunc(src, "f3", {}, input::Span{34, 37}));
    CHECK_DIAG(
        "fun f1() -> int 1 "
        "fun f2() -> int f2(1)",
        errUndeclaredFunc(src, "f2", {"int"}, input::Span{34, 38}));
  }
}

} // namespace frontend
