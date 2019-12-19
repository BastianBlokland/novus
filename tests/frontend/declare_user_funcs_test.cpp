#include "catch2/catch.hpp"
#include "frontend/diag_defs.hpp"
#include "helpers.hpp"
#include "prog/expr/node_call.hpp"
#include "prog/expr/node_lit_bool.hpp"
#include "prog/operator.hpp"

namespace frontend {

TEST_CASE("Analyzing user-function declarations", "[frontend]") {

  SECTION("Declare basic function") {
    const auto& output = ANALYZE("fun f(int a, bool b) -> bool false");
    REQUIRE(output.isSuccess());
    CHECK(
        GET_FUNC_DECL(output, "f", GET_TYPE_ID(output, "int"), GET_TYPE_ID(output, "bool"))
            .getOutput() == GET_TYPE_ID(output, "bool"));
  }

  SECTION("Declare type-inferred function") {
    const auto& output = ANALYZE("fun f(int a, bool b) b");
    REQUIRE(output.isSuccess());
    CHECK(
        GET_FUNC_DECL(output, "f", GET_TYPE_ID(output, "int"), GET_TYPE_ID(output, "bool"))
            .getOutput() == GET_TYPE_ID(output, "bool"));
  }

  SECTION("Declare overloaded function") {
    const auto& output = ANALYZE("fun f(int a) -> bool false "
                                 "fun f(string a) -> string \"hello world\" "
                                 "fun f(bool a) -> bool a "
                                 "fun f() -> int 1");
    REQUIRE(output.isSuccess());
    CHECK(
        GET_FUNC_DECL(output, "f", GET_TYPE_ID(output, "int")).getOutput() ==
        GET_TYPE_ID(output, "bool"));
    CHECK(
        GET_FUNC_DECL(output, "f", GET_TYPE_ID(output, "string")).getOutput() ==
        GET_TYPE_ID(output, "string"));
    CHECK(
        GET_FUNC_DECL(output, "f", GET_TYPE_ID(output, "bool")).getOutput() ==
        GET_TYPE_ID(output, "bool"));
    CHECK(GET_FUNC_DECL(output, "f").getOutput() == output.getProg().getInt());
  }

  SECTION("Declare template function") {
    const auto& output = ANALYZE("fun f{T}(T a) -> T a "
                                 "fun f2() -> int f{int}(1) "
                                 "fun f3() -> float f{float}(1.0)");
    REQUIRE(output.isSuccess());
    CHECK(
        GET_FUNC_DECL(output, "f__int", GET_TYPE_ID(output, "int")).getOutput() ==
        GET_TYPE_ID(output, "int"));
    CHECK(
        GET_FUNC_DECL(output, "f__float", GET_TYPE_ID(output, "float")).getOutput() ==
        GET_TYPE_ID(output, "float"));
  }

  SECTION("Declare conversion function") {
    const auto& output = ANALYZE("fun bool(int i) i != 0");
    REQUIRE(output.isSuccess());
    REQUIRE(GET_CONV(output, "int", "bool"));
  }

  SECTION("Declare conversion function with explicit ret type") {
    const auto& output = ANALYZE("fun bool(int i) -> bool i != 0");
    REQUIRE(output.isSuccess());
    REQUIRE(GET_CONV(output, "int", "bool"));
  }

  SECTION("Declare templated conversion function") {
    const auto& output = ANALYZE("struct Tuple{T, Y} = T a, Y b "
                                 "fun string{T, Y}(Tuple{T, Y} t) "
                                 " t.a + \",\" + t.b "
                                 "fun f() string{int, bool}(Tuple{int, bool}(42, false))");
    REQUIRE(output.isSuccess());
    CHECK(GET_FUNC_DECL(output, "f").getOutput() == GET_TYPE_ID(output, "string"));
    CHECK(
        GET_FUNC_DECL(output, "string", GET_TYPE_ID(output, "Tuple__int_bool")).getOutput() ==
        GET_TYPE_ID(output, "string"));
  }

  SECTION("Declare templated conversion function") {
    const auto& output = ANALYZE("struct Tuple{T, Y} = T a, Y b "
                                 "fun Tuple(int i) Tuple{int, bool}(i, false)"
                                 "fun f() Tuple(42)");
    REQUIRE(output.isSuccess());
    CHECK(GET_FUNC_DECL(output, "f").getOutput() == GET_TYPE_ID(output, "Tuple__int_bool"));
    CHECK(
        GET_FUNC_DECL(output, "Tuple", GET_TYPE_ID(output, "int")).getOutput() ==
        GET_TYPE_ID(output, "Tuple__int_bool"));
  }

  SECTION("Declare templated conversion function") {
    const auto& output = ANALYZE("struct Tuple{T, Y} = T a, Y b "
                                 "fun Tuple{T}(T t) Tuple{T, bool}(t, false) "
                                 "fun f() Tuple{string}(\"hello world\")");
    REQUIRE(output.isSuccess());
    CHECK(GET_FUNC_DECL(output, "f").getOutput() == GET_TYPE_ID(output, "Tuple__string_bool"));
    CHECK(
        GET_FUNC_DECL(output, "Tuple__string_bool", GET_TYPE_ID(output, "string")).getOutput() ==
        GET_TYPE_ID(output, "Tuple__string_bool"));
  }

  SECTION("Overload operator") {
    const auto& output = ANALYZE("fun -(bool b) !b "
                                 "fun f() -false");
    REQUIRE(output.isSuccess());

    auto args = std::vector<prog::expr::NodePtr>{};
    args.push_back(prog::expr::litBoolNode(output.getProg(), false));

    const auto& funcDef = GET_FUNC_DEF(output, "f");
    CHECK(
        funcDef.getExpr() ==
        *prog::expr::callExprNode(
            output.getProg(),
            GET_OP_ID(output, prog::Operator::Minus, GET_TYPE_ID(output, "bool")),
            std::move(args)));
  }

  SECTION("Diagnostics") {
    CHECK_DIAG(
        "fun print() -> int 1", errFuncNameConflictsWithAction(src, "print", input::Span{4, 8}));
    CHECK_DIAG(
        "fun a() -> int 1 "
        "fun a() -> int 1",
        errDuplicateFuncDeclaration(src, "a", input::Span{17, 32}));
    CHECK_DIAG("fun a(b c) -> int 1", errUndeclaredType(src, "b", input::Span{6, 6}));
    CHECK_DIAG("fun a() -> b 1", errUndeclaredType(src, "b", input::Span{11, 11}));
    CHECK_DIAG(
        "fun bool(int i) -> int i",
        errIncorrectReturnTypeInConvFunc(src, "bool", "int", input::Span{4, 7}));
    CHECK_DIAG(
        "struct s{T1, T2} = T1 a, T2 b "
        "fun s{T}(T a) -> T a "
        "fun f() s{int}(1)",
        errIncorrectReturnTypeInConvFunc(src, "s", "int", input::Span{30, 49}),
        errInvalidFuncInstantiation(src, input::Span{59, 67}));
    CHECK_DIAG(
        "fun -(int i) -> int 1",
        errDuplicateFuncDeclaration(src, "operator-minus", input::Span{0, 20}));
    CHECK_DIAG("fun ?() -> int 1", errNonOverloadableOperator(src, "qmark", input::Span{4, 4}));
    CHECK_DIAG(
        "fun f{int}() -> int 1", errTypeParamNameConflictsWithType(src, "int", input::Span{6, 8}));
    CHECK_DIAG(
        "fun f{T}(T{int} a) -> int i "
        "fun f() f{int}(1)",
        errTypeParamOnSubstitutionType(src, "T", input::Span{9, 14}),
        errUndeclaredType(src, "T", input::Span{9, 14}));
  }
}

} // namespace frontend
