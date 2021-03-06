#include "catch2/catch.hpp"
#include "helpers.hpp"
#include "opt/opt.hpp"
#include "prog/expr/nodes.hpp"

namespace opt {

using namespace prog::expr;

TEST_CASE("[opt] Precompute literals", "opt") {
  SECTION("switch expression") {
    ASSERT_EXPR(
        precomputeLiterals,
        "if intrinsic{int_gt_int}(1, 2) -> 1 "
        "else                           -> 2",
        litIntNode(prog, 2));
    ASSERT_EXPR(
        precomputeLiterals,
        "if intrinsic{int_gt_int}(2, 1) -> 1 "
        "else                           -> 2",
        litIntNode(prog, 1));
    ASSERT_EXPR(
        precomputeLiterals,
        "if intrinsic{int_le_int}(1, 1) -> 1 "
        "if intrinsic{int_gt_int}(2, 3) -> 2 "
        "else                           -> 3",
        litIntNode(prog, 3));
    ASSERT_EXPR(
        precomputeLiterals,
        "if intrinsic{int_le_int}(1, 1) -> 1 "
        "if intrinsic{int_gt_int}(4, 3) -> 2 "
        "else                           -> 3",
        litIntNode(prog, 2));

    ASSERT_EXPR(
        precomputeLiterals,
        "if intrinsic{int_le_int}(1, 1)                               -> 1 "
        "if intrinsic{int_gt_int}(intrinsic{env_argument_count}(), 3) -> 2 "
        "else                                                         -> 3",
        ([&]() {
          auto args = std::vector<prog::expr::NodePtr>{};
          args.push_back(callExprNode(prog, GET_INTRINSIC_ID(prog, "env_argument_count"), {}));
          args.push_back(litIntNode(prog, 3));

          auto conditions = std::vector<prog::expr::NodePtr>{};
          conditions.push_back(prog::expr::callExprNode(
              prog,
              GET_INTRINSIC_ID(prog, "int_gt_int", prog.getInt(), prog.getInt()),
              std::move(args)));

          auto branches = std::vector<prog::expr::NodePtr>{};
          branches.push_back(litIntNode(prog, 2));
          branches.push_back(litIntNode(prog, 3));
          return switchExprNode(prog, std::move(conditions), std::move(branches));
        })());
  }

  SECTION("int intrinsics") {
    ASSERT_EXPR_INT(precomputeLiterals, "intrinsic{int_add_int}(1, 2)", litIntNode(prog, 3));
    ASSERT_EXPR_INT(precomputeLiterals, "intrinsic{int_sub_int}(1, 2)", litIntNode(prog, -1));
    ASSERT_EXPR_INT(precomputeLiterals, "intrinsic{int_mul_int}(3, 2)", litIntNode(prog, 6));
    ASSERT_EXPR_INT(precomputeLiterals, "intrinsic{int_div_int}(4, 2)", litIntNode(prog, 2));
    ASSERT_EXPR_INT(
        precomputeLiterals,
        "intrinsic{int_div_int}(4, intrinsic{int_neg}(2))",
        litIntNode(prog, -2));
    ASSERT_EXPR_INT(
        precomputeLiterals,
        "intrinsic{int_div_int}(4, 0)",
        getIntrinsicIntBinaryOp(prog, "int_div_int", 4, 0));
    ASSERT_EXPR_INT(
        precomputeLiterals,
        "intrinsic{int_div_int}(0, 0)",
        getIntrinsicIntBinaryOp(prog, "int_div_int", 0, 0));
    ASSERT_EXPR_INT(precomputeLiterals, "intrinsic{int_rem_int}(4, 3)", litIntNode(prog, 1));
    ASSERT_EXPR_INT(
        precomputeLiterals,
        "intrinsic{int_rem_int}(4, intrinsic{int_neg}(3))",
        litIntNode(prog, 1));
    ASSERT_EXPR_INT(
        precomputeLiterals,
        "intrinsic{int_rem_int}(4, 0)",
        getIntrinsicIntBinaryOp(prog, "int_rem_int", 4, 0));
    ASSERT_EXPR_INT(
        precomputeLiterals,
        "intrinsic{int_rem_int}(0, 0)",
        getIntrinsicIntBinaryOp(prog, "int_rem_int", 0, 0));
    ASSERT_EXPR_INT(precomputeLiterals, "intrinsic{int_neg}(42)", litIntNode(prog, -42));
    ASSERT_EXPR_INT(precomputeLiterals, "intrinsic{int_shiftleft}(2, 1)", litIntNode(prog, 4));
    ASSERT_EXPR_INT(precomputeLiterals, "intrinsic{int_shiftright}(4, 1)", litIntNode(prog, 2));
    ASSERT_EXPR_INT(precomputeLiterals, "intrinsic{int_and_int}(3, 1)", litIntNode(prog, 1));
    ASSERT_EXPR_INT(precomputeLiterals, "intrinsic{int_or_int}(2, 1)", litIntNode(prog, 3));
    ASSERT_EXPR_INT(precomputeLiterals, "intrinsic{int_xor_int}(3, 1)", litIntNode(prog, 2));
    ASSERT_EXPR_INT(precomputeLiterals, "intrinsic{int_inv}(1)", litIntNode(prog, -2));
    ASSERT_EXPR_BOOL(precomputeLiterals, "intrinsic{int_eq_int}(1, 2)", litBoolNode(prog, false));
    ASSERT_EXPR_BOOL(precomputeLiterals, "intrinsic{int_le_int}(1, 2)", litBoolNode(prog, true));
    ASSERT_EXPR_BOOL(precomputeLiterals, "intrinsic{int_gt_int}(1, 2)", litBoolNode(prog, false));
    ASSERT_EXPR_LONG(precomputeLiterals, "intrinsic{int_to_long}(137)", litLongNode(prog, 137));
    ASSERT_EXPR_FLOAT(precomputeLiterals, "intrinsic{int_to_float}(137)", litFloatNode(prog, 137));
    ASSERT_EXPR_STRING(
        precomputeLiterals, "intrinsic{int_to_string}(1337)", litStringNode(prog, "1337"));
    ASSERT_EXPR_CHAR(precomputeLiterals, "intrinsic{int_to_char}(137)", litCharNode(prog, 137));
    ASSERT_EXPR_CHAR(precomputeLiterals, "intrinsic{int_to_char}(1337)", litCharNode(prog, 57));
  }

  SECTION("float intrinsics") {
    ASSERT_EXPR_FLOAT(
        precomputeLiterals, "intrinsic{float_add_float}(1.1, 1.2)", litFloatNode(prog, 2.3));
    ASSERT_EXPR_FLOAT(
        precomputeLiterals, "intrinsic{float_sub_float}(1.2, 1.1)", litFloatNode(prog, 0.1));
    ASSERT_EXPR_FLOAT(
        precomputeLiterals, "intrinsic{float_mul_float}(1.1, 1.2)", litFloatNode(prog, 1.32));
    ASSERT_EXPR_FLOAT(
        precomputeLiterals, "intrinsic{float_div_float}(3.3, 1.1)", litFloatNode(prog, 3.0));
    ASSERT_EXPR_FLOAT(
        precomputeLiterals, "intrinsic{float_mod_float}(5.2, 2.2)", litFloatNode(prog, 0.8));
    ASSERT_EXPR_FLOAT(
        precomputeLiterals, "intrinsic{float_pow}(1.1, 2.0)", litFloatNode(prog, 1.21));
    ASSERT_EXPR_FLOAT(precomputeLiterals, "intrinsic{float_sqrt}(4.0)", litFloatNode(prog, 2.0));
    ASSERT_EXPR_FLOAT(precomputeLiterals, "intrinsic{float_sin}(0.0)", litFloatNode(prog, 0.0));
    ASSERT_EXPR_FLOAT(precomputeLiterals, "intrinsic{float_cos}(0.0)", litFloatNode(prog, 1.0));
    ASSERT_EXPR_FLOAT(precomputeLiterals, "intrinsic{float_tan}(0.0)", litFloatNode(prog, 0.0));
    ASSERT_EXPR_FLOAT(precomputeLiterals, "intrinsic{float_asin}(0.0)", litFloatNode(prog, 0.0));
    ASSERT_EXPR_FLOAT(precomputeLiterals, "intrinsic{float_acos}(1.0)", litFloatNode(prog, 0.0));
    ASSERT_EXPR_FLOAT(precomputeLiterals, "intrinsic{float_atan}(0.0)", litFloatNode(prog, 0.0));
    ASSERT_EXPR_FLOAT(
        precomputeLiterals, "intrinsic{float_atan2}(0.0, 0.0)", litFloatNode(prog, 0.0));
    ASSERT_EXPR_FLOAT(precomputeLiterals, "intrinsic{float_neg}(1.1)", litFloatNode(prog, -1.1));
    ASSERT_EXPR_BOOL(
        precomputeLiterals, "intrinsic{float_eq_float}(1.1, 1.2)", litBoolNode(prog, false));
    ASSERT_EXPR_BOOL(
        precomputeLiterals, "intrinsic{float_le_float}(1.1, 1.2)", litBoolNode(prog, true));
    ASSERT_EXPR_BOOL(
        precomputeLiterals, "intrinsic{float_gt_float}(1.1, 1.2)", litBoolNode(prog, false));
    ASSERT_EXPR_INT(precomputeLiterals, "intrinsic{float_to_int}(1337.0)", litIntNode(prog, 1337));
    ASSERT_EXPR_STRING(
        precomputeLiterals,
        "intrinsic{float_to_string}(intrinsic{float_div_float}(0.0, 0.0), 1536)",
        litStringNode(prog, "nan"));
    ASSERT_EXPR_STRING(
        precomputeLiterals,
        "intrinsic{float_to_string}(42.1337, 1536)",
        litStringNode(prog, "42.1337"));
    ASSERT_EXPR_STRING(
        precomputeLiterals,
        "intrinsic{float_to_string}(.00001337, 1536)",
        litStringNode(prog, "1.337e-05"));
    ASSERT_EXPR_STRING(
        precomputeLiterals,
        "intrinsic{float_to_string}(.00001337, 1537)",
        litStringNode(prog, "0.000013"));
    ASSERT_EXPR_STRING(
        precomputeLiterals,
        "intrinsic{float_to_string}(.00001337, 2049)",
        litStringNode(prog, "0.00001337"));
    ASSERT_EXPR_STRING(
        precomputeLiterals,
        "intrinsic{float_to_string}(42.1337, 1538)",
        litStringNode(prog, "4.213370e+01"));
    ASSERT_EXPR_STRING(
        precomputeLiterals,
        "intrinsic{float_to_string}(intrinsic{float_neg}(42.1337), 1536)",
        litStringNode(prog, "-42.1337"));
    ASSERT_EXPR_CHAR(precomputeLiterals, "intrinsic{float_to_char}(230.0)", litCharNode(prog, 230));
    ASSERT_EXPR_LONG(precomputeLiterals, "intrinsic{float_to_long}(230.0)", litLongNode(prog, 230));
  }

  SECTION("long intrinsics") {
    ASSERT_EXPR_LONG(precomputeLiterals, "intrinsic{long_add_long}(1L, 2L)", litLongNode(prog, 3));
    ASSERT_EXPR_LONG(precomputeLiterals, "intrinsic{long_sub_long}(1L, 2L)", litLongNode(prog, -1));
    ASSERT_EXPR_LONG(precomputeLiterals, "intrinsic{long_mul_long}(3L, 2L)", litLongNode(prog, 6));
    ASSERT_EXPR_LONG(precomputeLiterals, "intrinsic{long_div_long}(4L, 2L)", litLongNode(prog, 2));
    ASSERT_EXPR_LONG(
        precomputeLiterals,
        "intrinsic{long_div_long}(4L, intrinsic{long_neg}(2L))",
        litLongNode(prog, -2));
    ASSERT_EXPR_LONG(
        precomputeLiterals,
        "intrinsic{long_div_long}(4L, 0L)",
        getIntrinsicLongBinaryOp(prog, "long_div_long", 4L, 0L));
    ASSERT_EXPR_LONG(
        precomputeLiterals,
        "intrinsic{long_div_long}(0L, 0L)",
        getIntrinsicLongBinaryOp(prog, "long_div_long", 0L, 0L));
    ASSERT_EXPR_LONG(precomputeLiterals, "intrinsic{long_rem_long}(4L, 3L)", litLongNode(prog, 1));
    ASSERT_EXPR_LONG(
        precomputeLiterals,
        "intrinsic{long_rem_long}(4L, intrinsic{long_neg}(3L))",
        litLongNode(prog, 1));
    ASSERT_EXPR_LONG(
        precomputeLiterals,
        "intrinsic{long_rem_long}(4L, 0L)",
        getIntrinsicLongBinaryOp(prog, "long_rem_long", 4L, 0L));
    ASSERT_EXPR_LONG(
        precomputeLiterals,
        "intrinsic{long_rem_long}(0L, 0L)",
        getIntrinsicLongBinaryOp(prog, "long_rem_long", 0L, 0L));
    ASSERT_EXPR_LONG(precomputeLiterals, "intrinsic{long_neg}(42L)", litLongNode(prog, -42));
    ASSERT_EXPR_LONG(precomputeLiterals, "intrinsic{long_shiftleft}(2L, 1)", litLongNode(prog, 4));
    ASSERT_EXPR_LONG(precomputeLiterals, "intrinsic{long_shiftright}(4L, 1)", litLongNode(prog, 2));
    ASSERT_EXPR_LONG(precomputeLiterals, "intrinsic{long_and_long}(3L, 1L)", litLongNode(prog, 1));
    ASSERT_EXPR_LONG(precomputeLiterals, "intrinsic{long_or_long}(2L, 1L)", litLongNode(prog, 3));
    ASSERT_EXPR_LONG(precomputeLiterals, "intrinsic{long_xor_long}(3L, 1L)", litLongNode(prog, 2));
    ASSERT_EXPR_LONG(precomputeLiterals, "intrinsic{long_inv}(1L)", litLongNode(prog, -2));
    ASSERT_EXPR_BOOL(
        precomputeLiterals, "intrinsic{long_eq_long}(1L, 2L)", litBoolNode(prog, false));
    ASSERT_EXPR_BOOL(
        precomputeLiterals, "intrinsic{long_le_long}(1L, 2L)", litBoolNode(prog, true));
    ASSERT_EXPR_BOOL(
        precomputeLiterals, "intrinsic{long_gt_long}(1L, 2L)", litBoolNode(prog, false));
    ASSERT_EXPR_INT(precomputeLiterals, "intrinsic{long_to_int}(137L)", litIntNode(prog, 137));
    ASSERT_EXPR_STRING(
        precomputeLiterals, "intrinsic{long_to_string}(1337L)", litStringNode(prog, "1337"));
    ASSERT_EXPR_CHAR(precomputeLiterals, "intrinsic{long_to_char}(137L)", litCharNode(prog, 137));
  }

  SECTION("char intrinsics") {
    ASSERT_EXPR_STRING(
        precomputeLiterals, "intrinsic{char_to_string}('h')", litStringNode(prog, "h"));
  }

  SECTION("string intrinsics") {
    ASSERT_EXPR_STRING(
        precomputeLiterals,
        "intrinsic{string_add_string}(\"hello\", \"world\")",
        litStringNode(prog, "helloworld"));
    ASSERT_EXPR_STRING(
        precomputeLiterals,
        "intrinsic{string_add_char}(\"hello\", ' ')",
        litStringNode(prog, "hello "));
    ASSERT_EXPR_INT(precomputeLiterals, "intrinsic{string_length}(\"hello\")", litIntNode(prog, 5));
    ASSERT_EXPR_CHAR(
        precomputeLiterals, "intrinsic{string_index}(\"hello\", 0)", litCharNode(prog, 'h'));
    ASSERT_EXPR_CHAR(
        precomputeLiterals, "intrinsic{string_index}(\"hello\", 4)", litCharNode(prog, 'o'));
    ASSERT_EXPR_CHAR(
        precomputeLiterals, "intrinsic{string_index}(\"hello\", 5)", litCharNode(prog, '\0'));
    ASSERT_EXPR_STRING(
        precomputeLiterals,
        "intrinsic{string_slice}(\"hello world\", 0, 5)",
        litStringNode(prog, "hello"));
    ASSERT_EXPR_STRING(
        precomputeLiterals,
        "intrinsic{string_slice}(\"hello world\", 4, 4)",
        litStringNode(prog, ""));
    ASSERT_EXPR_STRING(
        precomputeLiterals,
        "intrinsic{string_slice}(\"hello world\", 4, 5)",
        litStringNode(prog, "o"));
    ASSERT_EXPR_STRING(
        precomputeLiterals,
        "intrinsic{string_slice}(\"hello world\", 6, 11)",
        litStringNode(prog, "world"));
    ASSERT_EXPR_STRING(
        precomputeLiterals,
        "intrinsic{string_slice}(\"hello world\", 6, 99)",
        litStringNode(prog, "world"));
    ASSERT_EXPR_STRING(
        precomputeLiterals,
        "intrinsic{string_slice}(\"hello world\", 2, 9)",
        litStringNode(prog, "llo wor"));
    ASSERT_EXPR_STRING(
        precomputeLiterals,
        "intrinsic{string_slice}(\"hello world\", 8, 3)",
        litStringNode(prog, ""));
    ASSERT_EXPR_STRING(
        precomputeLiterals,
        "intrinsic{string_slice}(\"hello world\", 0, 11)",
        litStringNode(prog, "hello world"));
    ASSERT_EXPR_STRING(
        precomputeLiterals,
        "intrinsic{string_add_string}(intrinsic{string_add_char}(\"hello\", ' '), \"world\")",
        litStringNode(prog, "hello world"));
    ASSERT_EXPR_BOOL(
        precomputeLiterals,
        "intrinsic{string_eq_string}(\"hello\", \"world\")",
        litBoolNode(prog, false));
    ASSERT_EXPR_BOOL(
        precomputeLiterals,
        "intrinsic{string_eq_string}(\"hello\", \"hello\")",
        litBoolNode(prog, true));
  }

  SECTION("simplify intrinsic calls") {
    {
      const auto& output = ANALYZE("fun f(int i) -> bool intrinsic{int_eq_int}(i, 0)");
      REQUIRE(output.isSuccess());
      const auto prog     = precomputeLiterals(output.getProg());
      const auto& funcDef = GET_FUNC_DEF(prog, "f", prog.getInt());
      CHECK(
          funcDef.getBody() ==
          *prog::expr::callExprNode(
              prog,
              GET_INTRINSIC_ID(prog, "int_eq_zero", prog.getInt()),
              EXPRS(prog::expr::constExprNode(
                  funcDef.getConsts(), *funcDef.getConsts().lookup("i")))));
    }
    {
      const auto& output = ANALYZE("fun f(string s) -> bool intrinsic{string_eq_string}(s, \"\")");
      REQUIRE(output.isSuccess());
      const auto prog     = precomputeLiterals(output.getProg());
      const auto& funcDef = GET_FUNC_DEF(prog, "f", prog.getString());
      CHECK(
          funcDef.getBody() ==
          *prog::expr::callExprNode(
              prog,
              GET_INTRINSIC_ID(prog, "string_eq_empty", prog.getString()),
              EXPRS(prog::expr::constExprNode(
                  funcDef.getConsts(), *funcDef.getConsts().lookup("s")))));
    }
    {
      const auto& output =
          ANALYZE("fun f(string s) -> string intrinsic{string_add_string}(s, \"\")");
      REQUIRE(output.isSuccess());
      const auto prog     = precomputeLiterals(output.getProg());
      const auto& funcDef = GET_FUNC_DEF(prog, "f", prog.getString());
      CHECK(
          funcDef.getBody() ==
          *prog::expr::constExprNode(funcDef.getConsts(), *funcDef.getConsts().lookup("s")));
    }
    {
      const auto& output =
          ANALYZE("fun f(string s) -> string intrinsic{string_add_string}(\"\", s)");
      REQUIRE(output.isSuccess());
      const auto prog     = precomputeLiterals(output.getProg());
      const auto& funcDef = GET_FUNC_DEF(prog, "f", prog.getString());
      CHECK(
          funcDef.getBody() ==
          *prog::expr::constExprNode(funcDef.getConsts(), *funcDef.getConsts().lookup("s")));
    }
  }

  SECTION("reinterpret conversions") {
    {
      const auto& output = ANALYZE("enum e = a : 42 "
                                   "fun f() int(e.a)");
      REQUIRE(output.isSuccess());
      const auto prog = precomputeLiterals(output.getProg());
      CHECK(GET_FUNC_DEF(prog, "f").getBody() == *litIntNode(prog, 42));
    }

    {
      const auto& output = ANALYZE("enum e = a : 42 "
                                   "fun f() int(e.a)");
      REQUIRE(output.isSuccess());
      const auto prog = precomputeLiterals(output.getProg());
      CHECK(GET_FUNC_DEF(prog, "f").getBody() == *litIntNode(prog, 42));
    }
  }

  SECTION("temporary structs") {
    const auto& output = ANALYZE("struct s = int i "
                                 "fun f() s(42).i");
    REQUIRE(output.isSuccess());
    const auto prog = precomputeLiterals(output.getProg());
    CHECK(GET_FUNC_DEF(prog, "f").getBody() == *litIntNode(prog, 42));
  }

  SECTION("dynamic call to func literal") {
    const auto& output = ANALYZE("fun *(int x, int y) -> int intrinsic{int_mul_int}(x, y) "
                                 "fun f1(int i) i * 42 "
                                 "fun f2() (f1)(42)");
    REQUIRE(output.isSuccess());
    // Check that it originally is a dynamic call.
    REQUIRE(
        GET_FUNC_DEF(output.getProg(), "f2").getBody().getKind() == prog::expr::NodeKind::CallDyn);

    const auto prog = precomputeLiterals(output.getProg());

    // Verify that it was optimized into a normal call.
    auto callExpr = callExprNode(
        prog, GET_FUNC_ID(prog, "f1", GET_TYPE_ID(prog, "int")), EXPRS(litIntNode(prog, 42)));

    CHECK(GET_FUNC_DEF(prog, "f2").getBody() == *callExpr);
  }

  SECTION("dynamic call to closure") {
    const auto& output = ANALYZE("fun *(int x, int y) -> int intrinsic{int_mul_int}(x, y) "
                                 "fun f(int x) (lambda (int y) x * y)(42)");
    REQUIRE(output.isSuccess());
    // Check that it originally is a dynamic call.
    REQUIRE(
        GET_FUNC_DEF(output.getProg(), "f", GET_TYPE_ID(output.getProg(), "int"))
            .getBody()
            .getKind() == prog::expr::NodeKind::CallDyn);

    const auto prog = precomputeLiterals(output.getProg());

    // Verify that it was optimized into a normal call.
    REQUIRE(
        GET_FUNC_DEF(prog, "f", GET_TYPE_ID(prog, "int")).getBody().getKind() ==
        prog::expr::NodeKind::Call);
  }

  SECTION("Lazy get to normal lazy call") {
    const auto& output = ANALYZE("fun f1(int a, int b) -> int intrinsic{int_add_int}(a, b) "
                                 "fun f2() -> int intrinsic{lazy_get}(lazy f1(1, 2))");
    REQUIRE(output.isSuccess());
    // Check that it originally call lazy-get.
    auto& orgCall =
        *GET_FUNC_DEF(output.getProg(), "f2").getBody().downcast<prog::expr::CallExprNode>();
    REQUIRE(
        output.getProg().getFuncDecl(orgCall.getFunc()).getKind() == prog::sym::FuncKind::LazyGet);

    const auto prog = precomputeLiterals(output.getProg());

    // Verify that it was optimized to call f1 directly.
    auto& optCall = *GET_FUNC_DEF(prog, "f2").getBody().downcast<prog::expr::CallExprNode>();
    CHECK(
        optCall.getFunc() ==
        GET_FUNC_ID(
            prog,
            "f1",
            GET_TYPE_ID(output.getProg(), "int"),
            GET_TYPE_ID(output.getProg(), "int")));
  }

  SECTION("Lazy get to dynamic lazy call") {
    const auto& output = ANALYZE("fun f2(function{int, int, int} f) -> int "
                                 "  intrinsic{lazy_get}(lazy f(1, 2))");
    REQUIRE(output.isSuccess());
    // Check that it originally call lazy-get.
    auto& orgCall =
        *GET_FUNC_DEF(
             output.getProg(), "f2", GET_TYPE_ID(output.getProg(), "__function_int_int_int"))
             .getBody()
             .downcast<prog::expr::CallExprNode>();
    REQUIRE(
        output.getProg().getFuncDecl(orgCall.getFunc()).getKind() == prog::sym::FuncKind::LazyGet);

    const auto prog = precomputeLiterals(output.getProg());

    // Verify that it was optimized to call the delegate directly.
    auto& optCall =
        GET_FUNC_DEF(prog, "f2", GET_TYPE_ID(output.getProg(), "__function_int_int_int")).getBody();
    CHECK(optCall.getKind() == prog::expr::NodeKind::CallDyn);
  }
}

} // namespace opt
