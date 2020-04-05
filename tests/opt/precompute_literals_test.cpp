#include "catch2/catch.hpp"
#include "helpers.hpp"
#include "opt/opt.hpp"
#include "prog/expr/nodes.hpp"

namespace opt {

using namespace prog::expr;

TEST_CASE("Precompute literals", "[opt]") {
  SECTION("int") {
    ASSERT_EXPR(precomputeLiterals, "-42", litIntNode(prog, -42));        // NOLINT: Magic numbers
    ASSERT_EXPR(precomputeLiterals, "char(137)", litCharNode(prog, 137)); // NOLINT: Magic numbers
    ASSERT_EXPR(precomputeLiterals, "char(1337)", litCharNode(prog, 57)); // NOLINT: Magic numbers
  }

  SECTION("float") {
    ASSERT_EXPR(precomputeLiterals, "-1.337", litFloatNode(prog, -1.337)); // NOLINT: Magic numbers
  }

  SECTION("long") {
    ASSERT_EXPR(precomputeLiterals, "-42L", litLongNode(prog, -42L)); // NOLINT: Magic numbers
  }

  SECTION("char") {
    ASSERT_EXPR(
        precomputeLiterals, "'h' + 'w'", litStringNode(prog, "hw")); // NOLINT: Magic numbers
    ASSERT_EXPR(
        precomputeLiterals, "string('h')", litStringNode(prog, "h")); // NOLINT: Magic numbers
  }

  SECTION("string") {
    ASSERT_EXPR(
        precomputeLiterals,
        "\"hello\" + \"world\"",
        litStringNode(prog, "helloworld")); // NOLINT: Magic numbers
    ASSERT_EXPR(
        precomputeLiterals,
        "\"hello\" + ' '",
        litStringNode(prog, "hello ")); // NOLINT: Magic numbers
    ASSERT_EXPR(
        precomputeLiterals,
        "\"hello\" + ' ' + \"world\"",
        litStringNode(prog, "hello world")); // NOLINT: Magic numbers
  }
}

} // namespace opt
