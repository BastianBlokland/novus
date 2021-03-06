#include "catch2/catch.hpp"
#include "frontend/diag_defs.hpp"
#include "helpers.hpp"
#include "prog/expr/node_call_dyn.hpp"
#include "prog/expr/node_closure.hpp"
#include "prog/expr/node_const.hpp"
#include "prog/expr/node_lit_func.hpp"
#include "prog/expr/node_lit_int.hpp"

namespace frontend {

TEST_CASE("[frontend] Analyzing anonymous functions", "frontend") {

  SECTION("Return anonymous function") {
    const auto& output = ANALYZE("fun f() -> function{int} "
                                 "  lambda () 42");
    REQUIRE(output.isSuccess());

    CHECK(findAnonFuncDef(output, 0).getBody() == *prog::expr::litIntNode(output.getProg(), 42));
    CHECK(
        GET_FUNC_DEF(output, "f").getBody() ==
        *prog::expr::litFuncNode(
            output.getProg(), GET_TYPE_ID(output, "__function_int"), findAnonFunc(output, 0)));
  }

  SECTION("Return anonymous action") {
    const auto& output = ANALYZE("act f() -> action{int} "
                                 "  impure lambda () 42");
    REQUIRE(output.isSuccess());

    CHECK(findAnonFuncDef(output, 0).getBody() == *prog::expr::litIntNode(output.getProg(), 42));
    CHECK(
        GET_FUNC_DEF(output, "f").getBody() ==
        *prog::expr::litFuncNode(
            output.getProg(), GET_TYPE_ID(output, "__action_int"), findAnonFunc(output, 0)));
  }

  SECTION("Return anonymous function from action") {
    const auto& output = ANALYZE("act f() -> function{int} "
                                 "  lambda () 42");
    REQUIRE(output.isSuccess());

    CHECK(findAnonFuncDef(output, 0).getBody() == *prog::expr::litIntNode(output.getProg(), 42));
    CHECK(
        GET_FUNC_DEF(output, "f").getBody() ==
        *prog::expr::litFuncNode(
            output.getProg(), GET_TYPE_ID(output, "__function_int"), findAnonFunc(output, 0)));
  }

  SECTION("Invoke anonymous function") {
    const auto& output = ANALYZE("fun f() -> int "
                                 "  (lambda () 1)()");
    REQUIRE(output.isSuccess());

    CHECK(findAnonFuncDef(output, 0).getBody() == *prog::expr::litIntNode(output.getProg(), 1));
    CHECK(
        GET_FUNC_DEF(output, "f").getBody() ==
        *prog::expr::callDynExprNode(
            output.getProg(),
            prog::expr::litFuncNode(
                output.getProg(), GET_TYPE_ID(output, "__function_int"), findAnonFunc(output, 0)),
            {}));
  }

  SECTION("Invoke anonymous action") {
    const auto& output = ANALYZE("act f() -> int "
                                 "  (impure lambda () 1)()");
    REQUIRE(output.isSuccess());

    CHECK(findAnonFuncDef(output, 0).getBody() == *prog::expr::litIntNode(output.getProg(), 1));
    CHECK(
        GET_FUNC_DEF(output, "f").getBody() ==
        *prog::expr::callDynExprNode(
            output.getProg(),
            prog::expr::litFuncNode(
                output.getProg(), GET_TYPE_ID(output, "__action_int"), findAnonFunc(output, 0)),
            {}));
  }

  SECTION("Invoke anonymous function from action") {
    const auto& output = ANALYZE("act f() -> int "
                                 "  (lambda () 1)()");
    REQUIRE(output.isSuccess());

    CHECK(findAnonFuncDef(output, 0).getBody() == *prog::expr::litIntNode(output.getProg(), 1));
    CHECK(
        GET_FUNC_DEF(output, "f").getBody() ==
        *prog::expr::callDynExprNode(
            output.getProg(),
            prog::expr::litFuncNode(
                output.getProg(), GET_TYPE_ID(output, "__function_int"), findAnonFunc(output, 0)),
            {}));
  }

  SECTION("Invoke anonymous function with argument") {
    const auto& output = ANALYZE("fun f() -> int "
                                 "  (lambda (int i) i)(1)");
    REQUIRE(output.isSuccess());

    // Check the anonymous function.
    const auto& anonDef = findAnonFuncDef(output, 0);
    CHECK(
        anonDef.getBody() ==
        *prog::expr::constExprNode(anonDef.getConsts(), *anonDef.getConsts().lookup("i")));

    // Check the 'f' function.
    CHECK(
        GET_FUNC_DEF(output, "f").getBody() ==
        *prog::expr::callDynExprNode(
            output.getProg(),
            prog::expr::litFuncNode(
                output.getProg(),
                GET_TYPE_ID(output, "__function_int_int"),
                findAnonFunc(output, 0)),
            EXPRS(prog::expr::litIntNode(output.getProg(), 1))));
  }

  SECTION("Invoke anonymous function with closure") {
    const auto& output = ANALYZE("fun f(float v) -> float "
                                 "  (lambda (int i) v)(1)");
    REQUIRE(output.isSuccess());

    // Check the anonymous function.
    const auto& anonDef = findAnonFuncDef(output, 0);
    CHECK(
        anonDef.getBody() ==
        *prog::expr::constExprNode(anonDef.getConsts(), *anonDef.getConsts().lookup("__bound_1")));

    // Check the 'f' function.
    const auto& fDef = GET_FUNC_DEF(output, "f", GET_TYPE_ID(output, "float"));
    CHECK(
        fDef.getBody() ==
        *prog::expr::callDynExprNode(
            output.getProg(),
            prog::expr::closureNode(
                output.getProg(),
                GET_TYPE_ID(output, "__function_int_float"),
                findAnonFunc(output, 0),
                EXPRS(prog::expr::constExprNode(fDef.getConsts(), *fDef.getConsts().lookup("v")))),
            EXPRS(prog::expr::litIntNode(output.getProg(), 1))));
  }

  SECTION("Invoke anonymous function with nested closure") {
    const auto& output = ANALYZE("fun f(float v) -> function{float} "
                                 "  (lambda () (lambda () v))()");
    REQUIRE(output.isSuccess());

    // Check the most nested anonymous function.
    const auto& anon0Def = findAnonFuncDef(output, 0);
    CHECK(
        anon0Def.getBody() ==
        *prog::expr::constExprNode(
            anon0Def.getConsts(), *anon0Def.getConsts().lookup("__bound_0")));

    // Check the other anonymous function.
    const auto& anon1Def = findAnonFuncDef(output, 1);
    CHECK(
        anon1Def.getBody() ==
        *prog::expr::closureNode(
            output.getProg(),
            GET_TYPE_ID(output, "__function_float"),
            findAnonFunc(output, 0),
            EXPRS(prog::expr::constExprNode(
                anon1Def.getConsts(), *anon1Def.getConsts().lookup("__bound_0")))));

    // Check the 'f' function.
    const auto& fDef = GET_FUNC_DEF(output, "f", GET_TYPE_ID(output, "float"));
    CHECK(
        fDef.getBody() ==
        *prog::expr::callDynExprNode(
            output.getProg(),
            prog::expr::closureNode(
                output.getProg(),
                GET_TYPE_ID(output, "__function___function_float"),
                findAnonFunc(output, 1),
                EXPRS(prog::expr::constExprNode(fDef.getConsts(), *fDef.getConsts().lookup("v")))),
            {}));
  }

  SECTION("Return templated anonymous function") {
    const auto& output = ANALYZE("fun int() 0 "
                                 "fun float() 0.0 "
                                 "fun f1{T}() -> function{T} "
                                 "  lambda () T() "
                                 "fun f2() f1{int}() "
                                 "fun f3() f1{float}() ");
    REQUIRE(output.isSuccess());

    CHECK(
        findAnonFuncDef(output, 0).getBody() ==
        *prog::expr::callExprNode(output.getProg(), GET_FUNC_ID(output, "int"), {}));
    CHECK(
        findAnonFuncDef(output, 1).getBody() ==
        *prog::expr::callExprNode(output.getProg(), GET_FUNC_ID(output, "float"), {}));

    CHECK(
        GET_FUNC_DEF(output, "f1__int").getBody() ==
        *prog::expr::litFuncNode(
            output.getProg(), GET_TYPE_ID(output, "__function_int"), findAnonFunc(output, 0)));
    CHECK(
        GET_FUNC_DEF(output, "f1__float").getBody() ==
        *prog::expr::litFuncNode(
            output.getProg(), GET_TYPE_ID(output, "__function_float"), findAnonFunc(output, 1)));
  }

  SECTION("Anonymous function with conversion") {
    const auto& output = ANALYZE("fun implicit float(int i) intrinsic{int_to_float}(i) "
                                 "fun f() lambda () -> float 2");
    REQUIRE(output.isSuccess());
    const auto& anonFuncDef = findAnonFuncDef(output, 0);

    CHECK(
        anonFuncDef.getBody() ==
        *applyConv(output, "int", "float", prog::expr::litIntNode(output.getProg(), 2)));
  }

  SECTION("Diagnostics") {
    CHECK_DIAG("fun f() lambda (b c) 1", errUndeclaredType(NO_SRC, "b", 0));
    CHECK_DIAG("fun f() lambda (int a) -> b 1", errUndeclaredType(NO_SRC, "b", 0));
    CHECK_DIAG("fun f() lambda (int int) 1", errConstNameConflictsWithType(NO_SRC, "int"));
    CHECK_DIAG("fun f() lambda (int i, int i) 1", errConstNameConflictsWithConst(NO_SRC, "i"));
    CHECK_DIAG(
        "fun f{T}() lambda (int T) 1 "
        "fun f() f{int}()",
        errConstNameConflictsWithTypeSubstitution(NO_SRC, "T"),
        errInvalidFuncInstantiation(NO_SRC),
        errNoPureFuncFoundToInstantiate(NO_SRC, "f", {"int"}));
    CHECK_DIAG("fun f() lambda () b", errUndeclaredConst(NO_SRC, "b"));
    CHECK_DIAG("fun f() lambda () false ? i = 1 : i ", errUninitializedConst(NO_SRC, "i"));
    CHECK_DIAG("fun f() false ? i = 1 : (lambda () i)() ", errUninitializedConst(NO_SRC, "i"));
    CHECK_DIAG(
        "fun f() lambda (int a) -> bool a", errNoImplicitConversionFound(NO_SRC, "int", "bool"));
    CHECK_DIAG(
        "act a1() -> int 42 "
        "act a2() lambda () a1()",
        errUndeclaredPureFunc(NO_SRC, "a1", {}));
    CHECK_DIAG("fun f() lambda (int i = 0) i", errUnsupportedArgInitializer(NO_SRC, "i"));
  }
}

} // namespace frontend
