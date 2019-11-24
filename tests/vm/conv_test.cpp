#include "catch2/catch.hpp"
#include "helpers.hpp"

namespace vm {

TEST_CASE("Execute conversions", "[vm]") {

  SECTION("String") {
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitInt(0);
          builder->addConvIntString();
          builder->addPrintString();
        },
        "0");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitInt(42);
          builder->addConvIntString();
          builder->addPrintString();
        },
        "42");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitInt(2147483647);
          builder->addConvIntString();
          builder->addPrintString();
        },
        "2147483647");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitInt(-2147483647);
          builder->addConvIntString();
          builder->addPrintString();
        },
        "-2147483647");
  }
}

} // namespace vm
