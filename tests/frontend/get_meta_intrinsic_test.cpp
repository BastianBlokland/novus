#include "catch2/catch.hpp"
#include "frontend/diag_defs.hpp"
#include "helpers.hpp"
#include "prog/expr/node_lit_string.hpp"

namespace frontend {

TEST_CASE("[frontend] Analyzing meta intrinsics", "frontend") {

  SECTION("Get typename intrinsic") {
    const auto& output = ANALYZE("fun f() -> string intrinsic{type_name}{int}()");
    REQUIRE(output.isSuccess());

    CHECK(
        GET_FUNC_DEF(output, "f").getBody() == *prog::expr::litStringNode(output.getProg(), "int"));
  }

  SECTION("Get typename intrinsic for custom type") {
    const auto& output = ANALYZE("struct HelloWorld "
                                 "fun f() -> string intrinsic{type_name}{HelloWorld}()");
    REQUIRE(output.isSuccess());

    CHECK(
        GET_FUNC_DEF(output, "f").getBody() ==
        *prog::expr::litStringNode(output.getProg(), "HelloWorld"));
  }

  SECTION("Get typename intrinsic for templated type") {
    const auto& output = ANALYZE("struct Hello{T} "
                                 "fun f() -> string intrinsic{type_name}{Hello{int}}()");
    REQUIRE(output.isSuccess());

    CHECK(
        GET_FUNC_DEF(output, "f").getBody() ==
        *prog::expr::litStringNode(output.getProg(), "Hello{int}"));
  }

  SECTION("Get fail intrinsic") {
    const auto& output = ANALYZE("act a() -> int intrinsic{fail}{int}()");
    REQUIRE(output.isSuccess());

    auto callExpr = prog::expr::callExprNode(
        output.getProg(), GET_INTRINSIC_ID(output, "__fail_int"), NO_EXPRS);

    CHECK(GET_FUNC_DEF(output, "a").getBody() == *callExpr);
  }

  SECTION("Diagnostics") {
    CHECK_DIAG("fun f() -> int fail{int}()", errNoPureFuncFoundToInstantiate(NO_SRC, "fail", 1));
  }
}

} // namespace frontend
