#include "catch2/catch.hpp"
#include "helpers.hpp"

namespace backend {

TEST_CASE("Generate assembly for literals", "[backend]") {

  SECTION("Int literals") {
    CHECK_EXPR_INT("42", [](backend::Builder* builder) -> void { builder->addLoadLitInt(42); });
  }

  SECTION("Long literals") {
    CHECK_EXPR_LONG("4200000000000L", [](backend::Builder* builder) -> void {
      builder->addLoadLitLong(4'200'000'000'000L);
    });
  }

  SECTION("Float literals") {
    CHECK_EXPR_FLOAT(".1337", [](backend::Builder* builder) -> void {
      builder->addLoadLitFloat(0.1337F); // NOLINT: Magic numbers
    });
  }

  SECTION("Bool literals") {
    CHECK_EXPR_BOOL("false", [](backend::Builder* builder) -> void { builder->addLoadLitInt(0); });
    CHECK_EXPR_BOOL("true", [](backend::Builder* builder) -> void { builder->addLoadLitInt(1); });
  }

  SECTION("String literals") {
    CHECK_EXPR_STRING("\"hello world\"", [](backend::Builder* builder) -> void {
      builder->addLoadLitString("hello world");
    });
    CHECK_EXPR_STRING(
        "\"\"", [](backend::Builder* builder) -> void { builder->addLoadLitString(""); });
  }

  SECTION("Char literals") {
    CHECK_EXPR_CHAR("'a'", [](backend::Builder* builder) -> void { builder->addLoadLitInt('a'); });
  }
}

} // namespace backend
