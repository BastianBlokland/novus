#include "catch2/catch.hpp"
#include "helpers.hpp"

namespace vm {

TEST_CASE("Input / Output", "[vm]") {

  SECTION("Print") {
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitInt(42);
          builder->addPrintInt();

          builder->addLoadLitInt(-42);
          builder->addPrintInt();
        },
        "42",
        "-42");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitInt(0);
          builder->addPrintLogic();

          builder->addLoadLitInt(1);
          builder->addPrintLogic();

          builder->addLoadLitInt(5);
          builder->addPrintLogic();

          builder->addLoadLitInt(-42);
          builder->addPrintLogic();
        },
        "false",
        "true",
        "true",
        "true");
  }
}

} // namespace vm
