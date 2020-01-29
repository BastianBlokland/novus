#include "catch2/catch.hpp"
#include "helpers.hpp"

namespace frontend {

TEST_CASE("Analyzing overloads", "[frontend]") {

  SECTION("Allow conversion") {
    const auto& output = ANALYZE("fun f1(float a, float b) a + b "
                                 "fun f2() f1(42, 1337)");
    REQUIRE(output.isSuccess());
    CHECK(GET_FUNC_DEF(output, "f2").getExpr().getType() == GET_TYPE_ID(output, "float"));
  }

  SECTION("Prefer overload with no conversion") {
    const auto& output = ANALYZE("fun f1(float f) f "
                                 "fun f1(int i) i "
                                 "fun f2() f1(1337)");
    REQUIRE(output.isSuccess());
    CHECK(GET_FUNC_DEF(output, "f2").getExpr().getType() == GET_TYPE_ID(output, "int"));
  }

  SECTION("Prefer overload with less conversions") {
    const auto& output = ANALYZE("fun f1(float a, float b) b "
                                 "fun f1(float a, int b) b "
                                 "fun f2() f1(42, 1337)");
    REQUIRE(output.isSuccess());
    CHECK(GET_FUNC_DEF(output, "f2").getExpr().getType() == GET_TYPE_ID(output, "int"));
  }

  SECTION("Allow conversion in binary operator") {
    const auto& output = ANALYZE("conWrite(string(1.0 / 2))");
    REQUIRE(output.isSuccess());
  }

  SECTION("Disallow more then one conversion in binary operator") {
    const auto& output = ANALYZE("conWrite(false + true)");
    REQUIRE(!output.isSuccess());
  }
}

} // namespace frontend
