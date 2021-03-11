#include "catch2/catch.hpp"
#include "frontend/diag_defs.hpp"
#include "helpers.hpp"
#include "prog/expr/node_call.hpp"
#include "prog/expr/node_const.hpp"
#include "prog/expr/node_lit_int.hpp"
#include "prog/expr/node_lit_string.hpp"

namespace frontend {

TEST_CASE("[frontend] Analyzing user-type templates", "frontend") {

  SECTION("Construct templated struct") {
    const auto& output = ANALYZE("struct tuple{T1, T2} = T1 a, T2 b "
                                 "fun f() tuple{int, string}(1, \"hello world\")");
    REQUIRE(output.isSuccess());

    const auto& fDef = GET_FUNC_DEF(output, "f");
    auto callExpr    = prog::expr::callExprNode(
        output.getProg(),
        GET_FUNC_ID(
            output, "tuple__int_string", GET_TYPE_ID(output, "int"), GET_TYPE_ID(output, "string")),
        EXPRS(
            prog::expr::litIntNode(output.getProg(), 1),
            prog::expr::litStringNode(output.getProg(), "hello world")));

    CHECK(fDef.getBody() == *callExpr);
  }

  SECTION("Construct templated union") {
    const auto& output = ANALYZE("struct none "
                                 "union opt{T} = T, none "
                                 "fun f() opt{int}(1)");
    REQUIRE(output.isSuccess());

    const auto& fDef = GET_FUNC_DEF(output, "f");
    auto callExpr    = prog::expr::callExprNode(
        output.getProg(),
        GET_FUNC_ID(output, "opt__int", GET_TYPE_ID(output, "int")),
        EXPRS(prog::expr::litIntNode(output.getProg(), 1)));

    CHECK(fDef.getBody() == *callExpr);
  }

  SECTION("Construct templated struct with inferred type params") {
    const auto& output = ANALYZE("struct tuple{T1, T2} = T1 a, T2 b "
                                 "fun f() tuple(1, \"hello world\")");
    REQUIRE(output.isSuccess());

    const auto& fDef = GET_FUNC_DEF(output, "f");
    auto callExpr    = prog::expr::callExprNode(
        output.getProg(),
        GET_FUNC_ID(
            output, "tuple__int_string", GET_TYPE_ID(output, "int"), GET_TYPE_ID(output, "string")),
        EXPRS(
            prog::expr::litIntNode(output.getProg(), 1),
            prog::expr::litStringNode(output.getProg(), "hello world")));

    CHECK(fDef.getBody() == *callExpr);
  }

  SECTION("Construct templated union with inferred type param") {
    const auto& output = ANALYZE("struct none "
                                 "union opt{T} = T, none "
                                 "fun f() opt(1)");
    REQUIRE(output.isSuccess());

    const auto& fDef = GET_FUNC_DEF(output, "f");
    auto callExpr    = prog::expr::callExprNode(
        output.getProg(),
        GET_FUNC_ID(output, "opt__int", GET_TYPE_ID(output, "int")),
        EXPRS(prog::expr::litIntNode(output.getProg(), 1)));

    CHECK(fDef.getBody() == *callExpr);
  }

  SECTION("Conversion to templated struct") {
    const auto& output = ANALYZE("struct tuple{T1, T2} = T1 a, T2 b "
                                 "fun tuple{T}(T a) tuple{T, bool}(a, false) "
                                 "fun f() tuple{int}(1)");
    REQUIRE(output.isSuccess());

    const auto& fDef = GET_FUNC_DEF(output, "f");
    auto callExpr    = prog::expr::callExprNode(
        output.getProg(),
        GET_FUNC_ID(output, "tuple__int_bool", GET_TYPE_ID(output, "int")),
        EXPRS(prog::expr::litIntNode(output.getProg(), 1)));

    CHECK(fDef.getBody() == *callExpr);
  }

  SECTION("Templated conversion") {
    const auto& output =
        ANALYZE("fun string(int i) intrinsic{int_to_string}(i) "
                "fun string(float f) intrinsic{float_to_string}(f) "
                "struct tuple{T1, T2} = T1 a, T2 b "
                "fun string{T1, T2}(tuple{T1, T2} t) t.a.string() + \",\" + t.b.string() "
                "fun f(tuple{int, float} t) string{int, float}(t)");
    REQUIRE(output.isSuccess());

    const auto& fDef   = GET_FUNC_DEF(output, "f", GET_TYPE_ID(output, "tuple__int_float"));
    const auto& consts = fDef.getConsts();

    auto callExpr = prog::expr::callExprNode(
        output.getProg(),
        GET_FUNC_ID(output, "string", GET_TYPE_ID(output, "tuple__int_float")),
        EXPRS(prog::expr::constExprNode(consts, *consts.lookup("t"))));

    CHECK(fDef.getBody() == *callExpr);
  }
}

} // namespace frontend
