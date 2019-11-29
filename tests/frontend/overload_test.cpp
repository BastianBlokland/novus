#include "catch2/catch.hpp"
#include "helpers.hpp"
#include "prog/expr/node_call.hpp"

namespace frontend {

TEST_CASE("Analyzing overloads", "[frontend]") {

  SECTION("Allow conversion") {
    const auto& output = ANALYZE("fun f1(string a, string b) a + b "
                                 "fun f2() f1(42, 1337)");
    REQUIRE(output.isSuccess());
    CHECK(GET_FUNC_DEF(output, "f2").getExpr().getType() == GET_TYPE_ID(output, "string"));
  }

  SECTION("Prefer overload with no conversion") {
    const auto& output = ANALYZE("fun f1(string s) s "
                                 "fun f1(int i) i "
                                 "fun f2() f1(1337)");
    REQUIRE(output.isSuccess());
    CHECK(GET_FUNC_DEF(output, "f2").getExpr().getType() == GET_TYPE_ID(output, "int"));
  }

  SECTION("Prefer overload with less conversions") {
    const auto& output = ANALYZE("fun f1(string a, string b) b "
                                 "fun f1(string a, int b) b "
                                 "fun f2() f1(42, 1337)");
    REQUIRE(output.isSuccess());
    CHECK(GET_FUNC_DEF(output, "f2").getExpr().getType() == GET_TYPE_ID(output, "int"));
  }

  SECTION("Allow conversion in binary operator") {
    const auto& output = ANALYZE("print(\"hello\" + 42)");
    REQUIRE(output.isSuccess());
  }

  SECTION("Disallow more then one conversion in binary operator") {
    const auto& output = ANALYZE("print(false + true)");
    REQUIRE(!output.isSuccess());
  }
}

} // namespace frontend
