#include "catch2/catch.hpp"
#include "helpers.hpp"

namespace vm {

TEST_CASE("Execute float checks", "[vm]") {

  SECTION("Equal") {
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitFloat(0.1337F);  // NOLINT: Magic numbers
          builder->addLoadLitFloat(-0.1337F); // NOLINT: Magic numbers
          builder->addCheckEqFloat();
          builder->addConvBoolString();
          builder->addPrintString();
        },
        "false");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitFloat(0.1337F); // NOLINT: Magic numbers
          builder->addLoadLitFloat(0.1337F); // NOLINT: Magic numbers
          builder->addCheckEqFloat();
          builder->addConvBoolString();
          builder->addPrintString();
        },
        "true");
  }

  SECTION("Greater") {
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitFloat(-0.1337F); // NOLINT: Magic numbers
          builder->addLoadLitFloat(0.1337F);  // NOLINT: Magic numbers
          builder->addCheckGtFloat();
          builder->addConvBoolString();
          builder->addPrintString();
        },
        "false");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitFloat(0.2337F); // NOLINT: Magic numbers
          builder->addLoadLitFloat(0.1337F); // NOLINT: Magic numbers
          builder->addCheckGtFloat();
          builder->addConvBoolString();
          builder->addPrintString();
        },
        "true");
  }

  SECTION("Less") {
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitFloat(0.2337F); // NOLINT: Magic numbers
          builder->addLoadLitFloat(0.1337F); // NOLINT: Magic numbers
          builder->addCheckLeFloat();
          builder->addConvBoolString();
          builder->addPrintString();
        },
        "false");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitFloat(0.1337F); // NOLINT: Magic numbers
          builder->addLoadLitFloat(0.2337F); // NOLINT: Magic numbers
          builder->addCheckLeFloat();
          builder->addConvBoolString();
          builder->addPrintString();
        },
        "true");
  }
}

} // namespace vm
