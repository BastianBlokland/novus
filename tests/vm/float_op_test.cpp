#include "catch2/catch.hpp"
#include "helpers.hpp"
#include "vm/exceptions/div_by_zero.hpp"

namespace vm {

TEST_CASE("Execute float operations", "[vm]") {

  SECTION("Add") {
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitFloat(0.1337F); // NOLINT: Magic numbers
          builder->addLoadLitFloat(0.1F);    // NOLINT: Magic numbers
          builder->addAddFloat();
          builder->addConvFloatString();
          builder->addPCall(vm::PCallCode::PrintString);
          builder->addPop();
        },
        "input",
        "0.2337");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitFloat(9999.0F); // NOLINT: Magic numbers
          builder->addLoadLitFloat(1.0F);
          builder->addAddFloat();
          builder->addConvFloatString();
          builder->addPCall(vm::PCallCode::PrintString);
          builder->addPop();
        },
        "input",
        "10000");
  }

  SECTION("Substract") {
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitFloat(0.1337F); // NOLINT: Magic numbers
          builder->addLoadLitFloat(0.1F);    // NOLINT: Magic numbers
          builder->addSubFloat();
          builder->addConvFloatString();
          builder->addPCall(vm::PCallCode::PrintString);
          builder->addPop();
        },
        "input",
        "0.0337");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitFloat(0.1F);    // NOLINT: Magic numbers
          builder->addLoadLitFloat(0.1337F); // NOLINT: Magic numbers
          builder->addSubFloat();
          builder->addConvFloatString();
          builder->addPCall(vm::PCallCode::PrintString);
          builder->addPop();
        },
        "input",
        "-0.0337");
  }

  SECTION("Multiply") {
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitFloat(0.1337F);  // NOLINT: Magic numbers
          builder->addLoadLitFloat(10000.0F); // NOLINT: Magic numbers
          builder->addMulFloat();
          builder->addConvFloatString();
          builder->addPCall(vm::PCallCode::PrintString);
          builder->addPop();
        },
        "input",
        "1337");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitFloat(1337.0F); // NOLINT: Magic numbers
          builder->addLoadLitFloat(0.0001F); // NOLINT: Magic numbers
          builder->addMulFloat();
          builder->addConvFloatString();
          builder->addPCall(vm::PCallCode::PrintString);
          builder->addPop();
        },
        "input",
        "0.1337");
  }

  SECTION("Divide") {
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitFloat(1.0F);
          builder->addLoadLitFloat(50.0F); // NOLINT: Magic numbers
          builder->addDivFloat();
          builder->addConvFloatString();
          builder->addPCall(vm::PCallCode::PrintString);
          builder->addPop();
        },
        "input",
        "0.02");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitFloat(1.0F);
          builder->addLoadLitFloat(0.0F);
          builder->addDivFloat();
          builder->addConvFloatString();
          builder->addPCall(vm::PCallCode::PrintString);
          builder->addPop();
        },
        "input",
        "inf");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitFloat(0.0F);
          builder->addLoadLitFloat(0.0F);
          builder->addDivFloat();
          builder->addConvFloatString();
          builder->addPCall(vm::PCallCode::PrintString);
          builder->addPop();
        },
        "input",
        "nan");
  }

  SECTION("Mod") {
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitFloat(1.3F); // NOLINT: Magic numbers
          builder->addLoadLitFloat(1.0F);
          builder->addModFloat();
          builder->addConvFloatString();
          builder->addPCall(vm::PCallCode::PrintString);
          builder->addPop();
        },
        "input",
        "0.3");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitFloat(1.25F); // NOLINT: Magic numbers
          builder->addLoadLitFloat(.25F);  // NOLINT: Magic numbers
          builder->addModFloat();
          builder->addConvFloatString();
          builder->addPCall(vm::PCallCode::PrintString);
          builder->addPop();
        },
        "input",
        "0");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitFloat(1.3F); // NOLINT: Magic numbers
          builder->addLoadLitFloat(-1.0F);
          builder->addModFloat();
          builder->addConvFloatString();
          builder->addPCall(vm::PCallCode::PrintString);
          builder->addPop();
        },
        "input",
        "0.3");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitFloat(1.3F); // NOLINT: Magic numbers
          builder->addLoadLitFloat(0.0F);
          builder->addModFloat();
          builder->addConvFloatString();
          builder->addPCall(vm::PCallCode::PrintString);
          builder->addPop();
        },
        "input",
        "nan");
  }

  SECTION("Negate") {
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitFloat(0.1337F); // NOLINT: Magic numbers
          builder->addNegFloat();
          builder->addConvFloatString();
          builder->addPCall(vm::PCallCode::PrintString);
          builder->addPop();
        },
        "input",
        "-0.1337");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitFloat(0);
          builder->addNegFloat();
          builder->addConvFloatString();
          builder->addPCall(vm::PCallCode::PrintString);
          builder->addPop();
        },
        "input",
        "-0");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitFloat(0.1337F); // NOLINT: Magic numbers
          builder->addNegFloat();
          builder->addNegFloat();
          builder->addConvFloatString();
          builder->addPCall(vm::PCallCode::PrintString);
          builder->addPop();
        },
        "input",
        "0.1337");
  }
}

} // namespace vm
