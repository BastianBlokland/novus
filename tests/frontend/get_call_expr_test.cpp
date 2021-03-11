#include "catch2/catch.hpp"
#include "frontend/diag_defs.hpp"
#include "helpers.hpp"
#include "prog/expr/node_const.hpp"
#include "prog/expr/node_lit_bool.hpp"
#include "prog/expr/node_lit_int.hpp"
#include "prog/expr/node_lit_string.hpp"
#include "prog/expr/node_switch.hpp"

namespace frontend {

TEST_CASE("[frontend] Analyzing call expressions", "frontend") {

  SECTION("Get basic call") {
    const auto& output = ANALYZE("fun f1() -> int 1 "
                                 "fun f2() -> int f1()");
    REQUIRE(output.isSuccess());
    CHECK(
        GET_FUNC_DEF(output, "f2").getBody() ==
        *prog::expr::callExprNode(output.getProg(), GET_FUNC_ID(output, "f1"), {}));
  }

  SECTION("Get call with arg") {
    const auto& output = ANALYZE("fun f1(int a) -> int a + 1 "
                                 "fun f2() -> int f1(1)");
    REQUIRE(output.isSuccess());

    auto callExpr = prog::expr::callExprNode(
        output.getProg(),
        GET_FUNC_ID(output, "f1", GET_TYPE_ID(output, "int")),
        EXPRS(prog::expr::litIntNode(output.getProg(), 1)));

    CHECK(GET_FUNC_DEF(output, "f2").getBody() == *callExpr);
  }

  SECTION("Get recursive call") {
    const auto& output = ANALYZE("fun f() -> int false ? f() : 1");
    REQUIRE(output.isSuccess());

    auto switchExpr = prog::expr::switchExprNode(
        output.getProg(),
        EXPRS(prog::expr::litBoolNode(output.getProg(), false)),
        EXPRS(
            prog::expr::callExprNode(output.getProg(), GET_FUNC_ID(output, "f"), {}),
            prog::expr::litIntNode(output.getProg(), 1)));

    CHECK(GET_FUNC_DEF(output, "f").getBody() == *switchExpr);
  }

  SECTION("Get call with conversion") {
    const auto& output = ANALYZE("fun f1(float a) a "
                                 "fun f2() f1(1)");
    REQUIRE(output.isSuccess());

    auto callExpr = prog::expr::callExprNode(
        output.getProg(),
        GET_FUNC_ID(output, "f1", GET_TYPE_ID(output, "float")),
        EXPRS(applyConv(output, "int", "float", prog::expr::litIntNode(output.getProg(), 1))));

    CHECK(GET_FUNC_DEF(output, "f2").getBody() == *callExpr);
  }

  SECTION("Get templated call") {
    const auto& output = ANALYZE("fun f1{T}(T t) -> T t "
                                 "fun f2() -> int f1{int}(1)");
    REQUIRE(output.isSuccess());

    auto callExpr = prog::expr::callExprNode(
        output.getProg(),
        GET_FUNC_ID(output, "f1__int", GET_TYPE_ID(output, "int")),
        EXPRS(prog::expr::litIntNode(output.getProg(), 1)));

    CHECK(GET_FUNC_DEF(output, "f2").getBody() == *callExpr);
  }

  SECTION("Get forked call") {
    const auto& output = ANALYZE("fun f1() -> int 1 "
                                 "fun f2() -> future{int} fork f1()");
    REQUIRE(output.isSuccess());
    CHECK(
        GET_FUNC_DEF(output, "f2").getBody() ==
        *prog::expr::callExprNode(
            output.getProg(),
            GET_FUNC_ID(output, "f1"),
            GET_TYPE_ID(output, "__future_int"),
            {},
            prog::expr::CallMode::Forked));
  }

  SECTION("Get lazy call") {
    const auto& output = ANALYZE("fun f1() -> int 1 "
                                 "fun f2() -> lazy{int} lazy f1()");
    REQUIRE(output.isSuccess());
    CHECK(
        GET_FUNC_DEF(output, "f2").getBody() ==
        *prog::expr::callExprNode(
            output.getProg(),
            GET_FUNC_ID(output, "f1"),
            GET_TYPE_ID(output, "__lazy_int"),
            {},
            prog::expr::CallMode::Lazy));
  }

  SECTION("Get lazy call to overloaded call operator on literal") {
    const auto& output = ANALYZE("fun ()(int i) -> int i * i "
                                 "fun f() -> lazy{int} lazy 1()");
    REQUIRE(output.isSuccess());

    auto callExpr = prog::expr::callExprNode(
        output.getProg(),
        GET_FUNC_ID(output, "__op_parenparen", GET_TYPE_ID(output, "int")),
        GET_TYPE_ID(output, "__lazy_int"),
        EXPRS(prog::expr::litIntNode(output.getProg(), 1)),
        prog::expr::CallMode::Lazy);

    CHECK(GET_FUNC_DEF(output, "f").getBody() == *callExpr);
  }

  SECTION("Get call to overloaded call operator on literal") {
    const auto& output = ANALYZE("fun ()(int i) -> int i * i "
                                 "fun f() -> int 1()");
    REQUIRE(output.isSuccess());

    auto callExpr = prog::expr::callExprNode(
        output.getProg(),
        GET_FUNC_ID(output, "__op_parenparen", GET_TYPE_ID(output, "int")),
        EXPRS(prog::expr::litIntNode(output.getProg(), 1)));

    CHECK(GET_FUNC_DEF(output, "f").getBody() == *callExpr);
  }

  SECTION("Get call to overloaded call operator on const") {
    const auto& output = ANALYZE("fun ()(int i) -> int i * i "
                                 "fun f(int i) -> int i()");
    REQUIRE(output.isSuccess());

    const auto& fDef   = GET_FUNC_DEF(output, "f", GET_TYPE_ID(output, "int"));
    const auto& consts = fDef.getConsts();

    auto callExpr = prog::expr::callExprNode(
        output.getProg(),
        GET_FUNC_ID(output, "__op_parenparen", GET_TYPE_ID(output, "int")),
        EXPRS(prog::expr::constExprNode(consts, *consts.lookup("i"))));

    CHECK(fDef.getBody() == *callExpr);
  }

  SECTION("Get instance call") {
    const auto& output = ANALYZE("fun f1(int i) -> int i "
                                 "fun f2(int i) -> int i.f1()");
    REQUIRE(output.isSuccess());

    const auto& fDef   = GET_FUNC_DEF(output, "f2", GET_TYPE_ID(output, "int"));
    const auto& consts = fDef.getConsts();

    auto callExpr = prog::expr::callExprNode(
        output.getProg(),
        GET_FUNC_ID(output, "f1", GET_TYPE_ID(output, "int")),
        EXPRS(prog::expr::constExprNode(consts, *consts.lookup("i"))));

    CHECK(fDef.getBody() == *callExpr);
  }

  SECTION("Get instance call with args") {
    const auto& output = ANALYZE("fun string(int i) intrinsic{int_to_string}(i) "
                                 "fun f1(int i, string v) -> string i.string() + v "
                                 "fun f2(int i) -> string i.f1(\"test\")");
    REQUIRE(output.isSuccess());

    const auto& fDef   = GET_FUNC_DEF(output, "f2", GET_TYPE_ID(output, "int"));
    const auto& consts = fDef.getConsts();

    auto callExpr = prog::expr::callExprNode(
        output.getProg(),
        GET_FUNC_ID(output, "f1", GET_TYPE_ID(output, "int"), GET_TYPE_ID(output, "string")),
        EXPRS(
            prog::expr::constExprNode(consts, *consts.lookup("i")),
            prog::expr::litStringNode(output.getProg(), "test")));

    CHECK(fDef.getBody() == *callExpr);
  }

  SECTION("Get call to action") {
    const auto& output = ANALYZE("act a1() -> int 1 "
                                 "act a2() -> int a1()");
    REQUIRE(output.isSuccess());
    CHECK(
        GET_FUNC_DEF(output, "a2").getBody() ==
        *prog::expr::callExprNode(output.getProg(), GET_FUNC_ID(output, "a1"), {}));
  }

  SECTION("Get lazy call to action") {
    const auto& output = ANALYZE("act a1() -> int 1 "
                                 "act a2() -> lazy_action{int} lazy a1()");
    REQUIRE(output.isSuccess());
    CHECK(
        GET_FUNC_DEF(output, "a2").getBody() ==
        *prog::expr::callExprNode(
            output.getProg(),
            GET_FUNC_ID(output, "a1"),
            GET_TYPE_ID(output, "__lazy_action_int"),
            {},
            prog::expr::CallMode::Lazy));
  }

  SECTION("Lazy implicitly converts to lazy-action") {
    const auto& output = ANALYZE("fun f1() 1 "
                                 "fun a1(lazy_action{int} la) 1 "
                                 "fun f2() a1(lazy f1())");
    REQUIRE(output.isSuccess());

    auto callExpr = prog::expr::callExprNode(
        output.getProg(),
        GET_FUNC_ID(output, "a1", GET_TYPE_ID(output, "__lazy_action_int")),
        EXPRS(applyConv(
            output,
            "__lazy_int",
            "__lazy_action_int",
            prog::expr::callExprNode(
                output.getProg(),
                GET_FUNC_ID(output, "f1"),
                GET_TYPE_ID(output, "__lazy_int"),
                {},
                prog::expr::CallMode::Lazy))));

    CHECK(GET_FUNC_DEF(output, "f2").getBody() == *callExpr);
  }

  SECTION("Get call to templated action") {
    const auto& output = ANALYZE("act a1{T}(T t) -> T t "
                                 "act a2() -> int a1{int}(1)");
    REQUIRE(output.isSuccess());

    auto callExpr = prog::expr::callExprNode(
        output.getProg(),
        GET_FUNC_ID(output, "a1__int", GET_TYPE_ID(output, "int")),
        EXPRS(prog::expr::litIntNode(output.getProg(), 1)));

    CHECK(GET_FUNC_DEF(output, "a2").getBody() == *callExpr);
  }

  SECTION("Get lazy call to templated action") {
    const auto& output = ANALYZE("act a1{T}(T t) -> T t "
                                 "act a2() -> lazy_action{int} lazy a1{int}(1)");
    REQUIRE(output.isSuccess());

    auto callExpr = prog::expr::callExprNode(
        output.getProg(),
        GET_FUNC_ID(output, "a1__int", GET_TYPE_ID(output, "int")),
        EXPRS(prog::expr::litIntNode(output.getProg(), 1)));

    CHECK(GET_FUNC_DEF(output, "a2").getBody() == *callExpr);
  }

  SECTION("Diagnostics") {
    CHECK_DIAG(
        "fun f1() -> int 1 "
        "fun f2() -> int f3()",
        errUndeclaredPureFunc(NO_SRC, "f3", {}));
    CHECK_DIAG(
        "fun f1() -> int 1 "
        "fun f2() -> int f2(1)",
        errUndeclaredPureFunc(NO_SRC, "f2", {"int"}));
    CHECK_DIAG("fun f() -> int 1()", errUndeclaredCallOperator(NO_SRC, {"int"}));
    CHECK_DIAG("fun f(int i) -> int i()", errUndeclaredCallOperator(NO_SRC, {"int"}));
    CHECK_DIAG("fun f1(int i) -> int i.f2()", errFieldNotFoundOnType(NO_SRC, "f2", "int"));
    CHECK_DIAG(
        "act length(string str) -> int intrinsic{string_length}(str) "
        "fun f() -> int (42).length()",
        errUndeclaredPureFunc(NO_SRC, "length", {"int"}));
    CHECK_DIAG(
        "fun ()(string s) -> int intrinsic{string_length}(s) "
        "fun f() -> int 42()",
        errUndeclaredCallOperator(NO_SRC, {"int"}));
    CHECK_DIAG(
        "act a() -> int 1 "
        "fun f2() -> int a()",
        errUndeclaredPureFunc(NO_SRC, "a", {}));
    CHECK_DIAG(
        "act a{T}() -> T T() "
        "fun f2() -> int a{int}()",
        errNoPureFuncFoundToInstantiate(NO_SRC, "a", 1));
    CHECK_DIAG(
        "act a(int i) -> int i * 2 "
        "fun f2(int i) -> int i.a()",
        errUndeclaredPureFunc(NO_SRC, "a", {"int"}));
    CHECK_DIAG(
        "fun f(future{int} fi) -> int fork intrinsic{future_get}(fi)",
        errForkedNonUserFunc(NO_SRC));
    CHECK_DIAG(
        "fun f(future{int} fi) -> int lazy intrinsic{future_get}(fi)", errLazyNonUserFunc(NO_SRC));
  }
}

} // namespace frontend
