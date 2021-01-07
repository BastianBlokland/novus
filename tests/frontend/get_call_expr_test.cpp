#include "catch2/catch.hpp"
#include "frontend/diag_defs.hpp"
#include "helpers.hpp"
#include "prog/expr/node_const.hpp"
#include "prog/expr/node_lit_bool.hpp"
#include "prog/expr/node_lit_int.hpp"
#include "prog/expr/node_lit_string.hpp"
#include "prog/expr/node_switch.hpp"

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
    auto callExpr = prog::expr::callExprNode(
        output.getProg(), GET_FUNC_ID(output, "f1", GET_TYPE_ID(output, "int")), std::move(args));

    CHECK(GET_FUNC_DEF(output, "f2").getExpr() == *callExpr);
  }

  SECTION("Get recursive call") {
    const auto& output = ANALYZE("fun f() -> int false ? f() : 1");
    REQUIRE(output.isSuccess());

    auto conditions = std::vector<prog::expr::NodePtr>{};
    conditions.push_back(prog::expr::litBoolNode(output.getProg(), false));

    auto branches = std::vector<prog::expr::NodePtr>{};
    branches.push_back(prog::expr::callExprNode(output.getProg(), GET_FUNC_ID(output, "f"), {}));
    branches.push_back(prog::expr::litIntNode(output.getProg(), 1));
    auto switchExpr =
        prog::expr::switchExprNode(output.getProg(), std::move(conditions), std::move(branches));

    CHECK(GET_FUNC_DEF(output, "f").getExpr() == *switchExpr);
  }

  SECTION("Get call with conversion") {
    const auto& output = ANALYZE("fun f1(float a) a "
                                 "fun f2() f1(1)");
    REQUIRE(output.isSuccess());

    auto args = std::vector<prog::expr::NodePtr>{};
    args.push_back(applyConv(output, "int", "float", prog::expr::litIntNode(output.getProg(), 1)));
    auto callExpr = prog::expr::callExprNode(
        output.getProg(), GET_FUNC_ID(output, "f1", GET_TYPE_ID(output, "float")), std::move(args));

    CHECK(GET_FUNC_DEF(output, "f2").getExpr() == *callExpr);
  }

  SECTION("Get templated call") {
    const auto& output = ANALYZE("fun f1{T}(T t) -> T t "
                                 "fun f2() -> int f1{int}(1)");
    REQUIRE(output.isSuccess());

    auto args = std::vector<prog::expr::NodePtr>{};
    args.push_back(prog::expr::litIntNode(output.getProg(), 1));
    auto callExpr = prog::expr::callExprNode(
        output.getProg(),
        GET_FUNC_ID(output, "f1__int", GET_TYPE_ID(output, "int")),
        std::move(args));

    CHECK(GET_FUNC_DEF(output, "f2").getExpr() == *callExpr);
  }

  SECTION("Get forked call") {
    const auto& output = ANALYZE("fun f1() -> int 1 "
                                 "fun f2() -> future{int} fork f1()");
    REQUIRE(output.isSuccess());
    CHECK(
        GET_FUNC_DEF(output, "f2").getExpr() ==
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
        GET_FUNC_DEF(output, "f2").getExpr() ==
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

    auto args = std::vector<prog::expr::NodePtr>{};
    args.push_back(prog::expr::litIntNode(output.getProg(), 1));
    auto callExpr = prog::expr::callExprNode(
        output.getProg(),
        GET_FUNC_ID(output, "__op_parenparen", GET_TYPE_ID(output, "int")),
        GET_TYPE_ID(output, "__lazy_int"),
        std::move(args),
        prog::expr::CallMode::Lazy);

    CHECK(GET_FUNC_DEF(output, "f").getExpr() == *callExpr);
  }

  SECTION("Get call to overloaded call operator on literal") {
    const auto& output = ANALYZE("fun ()(int i) -> int i * i "
                                 "fun f() -> int 1()");
    REQUIRE(output.isSuccess());

    auto args = std::vector<prog::expr::NodePtr>{};
    args.push_back(prog::expr::litIntNode(output.getProg(), 1));
    auto callExpr = prog::expr::callExprNode(
        output.getProg(),
        GET_FUNC_ID(output, "__op_parenparen", GET_TYPE_ID(output, "int")),
        std::move(args));

    CHECK(GET_FUNC_DEF(output, "f").getExpr() == *callExpr);
  }

  SECTION("Get call to overloaded call operator on const") {
    const auto& output = ANALYZE("fun ()(int i) -> int i * i "
                                 "fun f(int i) -> int i()");
    REQUIRE(output.isSuccess());

    const auto& fDef   = GET_FUNC_DEF(output, "f", GET_TYPE_ID(output, "int"));
    const auto& consts = fDef.getConsts();

    auto args = std::vector<prog::expr::NodePtr>{};
    args.push_back(prog::expr::constExprNode(consts, *consts.lookup("i")));
    auto callExpr = prog::expr::callExprNode(
        output.getProg(),
        GET_FUNC_ID(output, "__op_parenparen", GET_TYPE_ID(output, "int")),
        std::move(args));

    CHECK(fDef.getExpr() == *callExpr);
  }

  SECTION("Get instance call") {
    const auto& output = ANALYZE("fun f1(int i) -> int i "
                                 "fun f2(int i) -> int i.f1()");
    REQUIRE(output.isSuccess());

    const auto& fDef   = GET_FUNC_DEF(output, "f2", GET_TYPE_ID(output, "int"));
    const auto& consts = fDef.getConsts();

    auto args = std::vector<prog::expr::NodePtr>{};
    args.push_back(prog::expr::constExprNode(consts, *consts.lookup("i")));
    auto callExpr = prog::expr::callExprNode(
        output.getProg(), GET_FUNC_ID(output, "f1", GET_TYPE_ID(output, "int")), std::move(args));

    CHECK(fDef.getExpr() == *callExpr);
  }

  SECTION("Get instance call with args") {
    const auto& output = ANALYZE("fun f1(int i, string v) -> string i.string() + v "
                                 "fun f2(int i) -> string i.f1(\"test\")");
    REQUIRE(output.isSuccess());

    const auto& fDef   = GET_FUNC_DEF(output, "f2", GET_TYPE_ID(output, "int"));
    const auto& consts = fDef.getConsts();

    auto args = std::vector<prog::expr::NodePtr>{};
    args.push_back(prog::expr::constExprNode(consts, *consts.lookup("i")));
    args.push_back(prog::expr::litStringNode(output.getProg(), "test"));
    auto callExpr = prog::expr::callExprNode(
        output.getProg(),
        GET_FUNC_ID(output, "f1", GET_TYPE_ID(output, "int"), GET_TYPE_ID(output, "string")),
        std::move(args));

    CHECK(fDef.getExpr() == *callExpr);
  }

  SECTION("Get call to action") {
    const auto& output = ANALYZE("act a1() -> int 1 "
                                 "act a2() -> int a1()");
    REQUIRE(output.isSuccess());
    CHECK(
        GET_FUNC_DEF(output, "a2").getExpr() ==
        *prog::expr::callExprNode(output.getProg(), GET_FUNC_ID(output, "a1"), {}));
  }

  SECTION("Get call to templated action") {
    const auto& output = ANALYZE("fun a1{T}(T t) -> T t "
                                 "fun a2() -> int a1{int}(1)");
    REQUIRE(output.isSuccess());

    auto args = std::vector<prog::expr::NodePtr>{};
    args.push_back(prog::expr::litIntNode(output.getProg(), 1));
    auto callExpr = prog::expr::callExprNode(
        output.getProg(),
        GET_FUNC_ID(output, "a1__int", GET_TYPE_ID(output, "int")),
        std::move(args));

    CHECK(GET_FUNC_DEF(output, "a2").getExpr() == *callExpr);
  }

  SECTION("Get fail action call") {
    const auto& output = ANALYZE("act a() -> int failfast{int}()");
    REQUIRE(output.isSuccess());

    auto callExpr = prog::expr::callExprNode(
        output.getProg(),
        GET_FUNC_ID(output, "__failfast_int"),
        std::vector<prog::expr::NodePtr>{});

    CHECK(GET_FUNC_DEF(output, "a").getExpr() == *callExpr);
  }

  SECTION("Diagnostics") {
    CHECK_DIAG(
        "fun f1() -> int 1 "
        "fun f2() -> int f3()",
        errUndeclaredPureFunc(src, "f3", {}, input::Span{34, 37}));
    CHECK_DIAG(
        "fun f1() -> int 1 "
        "fun f2() -> int f2(1)",
        errUndeclaredPureFunc(src, "f2", {"int"}, input::Span{34, 38}));
    CHECK_DIAG("fun f() -> int 1()", errUndeclaredCallOperator(src, {"int"}, input::Span{15, 17}));
    CHECK_DIAG(
        "fun f(int i) -> int i()", errUndeclaredCallOperator(src, {"int"}, input::Span{20, 22}));
    CHECK_DIAG(
        "fun f1(int i) -> int i.f2()",
        errFieldNotFoundOnType(src, "f2", "int", input::Span{21, 24}));
    CHECK_DIAG(
        "act length(string str) -> int intrinsic{string_length}(str) "
        "fun f() -> int (42).length()",
        errUndeclaredPureFunc(src, "length", {"int"}, input::Span{75, 87}));
    CHECK_DIAG(
        "fun ()(string s) -> int intrinsic{string_length}(s) "
        "fun f() -> int 42()",
        errUndeclaredCallOperator(src, {"int"}, input::Span{67, 70}));
    CHECK_DIAG(
        "act a() -> int 1 "
        "fun f2() -> int a()",
        errUndeclaredPureFunc(src, "a", {}, input::Span{33, 35}));
    CHECK_DIAG(
        "act a{T}() -> T T() "
        "fun f2() -> int a{int}()",
        errNoPureFuncFoundToInstantiate(src, "a", 1, input::Span{36, 43}));
    CHECK_DIAG(
        "act a(int i) -> int i * 2 "
        "fun f2(int i) -> int i.a()",
        errUndeclaredPureFunc(src, "a", {"int"}, input::Span{47, 51}));
    CHECK_DIAG(
        "fun f(future{int} fi) -> int fork fi.get()",
        errForkedNonUserFunc(src, input::Span{29, 41}));
    CHECK_DIAG(
        "fun f(future{int} fi) -> int lazy fi.get()", errLazyNonUserFunc(src, input::Span{29, 41}));
    CHECK_DIAG(
        "act a(int i) -> int i * 2 "
        "act f2(int i) -> int lazy i.a()",
        errLazyActionCall(src, input::Span{47, 56}));
    CHECK_DIAG(
        "fun f() -> int failfast{int}()",
        errNoPureFuncFoundToInstantiate(src, "failfast", 1, input::Span{15, 29}));
    CHECK_DIAG(
        "act f() -> int failfast()",
        errInvalidFailCall(src, 0, 0, input::Span{15, 24}),
        errUndeclaredFuncOrAction(src, "failfast", {}, input::Span{15, 24}));
    CHECK_DIAG(
        "act f() -> int failfast(1)",
        errInvalidFailCall(src, 0, 1, input::Span{15, 25}),
        errUndeclaredFuncOrAction(src, "failfast", {"int"}, input::Span{15, 25}));
    CHECK_DIAG(
        "act f() -> int failfast{int}(1.0)",
        errInvalidFailCall(src, 1, 1, input::Span{15, 32}),
        errNoFuncOrActionFoundToInstantiate(src, "failfast", 1, input::Span{15, 32}));
  }
}

} // namespace frontend
