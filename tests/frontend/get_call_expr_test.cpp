#include "catch2/catch.hpp"
#include "frontend/diag_defs.hpp"
#include "helpers.hpp"
#include "prog/expr/node_const.hpp"
#include "prog/expr/node_lit_int.hpp"
#include "prog/expr/node_lit_string.hpp"

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

  SECTION("Get call to overloaded call operator on literal") {
    const auto& output = ANALYZE("fun ()(int i) -> int i * i "
                                 "fun f() -> int 1()");
    REQUIRE(output.isSuccess());

    auto args = std::vector<prog::expr::NodePtr>{};
    args.push_back(prog::expr::litIntNode(output.getProg(), 1));
    CHECK(
        GET_FUNC_DEF(output, "f").getExpr() ==
        *prog::expr::callExprNode(
            output.getProg(),
            GET_FUNC_ID(output, "__op_parenparen", GET_TYPE_ID(output, "int")),
            std::move(args)));
  }

  SECTION("Get call to overloaded call operator on const") {
    const auto& output = ANALYZE("fun ()(int i) -> int i * i "
                                 "fun f(int i) -> int i()");
    REQUIRE(output.isSuccess());

    const auto& fDef   = GET_FUNC_DEF(output, "f", GET_TYPE_ID(output, "int"));
    const auto& consts = fDef.getConsts();

    auto args = std::vector<prog::expr::NodePtr>{};
    args.push_back(prog::expr::constExprNode(consts, *consts.lookup("i")));
    CHECK(
        fDef.getExpr() ==
        *prog::expr::callExprNode(
            output.getProg(),
            GET_FUNC_ID(output, "__op_parenparen", GET_TYPE_ID(output, "int")),
            std::move(args)));
  }

  SECTION("Get instance call") {
    const auto& output = ANALYZE("fun f1(int i) -> int i "
                                 "fun f2(int i) -> int i.f1()");
    REQUIRE(output.isSuccess());

    const auto& fDef   = GET_FUNC_DEF(output, "f2", GET_TYPE_ID(output, "int"));
    const auto& consts = fDef.getConsts();

    auto args = std::vector<prog::expr::NodePtr>{};
    args.push_back(prog::expr::constExprNode(consts, *consts.lookup("i")));
    CHECK(
        fDef.getExpr() ==
        *prog::expr::callExprNode(
            output.getProg(),
            GET_FUNC_ID(output, "f1", GET_TYPE_ID(output, "int")),
            std::move(args)));
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
    CHECK(
        fDef.getExpr() ==
        *prog::expr::callExprNode(
            output.getProg(),
            GET_FUNC_ID(output, "f1", GET_TYPE_ID(output, "int"), GET_TYPE_ID(output, "string")),
            std::move(args)));
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
        "fun f() -> int (42).length()",
        errUndeclaredPureFunc(src, "length", {"int"}, input::Span{15, 27}));
    CHECK_DIAG(
        "fun ()(string s) -> int s.length() "
        "fun f() -> int 42()",
        errUndeclaredCallOperator(src, {"int"}, input::Span{50, 53}));
  }
}

} // namespace frontend
