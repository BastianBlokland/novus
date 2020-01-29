#include "catch2/catch.hpp"
#include "frontend/diag_defs.hpp"
#include "helpers.hpp"
#include "prog/expr/node_call_dyn.hpp"
#include "prog/expr/node_const.hpp"
#include "prog/expr/node_field.hpp"
#include "prog/expr/node_lit_bool.hpp"
#include "prog/expr/node_lit_func.hpp"
#include "prog/expr/node_lit_int.hpp"

namespace frontend {

TEST_CASE("Analyzing call dynamic expressions", "[frontend]") {

  SECTION("Get delegate call without args") {
    const auto& output = ANALYZE("fun f1() -> int 1 "
                                 "fun f2(delegate{int} op) -> int op() "
                                 "fun f() -> int f2(f1)");
    REQUIRE(output.isSuccess());
    const auto& f2Def = GET_FUNC_DEF(output, "f2", GET_TYPE_ID(output, "__delegate_int"));
    const auto& fDef  = GET_FUNC_DEF(output, "f");

    CHECK(
        f2Def.getExpr() ==
        *prog::expr::callDynExprNode(
            output.getProg(),
            prog::expr::constExprNode(f2Def.getConsts(), *f2Def.getConsts().lookup("op")),
            {}));

    auto fArgs = std::vector<prog::expr::NodePtr>{};
    fArgs.push_back(prog::expr::litFuncNode(
        output.getProg(), GET_TYPE_ID(output, "__delegate_int"), GET_FUNC_ID(output, "f1")));
    CHECK(
        fDef.getExpr() ==
        *prog::expr::callExprNode(output.getProg(), f2Def.getId(), std::move(fArgs)));
  }

  SECTION("Get delegate call with args") {
    const auto& output = ANALYZE("fun f1(bool b, float v) -> int b ? 1 : 0 "
                                 "fun f2(delegate{bool, float, int} op) -> int op(false, 1) "
                                 "fun f() -> int f2(f1)");
    REQUIRE(output.isSuccess());
    const auto& f2Def =
        GET_FUNC_DEF(output, "f2", GET_TYPE_ID(output, "__delegate_bool_float_int"));
    const auto& fDef = GET_FUNC_DEF(output, "f");

    auto f2Args = std::vector<prog::expr::NodePtr>{};
    f2Args.push_back(prog::expr::litBoolNode(output.getProg(), false));
    f2Args.push_back(prog::expr::litIntNode(output.getProg(), 1));
    CHECK(
        f2Def.getExpr() ==
        *prog::expr::callDynExprNode(
            output.getProg(),
            prog::expr::constExprNode(f2Def.getConsts(), *f2Def.getConsts().lookup("op")),
            std::move(f2Args)));

    auto fArgs = std::vector<prog::expr::NodePtr>{};
    fArgs.push_back(prog::expr::litFuncNode(
        output.getProg(),
        GET_TYPE_ID(output, "__delegate_bool_float_int"),
        GET_FUNC_ID(output, "f1", GET_TYPE_ID(output, "bool"), GET_TYPE_ID(output, "float"))));
    CHECK(
        fDef.getExpr() ==
        *prog::expr::callExprNode(output.getProg(), f2Def.getId(), std::move(fArgs)));
  }

  SECTION("Get delegate call with templated function") {
    const auto& output = ANALYZE("fun f1{T}() -> T T() "
                                 "fun f2(delegate{int} op) -> int op() "
                                 "fun f() -> int f2(f1{int})");
    REQUIRE(output.isSuccess());
    const auto& f2Def = GET_FUNC_DEF(output, "f2", GET_TYPE_ID(output, "__delegate_int"));
    const auto& fDef  = GET_FUNC_DEF(output, "f");

    CHECK(
        f2Def.getExpr() ==
        *prog::expr::callDynExprNode(
            output.getProg(),
            prog::expr::constExprNode(f2Def.getConsts(), *f2Def.getConsts().lookup("op")),
            {}));

    auto fArgs = std::vector<prog::expr::NodePtr>{};
    fArgs.push_back(prog::expr::litFuncNode(
        output.getProg(), GET_TYPE_ID(output, "__delegate_int"), GET_FUNC_ID(output, "f1__int")));
    CHECK(
        fDef.getExpr() ==
        *prog::expr::callExprNode(output.getProg(), f2Def.getId(), std::move(fArgs)));
  }

  SECTION("Get delegate call on struct") {
    const auto& output = ANALYZE("struct S = delegate{int} del "
                                 "fun f(S s) -> int s.del()");
    REQUIRE(output.isSuccess());
    const auto& sDef = std::get<prog::sym::StructDef>(GET_TYPE_DEF(output, "S"));
    const auto& fDef = GET_FUNC_DEF(output, "f", GET_TYPE_ID(output, "S"));

    CHECK(
        fDef.getExpr() ==
        *prog::expr::callDynExprNode(
            output.getProg(),
            prog::expr::fieldExprNode(
                output.getProg(),
                prog::expr::constExprNode(fDef.getConsts(), *fDef.getConsts().lookup("s")),
                *sDef.getFields().lookup("del")),
            {}));
  }

  SECTION("Diagnostics") {
    CHECK_DIAG(
        "fun f(delegate{int, float, bool} op) -> bool op(false, 1.0)",
        errIncorrectArgsToDelegate(src, input::Span{45, 58}));
    CHECK_DIAG(
        "fun f1(int v) -> int v "
        "fun f1(float v) -> float v "
        "fun f2() -> int op = f1; op()",
        errAmbiguousFunction(src, "f1", input::Span{71, 72}),
        errUndeclaredPureFunc(src, "op", {}, input::Span{75, 78}));
    CHECK_DIAG(
        "fun f1{T}(int v) -> T T() "
        "fun f1{T}(float v) -> T T() "
        "fun f2() -> int op = f1{int}; op(1)",
        errAmbiguousTemplateFunction(src, "f1", 1, input::Span{75, 81}),
        errUndeclaredPureFunc(src, "op", {"int"}, input::Span{84, 88}));
    CHECK_DIAG("fun f() f1{float}", errNoFuncFoundToInstantiate(src, "f1", 1, input::Span{8, 16}));
    CHECK_DIAG(
        "fun f1{T}() -> T T() "
        "fun f2() -> int op = f1; op()",
        errNoTypeParamsProvidedToTemplateFunction(src, "f1", input::Span{42, 43}),
        errUndeclaredPureFunc(src, "op", {}, input::Span{46, 49}));
    CHECK_DIAG(
        "fun f() -> delegate{string, string} conWrite",
        errUndeclaredConst(src, "conWrite", input::Span{36, 43}));
    CHECK_DIAG(
        "fun f() -> string op = conWrite; op(\"hello world\")",
        errUndeclaredConst(src, "conWrite", input::Span{23, 30}),
        errUndeclaredPureFunc(src, "op", {"string"}, input::Span{33, 49}));
    CHECK_DIAG(
        "action main() conWrite(\"hello world\") "
        "action a() -> string op = main; op()",
        errUndeclaredConst(src, "main", input::Span{64, 67}),
        errUndeclaredFuncOrAction(src, "op", {}, input::Span{70, 73}));
    CHECK_DIAG(
        "action main{T}() conWrite(\"hello world\") "
        "action a() -> string op = main{int}; op()",
        errNoFuncFoundToInstantiate(src, "main", 1, input::Span{67, 75}),
        errUndeclaredFuncOrAction(src, "op", {}, input::Span{78, 81}));
  }
}

} // namespace frontend
