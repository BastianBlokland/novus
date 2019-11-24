#include "catch2/catch.hpp"
#include "helpers.hpp"

namespace vm {

TEST_CASE("Execute input and output", "[vm]") {

  SECTION("Print") {
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitString("hello world");
          builder->addPrintString();

          builder->addLoadLitInt(0);
          builder->addConvBoolString();
          builder->addPrintString();

          builder->addLoadLitInt(1);
          builder->addConvBoolString();
          builder->addPrintString();

          builder->addLoadLitInt(-42);
          builder->addConvIntString();
          builder->addPrintString();
        },
        "hello world",
        "false",
        "true",
        "-42");
  }
}

} // namespace vm
