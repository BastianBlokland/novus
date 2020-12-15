#include "catch2/catch.hpp"
#include "helpers.hpp"
#include "opt/opt.hpp"
#include "prog/expr/nodes.hpp"

namespace opt {

using namespace prog::expr;

TEST_CASE("Precompute literals", "[opt]") {
  SECTION("switch expression") {
    ASSERT_EXPR(
        precomputeLiterals,
        "if 1 > 2 -> 1 "
        "else     -> 2",
        litIntNode(prog, 2)); // NOLINT: Magic numbers
    ASSERT_EXPR(
        precomputeLiterals,
        "if 2 > 1 -> 1 "
        "else     -> 2",
        litIntNode(prog, 1)); // NOLINT: Magic numbers
    ASSERT_EXPR(
        precomputeLiterals,
        "if 1 < 1 -> 1 "
        "if 2 > 3 -> 2 "
        "else     -> 3",
        litIntNode(prog, 3)); // NOLINT: Magic numbers
    ASSERT_EXPR(
        precomputeLiterals,
        "if 1 < 1 -> 1 "
        "if 4 > 3 -> 2 "
        "else     -> 3",
        litIntNode(prog, 2)); // NOLINT: Magic numbers

    ASSERT_EXPR(
        precomputeLiterals,
        "if 1 < 1                 -> 1 "
        "if envGetArgCount() > 3  -> 2 "
        "else                     -> 3",
        ([&]() {
          auto args = std::vector<prog::expr::NodePtr>{};
          args.push_back(callExprNode(prog, GET_FUNC_ID(prog, "envGetArgCount"), {}));
          args.push_back(litIntNode(prog, 3));

          auto conditions = std::vector<prog::expr::NodePtr>{};
          conditions.push_back(prog::expr::callExprNode(
              prog,
              GET_OP_FUNC_ID(prog, prog::Operator::Gt, prog.getInt(), prog.getInt()),
              std::move(args)));

          auto branches = std::vector<prog::expr::NodePtr>{};
          branches.push_back(litIntNode(prog, 2));
          branches.push_back(litIntNode(prog, 3));
          return switchExprNode(prog, std::move(conditions), std::move(branches));
        })());
  }

  SECTION("int intrinsics") {
    ASSERT_EXPR(precomputeLiterals, "1 + 2", litIntNode(prog, 3));   // NOLINT: Magic numbers
    ASSERT_EXPR(precomputeLiterals, "1 - 2", litIntNode(prog, -1));  // NOLINT: Magic numbers
    ASSERT_EXPR(precomputeLiterals, "3 * 2", litIntNode(prog, 6));   // NOLINT: Magic numbers
    ASSERT_EXPR(precomputeLiterals, "4 / 2", litIntNode(prog, 2));   // NOLINT: Magic numbers
    ASSERT_EXPR(precomputeLiterals, "4 / -2", litIntNode(prog, -2)); // NOLINT: Magic numbers
    ASSERT_EXPR(
        precomputeLiterals,
        "4 / 0",
        getIntBinaryOpExpr(prog, prog::Operator::Slash, 4, 0)); // NOLINT: Magic numbers
    ASSERT_EXPR(precomputeLiterals, "0 / 0", getIntBinaryOpExpr(prog, prog::Operator::Slash, 0, 0));
    ASSERT_EXPR(precomputeLiterals, "4 % 3", litIntNode(prog, 1));  // NOLINT: Magic numbers
    ASSERT_EXPR(precomputeLiterals, "4 % -3", litIntNode(prog, 1)); // NOLINT: Magic numbers
    ASSERT_EXPR(
        precomputeLiterals,
        "4 % 0",
        getIntBinaryOpExpr(prog, prog::Operator::Rem, 4, 0)); // NOLINT: Magic numbers
    ASSERT_EXPR(precomputeLiterals, "0 % 0", getIntBinaryOpExpr(prog, prog::Operator::Rem, 0, 0));
    ASSERT_EXPR(precomputeLiterals, "-42", litIntNode(prog, -42));  // NOLINT: Magic numbers
    ASSERT_EXPR(precomputeLiterals, "++1", litIntNode(prog, 2));    // NOLINT: Magic numbers
    ASSERT_EXPR(precomputeLiterals, "--2", litIntNode(prog, 1));    // NOLINT: Magic numbers
    ASSERT_EXPR(precomputeLiterals, "2 << 1", litIntNode(prog, 4)); // NOLINT: Magic numbers
    ASSERT_EXPR(precomputeLiterals, "4 >> 1", litIntNode(prog, 2)); // NOLINT: Magic numbers
    ASSERT_EXPR(precomputeLiterals, "3 & 1", litIntNode(prog, 1));  // NOLINT: Magic numbers
    ASSERT_EXPR(precomputeLiterals, "2 | 1", litIntNode(prog, 3));  // NOLINT: Magic numbers
    ASSERT_EXPR(precomputeLiterals, "3 ^ 1", litIntNode(prog, 2));  // NOLINT: Magic numbers
    ASSERT_EXPR(precomputeLiterals, "~1", litIntNode(prog, -2));    // NOLINT: Magic numbers
    ASSERT_EXPR(precomputeLiterals, "1 == 2", litBoolNode(prog, false));
    ASSERT_EXPR(precomputeLiterals, "1 != 2", litBoolNode(prog, true));
    ASSERT_EXPR(precomputeLiterals, "1 < 2", litBoolNode(prog, true));
    ASSERT_EXPR(precomputeLiterals, "1 <= 2", litBoolNode(prog, true));
    ASSERT_EXPR(precomputeLiterals, "1 > 2", litBoolNode(prog, false));
    ASSERT_EXPR(precomputeLiterals, "1 >= 2", litBoolNode(prog, false));
    ASSERT_EXPR(precomputeLiterals, "long(137)", litLongNode(prog, 137));   // NOLINT: Magic numbers
    ASSERT_EXPR(precomputeLiterals, "float(137)", litFloatNode(prog, 137)); // NOLINT: Magic numbers
    ASSERT_EXPR(precomputeLiterals, "string(1337)", litStringNode(prog, "1337"));
    ASSERT_EXPR(precomputeLiterals, "char(137)", litCharNode(prog, 137)); // NOLINT: Magic numbers
    ASSERT_EXPR(precomputeLiterals, "char(1337)", litCharNode(prog, 57)); // NOLINT: Magic numbers
    ASSERT_EXPR(precomputeLiterals, "int()", litIntNode(prog, 0));
  }

  SECTION("float intrinsics") {
    ASSERT_EXPR(precomputeLiterals, "1.1 + 1.2", litFloatNode(prog, 2.3));  // NOLINT: Magic numbers
    ASSERT_EXPR(precomputeLiterals, "1.2 - 1.1", litFloatNode(prog, 0.1));  // NOLINT: Magic numbers
    ASSERT_EXPR(precomputeLiterals, "1.1 * 1.2", litFloatNode(prog, 1.32)); // NOLINT: Magic numbers
    ASSERT_EXPR(precomputeLiterals, "3.3 / 1.1", litFloatNode(prog, 3.0));  // NOLINT: Magic numbers
    ASSERT_EXPR(precomputeLiterals, "5.2 % 2.2", litFloatNode(prog, 0.8));  // NOLINT: Magic numbers
    ASSERT_EXPR(
        precomputeLiterals, "pow(1.1, 2.0)", litFloatNode(prog, 1.21));    // NOLINT: Magic numbers
    ASSERT_EXPR(precomputeLiterals, "sqrt(4.0)", litFloatNode(prog, 2.0)); // NOLINT: Magic numbers
    ASSERT_EXPR(precomputeLiterals, "sin(0.0)", litFloatNode(prog, 0.0));  // NOLINT: Magic numbers
    ASSERT_EXPR(precomputeLiterals, "cos(0.0)", litFloatNode(prog, 1.0));  // NOLINT: Magic numbers
    ASSERT_EXPR(precomputeLiterals, "tan(0.0)", litFloatNode(prog, 0.0));  // NOLINT: Magic numbers
    ASSERT_EXPR(precomputeLiterals, "asin(0.0)", litFloatNode(prog, 0.0)); // NOLINT: Magic numbers
    ASSERT_EXPR(precomputeLiterals, "acos(1.0)", litFloatNode(prog, 0.0)); // NOLINT: Magic numbers
    ASSERT_EXPR(precomputeLiterals, "atan(0.0)", litFloatNode(prog, 0.0)); // NOLINT: Magic numbers
    ASSERT_EXPR(
        precomputeLiterals, "atan2(0.0, 0.0)", litFloatNode(prog, 0.0)); // NOLINT: Magic numbers
    ASSERT_EXPR(precomputeLiterals, "-1.1", litFloatNode(prog, -1.1));   // NOLINT: Magic numbers
    ASSERT_EXPR(precomputeLiterals, "--1.1", litFloatNode(prog, 0.1));   // NOLINT: Magic numbers
    ASSERT_EXPR(precomputeLiterals, "++1.1", litFloatNode(prog, 2.1));   // NOLINT: Magic numbers
    ASSERT_EXPR(precomputeLiterals, "1.1 == 1.2", litBoolNode(prog, false));
    ASSERT_EXPR(precomputeLiterals, "1.1 != 1.2", litBoolNode(prog, true));
    ASSERT_EXPR(precomputeLiterals, "1.1 < 1.2", litBoolNode(prog, true));
    ASSERT_EXPR(precomputeLiterals, "1.1 <= 1.2", litBoolNode(prog, true));
    ASSERT_EXPR(precomputeLiterals, "1.1 > 1.2", litBoolNode(prog, false));
    ASSERT_EXPR(precomputeLiterals, "1.1 >= 1.2", litBoolNode(prog, false));
    ASSERT_EXPR(precomputeLiterals, "int(1337.0)", litIntNode(prog, 1337)); // NOLINT: Magic numbers
    ASSERT_EXPR(precomputeLiterals, "string(0.0 / 0.0)", litStringNode(prog, "nan"));
    ASSERT_EXPR(precomputeLiterals, "string(42.1337)", litStringNode(prog, "42.1337"));
    ASSERT_EXPR(precomputeLiterals, "string(-42.1337)", litStringNode(prog, "-42.1337"));
    ASSERT_EXPR(precomputeLiterals, "char(230.0)", litCharNode(prog, 230)); // NOLINT: Magic numbers
    ASSERT_EXPR(precomputeLiterals, "long(230.0)", litLongNode(prog, 230)); // NOLINT: Magic numbers
    ASSERT_EXPR(precomputeLiterals, "float()", litFloatNode(prog, 0.0));
  }

  SECTION("long intrinsics") {
    ASSERT_EXPR(precomputeLiterals, "1L + 2L", litLongNode(prog, 3));   // NOLINT: Magic numbers
    ASSERT_EXPR(precomputeLiterals, "1L - 2L", litLongNode(prog, -1));  // NOLINT: Magic numbers
    ASSERT_EXPR(precomputeLiterals, "3L * 2L", litLongNode(prog, 6));   // NOLINT: Magic numbers
    ASSERT_EXPR(precomputeLiterals, "4L / 2L", litLongNode(prog, 2));   // NOLINT: Magic numbers
    ASSERT_EXPR(precomputeLiterals, "4L / -2L", litLongNode(prog, -2)); // NOLINT: Magic numbers
    ASSERT_EXPR(
        precomputeLiterals,
        "4L / 0L",
        getLongBinaryOpExpr(prog, prog::Operator::Slash, 4, 0)); // NOLINT: Magic numbers
    ASSERT_EXPR(
        precomputeLiterals, "0L / 0L", getLongBinaryOpExpr(prog, prog::Operator::Slash, 0, 0));
    ASSERT_EXPR(precomputeLiterals, "4L % 3L", litLongNode(prog, 1));  // NOLINT: Magic numbers
    ASSERT_EXPR(precomputeLiterals, "4L % -3L", litLongNode(prog, 1)); // NOLINT: Magic numbers
    ASSERT_EXPR(
        precomputeLiterals,
        "4L % 0L",
        getLongBinaryOpExpr(prog, prog::Operator::Rem, 4, 0)); // NOLINT: Magic numbers
    ASSERT_EXPR(
        precomputeLiterals, "0L % 0L", getLongBinaryOpExpr(prog, prog::Operator::Rem, 0, 0));
    ASSERT_EXPR(precomputeLiterals, "-42L", litLongNode(prog, -42)); // NOLINT: Magic numbers
    ASSERT_EXPR(precomputeLiterals, "++1L", litLongNode(prog, 2));   // NOLINT: Magic numbers
    ASSERT_EXPR(precomputeLiterals, "--2L", litLongNode(prog, 1));   // NOLINT: Magic numbers
    ASSERT_EXPR(precomputeLiterals, "1L == 2L", litBoolNode(prog, false));
    ASSERT_EXPR(precomputeLiterals, "1L != 2L", litBoolNode(prog, true));
    ASSERT_EXPR(precomputeLiterals, "1L < 2L", litBoolNode(prog, true));
    ASSERT_EXPR(precomputeLiterals, "1L <= 2L", litBoolNode(prog, true));
    ASSERT_EXPR(precomputeLiterals, "1L > 2L", litBoolNode(prog, false));
    ASSERT_EXPR(precomputeLiterals, "1L >= 2L", litBoolNode(prog, false));
    ASSERT_EXPR(precomputeLiterals, "int(137L)", litIntNode(prog, 137)); // NOLINT: Magic numbers
    ASSERT_EXPR(precomputeLiterals, "string(1337L)", litStringNode(prog, "1337"));
    ASSERT_EXPR(precomputeLiterals, "long()", litLongNode(prog, 0));
  }

  SECTION("bool intrinsics") {
    ASSERT_EXPR(precomputeLiterals, "!false", litBoolNode(prog, true));
    ASSERT_EXPR(precomputeLiterals, "false == true", litBoolNode(prog, false));
    ASSERT_EXPR(precomputeLiterals, "false != true", litBoolNode(prog, true));
    ASSERT_EXPR(precomputeLiterals, "bool()", litBoolNode(prog, false));
  }

  SECTION("char intrinsics") {
    ASSERT_EXPR(precomputeLiterals, "'h' + 'w'", litStringNode(prog, "hw"));
    ASSERT_EXPR(precomputeLiterals, "string('h')", litStringNode(prog, "h"));
    ASSERT_EXPR(precomputeLiterals, "++'a'", litCharNode(prog, 'b'));
    ASSERT_EXPR(precomputeLiterals, "--'b'", litCharNode(prog, 'a'));
    ASSERT_EXPR(precomputeLiterals, "'b' == 'c'", litBoolNode(prog, false));
    ASSERT_EXPR(precomputeLiterals, "'b' == 'b'", litBoolNode(prog, true));
    ASSERT_EXPR(precomputeLiterals, "'b' != 'c'", litBoolNode(prog, true));
    ASSERT_EXPR(precomputeLiterals, "'b' != 'b'", litBoolNode(prog, false));
    ASSERT_EXPR(precomputeLiterals, "'a' < 'b'", litBoolNode(prog, true));
    ASSERT_EXPR(precomputeLiterals, "'b' < 'a'", litBoolNode(prog, false));
    ASSERT_EXPR(precomputeLiterals, "'a' <= 'b'", litBoolNode(prog, true));
    ASSERT_EXPR(precomputeLiterals, "'a' <= 'a'", litBoolNode(prog, true));
    ASSERT_EXPR(precomputeLiterals, "'b' <= 'a'", litBoolNode(prog, false));
    ASSERT_EXPR(precomputeLiterals, "'a' > 'b'", litBoolNode(prog, false));
    ASSERT_EXPR(precomputeLiterals, "'b' > 'a'", litBoolNode(prog, true));
    ASSERT_EXPR(precomputeLiterals, "'a' >= 'b'", litBoolNode(prog, false));
    ASSERT_EXPR(precomputeLiterals, "'a' >= 'a'", litBoolNode(prog, true));
    ASSERT_EXPR(precomputeLiterals, "'b' >= 'a'", litBoolNode(prog, true))
  }

  SECTION("string intrinsics") {
    ASSERT_EXPR(precomputeLiterals, "\"hello\" + \"world\"", litStringNode(prog, "helloworld"));
    ASSERT_EXPR(precomputeLiterals, "\"hello\" + ' '", litStringNode(prog, "hello "));
    ASSERT_EXPR(precomputeLiterals, "length(\"hello\")", litIntNode(prog, 5));
    ASSERT_EXPR(precomputeLiterals, "\"hello\"[0]", litCharNode(prog, 'h'));
    ASSERT_EXPR(precomputeLiterals, "\"hello\"[4]", litCharNode(prog, 'o'));
    ASSERT_EXPR(precomputeLiterals, "\"hello\"[-1]", litCharNode(prog, '\0'));
    ASSERT_EXPR(precomputeLiterals, "\"hello\"[5]", litCharNode(prog, '\0'));
    ASSERT_EXPR(precomputeLiterals, "\"hello world\"[0, 5]", litStringNode(prog, "hello"));
    ASSERT_EXPR(precomputeLiterals, "\"hello world\"[4, 4]", litStringNode(prog, ""));
    ASSERT_EXPR(precomputeLiterals, "\"hello world\"[4, 5]", litStringNode(prog, "o"));
    ASSERT_EXPR(precomputeLiterals, "\"hello world\"[-99, 5]", litStringNode(prog, "hello"));
    ASSERT_EXPR(precomputeLiterals, "\"hello world\"[6, 11]", litStringNode(prog, "world"));
    ASSERT_EXPR(precomputeLiterals, "\"hello world\"[6, 99]", litStringNode(prog, "world"));
    ASSERT_EXPR(precomputeLiterals, "\"hello world\"[2, 9]", litStringNode(prog, "llo wor"));
    ASSERT_EXPR(precomputeLiterals, "\"hello world\"[8, 3]", litStringNode(prog, ""));
    ASSERT_EXPR(precomputeLiterals, "\"hello world\"[0, 11]", litStringNode(prog, "hello world"));
    ASSERT_EXPR(
        precomputeLiterals, "\"hello\" + ' ' + \"world\"", litStringNode(prog, "hello world"));
    ASSERT_EXPR(precomputeLiterals, "\"hello\" == \"world\"", litBoolNode(prog, false));
    ASSERT_EXPR(precomputeLiterals, "\"hello\" == \"hello\"", litBoolNode(prog, true));
    ASSERT_EXPR(precomputeLiterals, "\"hello\" != \"world\"", litBoolNode(prog, true));
    ASSERT_EXPR(precomputeLiterals, "string()", litStringNode(prog, ""));
  }

  SECTION("identity conversions") {
    ASSERT_EXPR(precomputeLiterals, "+42", litIntNode(prog, 42));       // NOLINT: Magic numbers
    ASSERT_EXPR(precomputeLiterals, "+42.1", litFloatNode(prog, 42.1)); // NOLINT: Magic numbers
    ASSERT_EXPR(precomputeLiterals, "+42L", litLongNode(prog, 42));     // NOLINT: Magic numbers

    ASSERT_EXPR(precomputeLiterals, "int(42)", litIntNode(prog, 42)); // NOLINT: Magic numbers
    ASSERT_EXPR(
        precomputeLiterals, "float(42.1)", litFloatNode(prog, 42.1));    // NOLINT: Magic numbers
    ASSERT_EXPR(precomputeLiterals, "long(42L)", litLongNode(prog, 42)); // NOLINT: Magic numbers
    ASSERT_EXPR(precomputeLiterals, "bool(true)", litBoolNode(prog, true));
    ASSERT_EXPR(precomputeLiterals, "string(\"hello\")", litStringNode(prog, "hello"));
    ASSERT_EXPR(precomputeLiterals, "char('a')", litCharNode(prog, 'a'));
  }

  SECTION("reinterpret conversions") {
    ASSERT_EXPR(precomputeLiterals, "int('a')", litIntNode(prog, 'a'));
    ASSERT_EXPR(precomputeLiterals, "asFloat(0xFFA0_0000)", litFloatNode(prog, std::nanf("")));
    ASSERT_EXPR(precomputeLiterals, "asInt(0.0)", litIntNode(prog, 0));
    ASSERT_EXPR(
        precomputeLiterals, "asInt(asFloat(42))", litIntNode(prog, 42)); // NOLINT: Magic numbers

    {
      const auto& output = ANALYZE("enum e = a : 42 "
                                   "fun f() int(e.a)");
      REQUIRE(output.isSuccess());
      const auto prog = precomputeLiterals(output.getProg());
      CHECK(GET_FUNC_DEF(prog, "f").getExpr() == *litIntNode(prog, 42)); // NOLINT: Magic numbers
    }

    {
      const auto& output = ANALYZE("enum e = a : 42 "
                                   "fun f() int(e.a)");
      REQUIRE(output.isSuccess());
      const auto prog = precomputeLiterals(output.getProg());
      CHECK(GET_FUNC_DEF(prog, "f").getExpr() == *litIntNode(prog, 42)); // NOLINT: Magic numbers
    }
  }

  SECTION("temporary structs") {
    const auto& output = ANALYZE("struct s = int i "
                                 "fun f() s(42).i");
    REQUIRE(output.isSuccess());
    const auto prog = precomputeLiterals(output.getProg());
    CHECK(GET_FUNC_DEF(prog, "f").getExpr() == *litIntNode(prog, 42)); // NOLINT: Magic numbers
  }

  SECTION("dynamic call to func literal") {
    const auto& output = ANALYZE("fun f1(int i) i * 42 "
                                 "fun f2() (f1)(42)");
    REQUIRE(output.isSuccess());
    // Check that it originally is a dynamic call.
    REQUIRE(
        GET_FUNC_DEF(output.getProg(), "f2").getExpr().getKind() == prog::expr::NodeKind::CallDyn);

    const auto prog = precomputeLiterals(output.getProg());

    // Verify that it was optimized into a normal call.
    auto args = std::vector<prog::expr::NodePtr>{};
    args.push_back(litIntNode(prog, 42));
    auto callExpr =
        callExprNode(prog, GET_FUNC_ID(prog, "f1", GET_TYPE_ID(prog, "int")), std::move(args));

    CHECK(GET_FUNC_DEF(prog, "f2").getExpr() == *callExpr);
  }

  SECTION("dynamic call to closure") {
    const auto& output = ANALYZE("fun f(int x) (lambda (int y) x * y)(42)");
    REQUIRE(output.isSuccess());
    // Check that it originally is a dynamic call.
    REQUIRE(
        GET_FUNC_DEF(output.getProg(), "f", GET_TYPE_ID(output.getProg(), "int"))
            .getExpr()
            .getKind() == prog::expr::NodeKind::CallDyn);

    const auto prog = precomputeLiterals(output.getProg());

    // Verify that it was optimized into a normal call.
    REQUIRE(
        GET_FUNC_DEF(prog, "f", GET_TYPE_ID(prog, "int")).getExpr().getKind() ==
        prog::expr::NodeKind::Call);
  }
}

} // namespace opt
