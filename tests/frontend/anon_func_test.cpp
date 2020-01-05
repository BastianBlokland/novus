#include "catch2/catch.hpp"
#include "frontend/diag_defs.hpp"
#include "helpers.hpp"
#include "prog/expr/node_call_dyn.hpp"
#include "prog/expr/node_closure.hpp"
#include "prog/expr/node_const.hpp"
#include "prog/expr/node_lit_func.hpp"
#include "prog/expr/node_lit_int.hpp"

namespace frontend {

TEST_CASE("Analyzing anonymous functions", "[frontend]") {

  SECTION("Return anonymous function") {
    const auto& output = ANALYZE("fun f() -> delegate{int} "
                                 "  lambda () 42");
    REQUIRE(output.isSuccess());

    CHECK(
        GET_FUNC_DEF(output, "__anon_0").getExpr() ==
        *prog::expr::litIntNode(output.getProg(), 42));
    CHECK(
        GET_FUNC_DEF(output, "f").getExpr() ==
        *prog::expr::litFuncNode(
            output.getProg(),
            GET_TYPE_ID(output, "__delegate_int"),
            GET_FUNC_ID(output, "__anon_0")));
  }

  SECTION("Invoke anonymous function") {
    const auto& output = ANALYZE("fun f() -> int "
                                 "  (lambda () 1)()");
    REQUIRE(output.isSuccess());

    CHECK(
        GET_FUNC_DEF(output, "__anon_0").getExpr() == *prog::expr::litIntNode(output.getProg(), 1));
    CHECK(
        GET_FUNC_DEF(output, "f").getExpr() ==
        *prog::expr::callDynExprNode(
            output.getProg(),
            prog::expr::litFuncNode(
                output.getProg(),
                GET_TYPE_ID(output, "__delegate_int"),
                GET_FUNC_ID(output, "__anon_0")),
            {}));
  }

  SECTION("Invoke anonymous function with argument") {
    const auto& output = ANALYZE("fun f() -> int "
                                 "  (lambda (int i) i)(1)");
    REQUIRE(output.isSuccess());

    // Check the anonymous function.
    const auto& anonDef = GET_FUNC_DEF(output, "__anon_0", GET_TYPE_ID(output, "int"));
    CHECK(
        anonDef.getExpr() ==
        *prog::expr::constExprNode(anonDef.getConsts(), *anonDef.getConsts().lookup("i")));

    // Check the 'f' function.
    auto args = std::vector<prog::expr::NodePtr>{};
    args.push_back(prog::expr::litIntNode(output.getProg(), 1));
    CHECK(
        GET_FUNC_DEF(output, "f").getExpr() ==
        *prog::expr::callDynExprNode(
            output.getProg(),
            prog::expr::litFuncNode(
                output.getProg(),
                GET_TYPE_ID(output, "__delegate_int_int"),
                GET_FUNC_ID(output, "__anon_0", GET_TYPE_ID(output, "int"))),
            std::move(args)));
  }

  SECTION("Invoke anonymous function with closure") {
    const auto& output = ANALYZE("fun f(float v) -> float "
                                 "  (lambda (int i) v)(1)");
    REQUIRE(output.isSuccess());

    // Check the anonymous function.
    const auto& anonDef =
        GET_FUNC_DEF(output, "__anon_0", GET_TYPE_ID(output, "int"), GET_TYPE_ID(output, "float"));
    CHECK(
        anonDef.getExpr() ==
        *prog::expr::constExprNode(anonDef.getConsts(), *anonDef.getConsts().lookup("__bound_1")));

    // Check the 'f' function.
    const auto& fDef = GET_FUNC_DEF(output, "f", GET_TYPE_ID(output, "float"));
    auto closureArgs = std::vector<prog::expr::NodePtr>{};
    closureArgs.push_back(
        prog::expr::constExprNode(fDef.getConsts(), *fDef.getConsts().lookup("v")));

    auto delArgs = std::vector<prog::expr::NodePtr>{};
    delArgs.push_back(prog::expr::litIntNode(output.getProg(), 1));
    CHECK(
        fDef.getExpr() ==
        *prog::expr::callDynExprNode(
            output.getProg(),
            prog::expr::closureNode(
                output.getProg(),
                GET_TYPE_ID(output, "__delegate_int_float"),
                GET_FUNC_ID(
                    output, "__anon_0", GET_TYPE_ID(output, "int"), GET_TYPE_ID(output, "float")),
                std::move(closureArgs)),
            std::move(delArgs)));
  }

  SECTION("Invoke anonymous function with nested closure") {
    const auto& output = ANALYZE("fun f(float v) -> delegate{float} "
                                 "  (lambda () (lambda () v))()");
    REQUIRE(output.isSuccess());

    // Check the most nested anonymous function.
    const auto& anon0Def = GET_FUNC_DEF(output, "__anon_0", GET_TYPE_ID(output, "float"));
    CHECK(
        anon0Def.getExpr() ==
        *prog::expr::constExprNode(
            anon0Def.getConsts(), *anon0Def.getConsts().lookup("__bound_0")));

    // Check the other anonymous function.
    const auto& anon1Def     = GET_FUNC_DEF(output, "__anon_1", GET_TYPE_ID(output, "float"));
    auto anon1DefClosureArgs = std::vector<prog::expr::NodePtr>{};
    anon1DefClosureArgs.push_back(
        prog::expr::constExprNode(anon1Def.getConsts(), *anon1Def.getConsts().lookup("__bound_0")));
    CHECK(
        anon1Def.getExpr() ==
        *prog::expr::closureNode(
            output.getProg(),
            GET_TYPE_ID(output, "__delegate_float"),
            GET_FUNC_ID(output, "__anon_0", GET_TYPE_ID(output, "float")),
            std::move(anon1DefClosureArgs)));

    // Check the 'f' function.
    const auto& fDef  = GET_FUNC_DEF(output, "f", GET_TYPE_ID(output, "float"));
    auto fClosureArgs = std::vector<prog::expr::NodePtr>{};
    fClosureArgs.push_back(
        prog::expr::constExprNode(fDef.getConsts(), *fDef.getConsts().lookup("v")));

    CHECK(
        fDef.getExpr() ==
        *prog::expr::callDynExprNode(
            output.getProg(),
            prog::expr::closureNode(
                output.getProg(),
                GET_TYPE_ID(output, "__delegate___delegate_float"),
                GET_FUNC_ID(output, "__anon_1", GET_TYPE_ID(output, "float")),
                std::move(fClosureArgs)),
            {}));
  }

  SECTION("Return templated anonymous function") {
    const auto& output = ANALYZE("fun f1{T}() -> delegate{T} "
                                 "  lambda () T() "
                                 "fun f2() f1{int}() "
                                 "fun f3() f1{float}() ");
    REQUIRE(output.isSuccess());

    CHECK(
        GET_FUNC_DEF(output, "__anon_0").getExpr() ==
        *prog::expr::callExprNode(
            output.getProg(), *output.getProg().lookupFunc("int", prog::sym::TypeSet({}), -1), {}));
    CHECK(
        GET_FUNC_DEF(output, "__anon_1").getExpr() ==
        *prog::expr::callExprNode(
            output.getProg(),
            *output.getProg().lookupFunc("float", prog::sym::TypeSet({}), -1),
            {}));

    CHECK(
        GET_FUNC_DEF(output, "f1__int").getExpr() ==
        *prog::expr::litFuncNode(
            output.getProg(),
            GET_TYPE_ID(output, "__delegate_int"),
            GET_FUNC_ID(output, "__anon_0")));
    CHECK(
        GET_FUNC_DEF(output, "f1__float").getExpr() ==
        *prog::expr::litFuncNode(
            output.getProg(),
            GET_TYPE_ID(output, "__delegate_float"),
            GET_FUNC_ID(output, "__anon_1")));
  }

  SECTION("Diagnostics") {
    CHECK_DIAG("fun f() lambda (b c) 1", errUndeclaredType(src, "b", input::Span{16, 16}));
    CHECK_DIAG(
        "fun f() lambda (int f) 1",
        errConstNameConflictsWithFunction(src, "f", input::Span{20, 20}));
    CHECK_DIAG(
        "fun f() lambda (int int) 1",
        errConstNameConflictsWithType(src, "int", input::Span{20, 22}));
    CHECK_DIAG(
        "fun f() lambda (int print) 1",
        errConstNameConflictsWithAction(src, "print", input::Span{20, 24}));
    CHECK_DIAG(
        "fun f() lambda (int i, int i) 1",
        errConstNameConflictsWithConst(src, "i", input::Span{27, 27}));
    CHECK_DIAG(
        "fun f{T}() lambda (int T) 1 "
        "fun f() f{int}()",
        errConstNameConflictsWithTypeSubstitution(src, "T", input::Span{23, 23}),
        errInvalidFuncInstantiation(src, input::Span{36, 36}),
        errNoFuncFoundToInstantiate(src, "f", 1, input::Span{36, 43}));
    CHECK_DIAG("fun f() lambda () b", errUndeclaredConst(src, "b", input::Span{18, 18}));
    CHECK_DIAG(
        "fun f() lambda () false ? i = 1 : i ",
        errUninitializedConst(src, "i", input::Span{34, 34}));
    CHECK_DIAG(
        "fun f() false ? i = 1 : (lambda () i)() ",
        errUninitializedConst(src, "i", input::Span{35, 35}));
  }
}

} // namespace frontend