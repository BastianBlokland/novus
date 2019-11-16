#include "catch2/catch.hpp"
#include "helpers.hpp"

namespace vm {

TEST_CASE("Loading literals", "[vm]") {

  SECTION("Integer literals") {
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitInt(42);
          builder->addPrintInt();
        },
        "42");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitInt(-42);
          builder->addPrintInt();
        },
        "-42");
  }
}

} // namespace vm
