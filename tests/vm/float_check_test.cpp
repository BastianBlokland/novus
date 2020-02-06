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
          builder->addPCall(vm::PCallCode::ConWriteString);
        },
        "input",
        "false");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitFloat(0.1337F); // NOLINT: Magic numbers
          builder->addLoadLitFloat(0.1337F); // NOLINT: Magic numbers
          builder->addCheckEqFloat();
          builder->addConvBoolString();
          builder->addPCall(vm::PCallCode::ConWriteString);
        },
        "input",
        "true");
  }

  SECTION("Greater") {
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitFloat(-0.1337F); // NOLINT: Magic numbers
          builder->addLoadLitFloat(0.1337F);  // NOLINT: Magic numbers
          builder->addCheckGtFloat();
          builder->addConvBoolString();
          builder->addPCall(vm::PCallCode::ConWriteString);
        },
        "input",
        "false");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitFloat(0.2337F); // NOLINT: Magic numbers
          builder->addLoadLitFloat(0.1337F); // NOLINT: Magic numbers
          builder->addCheckGtFloat();
          builder->addConvBoolString();
          builder->addPCall(vm::PCallCode::ConWriteString);
        },
        "input",
        "true");
  }

  SECTION("Less") {
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitFloat(0.2337F); // NOLINT: Magic numbers
          builder->addLoadLitFloat(0.1337F); // NOLINT: Magic numbers
          builder->addCheckLeFloat();
          builder->addConvBoolString();
          builder->addPCall(vm::PCallCode::ConWriteString);
        },
        "input",
        "false");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitFloat(0.1337F); // NOLINT: Magic numbers
          builder->addLoadLitFloat(0.2337F); // NOLINT: Magic numbers
          builder->addCheckLeFloat();
          builder->addConvBoolString();
          builder->addPCall(vm::PCallCode::ConWriteString);
        },
        "input",
        "true");
  }
}

} // namespace vm
