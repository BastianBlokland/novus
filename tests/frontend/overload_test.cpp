#include "catch2/catch.hpp"
#include "helpers.hpp"

namespace frontend {

TEST_CASE("[frontend] Analyzing overloads", "frontend") {

  SECTION("Allow conversion") {
    const auto& output = ANALYZE("fun f1(float a, float b) a + b "
                                 "fun f2() f1(42, 1337)");
    REQUIRE(output.isSuccess());
    CHECK(GET_FUNC_DEF(output, "f2").getBody().getType() == GET_TYPE_ID(output, "float"));
  }

  SECTION("Prefer overload with no conversion") {
    const auto& output = ANALYZE("fun f1(float f) f "
                                 "fun f1(int i) i "
                                 "fun f2() f1(1337)");
    REQUIRE(output.isSuccess());
    CHECK(GET_FUNC_DEF(output, "f2").getBody().getType() == GET_TYPE_ID(output, "int"));
  }

  SECTION("Prefer overload with less conversions") {
    const auto& output = ANALYZE("fun f1(float a, float b) b "
                                 "fun f1(float a, int b) b "
                                 "fun f2() f1(42, 1337)");
    REQUIRE(output.isSuccess());
    CHECK(GET_FUNC_DEF(output, "f2").getBody().getType() == GET_TYPE_ID(output, "int"));
  }

  SECTION("Allow conversion in binary operator") {
    const auto& output = ANALYZE("fun a(bool b) b "
                                 "a(1.0 / 2 == .5)");
    REQUIRE(output.isSuccess());
  }

  SECTION("Disallow more then one conversion in binary operator") {
    const auto& output = ANALYZE("fun fa(bool b) b"
                                 "fa(false + true)");
    REQUIRE(!output.isSuccess());
  }
}

} // namespace frontend
