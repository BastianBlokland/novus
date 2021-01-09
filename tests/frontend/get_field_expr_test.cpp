#include "catch2/catch.hpp"
#include "frontend/diag_defs.hpp"
#include "helpers.hpp"
#include "prog/expr/node_const.hpp"
#include "prog/expr/node_field.hpp"
#include "prog/operator.hpp"

namespace frontend {

TEST_CASE("[frontend] Analyzing field expressions", "frontend") {

  SECTION("Get basic field expression") {
    const auto& output = ANALYZE("struct S = int a, bool b "
                                 "fun f(S s) s.a");
    REQUIRE(output.isSuccess());

    const auto& structDef  = std::get<prog::sym::StructDef>(GET_TYPE_DEF(output, "S"));
    const auto& funcDef    = GET_FUNC_DEF(output, "f", GET_TYPE_ID(output, "S"));
    const auto& funcConsts = funcDef.getConsts();

    CHECK(
        funcDef.getExpr() ==
        *prog::expr::fieldExprNode(
            output.getProg(),
            prog::expr::constExprNode(funcConsts, funcConsts.lookup("s").value()),
            *structDef.getFields().lookup("a")));
  }

  SECTION("Get nested field expression") {
    const auto& output = ANALYZE("struct S1 = S2 a "
                                 "struct S2 = int a "
                                 "fun f(S1 s) s.a.a");
    REQUIRE(output.isSuccess());

    const auto& struct1Def = std::get<prog::sym::StructDef>(GET_TYPE_DEF(output, "S1"));
    const auto& struct2Def = std::get<prog::sym::StructDef>(GET_TYPE_DEF(output, "S2"));
    const auto& funcDef    = GET_FUNC_DEF(output, "f", GET_TYPE_ID(output, "S1"));
    const auto& funcConsts = funcDef.getConsts();

    CHECK(
        funcDef.getExpr() ==
        *prog::expr::fieldExprNode(
            output.getProg(),
            prog::expr::fieldExprNode(
                output.getProg(),
                prog::expr::constExprNode(funcConsts, funcConsts.lookup("s").value()),
                *struct2Def.getFields().lookup("a")),
            *struct1Def.getFields().lookup("a")));
  }

  SECTION("Diagnostics") {
    CHECK_DIAG(
        "fun f(int i) -> int i.a", errFieldNotFoundOnType(src, "a", "int", input::Span{20, 22}));
    CHECK_DIAG(
        "struct S = int a "
        "fun f(S s) -> int s.b",
        errFieldNotFoundOnType(src, "b", "S", input::Span{35, 37}));
  }
}

} // namespace frontend
