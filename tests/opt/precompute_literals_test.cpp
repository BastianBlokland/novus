#include "catch2/catch.hpp"
#include "helpers.hpp"
#include "opt/opt.hpp"
#include "prog/expr/nodes.hpp"

namespace opt {

using namespace prog::expr;

TEST_CASE("Precompute literals", "[opt]") {
  SECTION("int") {
    ASSERT_EXPR(precomputeLiterals, "1 + 2", litIntNode(prog, 3));  // NOLINT: Magic numbers
    ASSERT_EXPR(precomputeLiterals, "1 - 2", litIntNode(prog, -1)); // NOLINT: Magic numbers
    ASSERT_EXPR(precomputeLiterals, "3 * 2", litIntNode(prog, 6));  // NOLINT: Magic numbers
    ASSERT_EXPR(precomputeLiterals, "4 / 2", litIntNode(prog, 2));  // NOLINT: Magic numbers
    ASSERT_EXPR(precomputeLiterals, "4 % 3", litIntNode(prog, 1));  // NOLINT: Magic numbers
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

  SECTION("float") {
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
    ASSERT_EXPR(precomputeLiterals, "float()", litFloatNode(prog, 0.0));
  }

  SECTION("long") {
    ASSERT_EXPR(precomputeLiterals, "1L + 2L", litLongNode(prog, 3));  // NOLINT: Magic numbers
    ASSERT_EXPR(precomputeLiterals, "1L - 2L", litLongNode(prog, -1)); // NOLINT: Magic numbers
    ASSERT_EXPR(precomputeLiterals, "3L * 2L", litLongNode(prog, 6));  // NOLINT: Magic numbers
    ASSERT_EXPR(precomputeLiterals, "4L / 2L", litLongNode(prog, 2));  // NOLINT: Magic numbers
    ASSERT_EXPR(precomputeLiterals, "4L % 3L", litLongNode(prog, 1));  // NOLINT: Magic numbers
    ASSERT_EXPR(precomputeLiterals, "-42L", litLongNode(prog, -42));   // NOLINT: Magic numbers
    ASSERT_EXPR(precomputeLiterals, "++1L", litLongNode(prog, 2));     // NOLINT: Magic numbers
    ASSERT_EXPR(precomputeLiterals, "--2L", litLongNode(prog, 1));     // NOLINT: Magic numbers
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

  SECTION("bool") {
    ASSERT_EXPR(precomputeLiterals, "!false", litBoolNode(prog, true));
    ASSERT_EXPR(precomputeLiterals, "false == true", litBoolNode(prog, false));
    ASSERT_EXPR(precomputeLiterals, "false != true", litBoolNode(prog, true));
    ASSERT_EXPR(precomputeLiterals, "bool()", litBoolNode(prog, false));
  }

  SECTION("char") {
    ASSERT_EXPR(precomputeLiterals, "'h' + 'w'", litStringNode(prog, "hw"));
    ASSERT_EXPR(precomputeLiterals, "string('h')", litStringNode(prog, "h"));
    ASSERT_EXPR(precomputeLiterals, "++'a'", litCharNode(prog, 'b'));
    ASSERT_EXPR(precomputeLiterals, "--'b'", litCharNode(prog, 'a'));
  }

  SECTION("string") {
    ASSERT_EXPR(precomputeLiterals, "\"hello\" + \"world\"", litStringNode(prog, "helloworld"));
    ASSERT_EXPR(precomputeLiterals, "\"hello\" + ' '", litStringNode(prog, "hello "));
    ASSERT_EXPR(precomputeLiterals, "length(\"hello\")", litIntNode(prog, 5));
    ASSERT_EXPR(
        precomputeLiterals, "\"hello\" + ' ' + \"world\"", litStringNode(prog, "hello world"));
    ASSERT_EXPR(precomputeLiterals, "\"hello\" == \"world\"", litBoolNode(prog, false));
    ASSERT_EXPR(precomputeLiterals, "\"hello\" == \"hello\"", litBoolNode(prog, true));
    ASSERT_EXPR(precomputeLiterals, "\"hello\" != \"world\"", litBoolNode(prog, true));
    ASSERT_EXPR(precomputeLiterals, "string()", litStringNode(prog, ""));
  }
}

} // namespace opt
