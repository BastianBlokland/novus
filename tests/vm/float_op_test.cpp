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
          builder->addPCall(vm::PCallCode::Print);
          builder->addPop();
        },
        "0.2337");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitFloat(9999.0F); // NOLINT: Magic numbers
          builder->addLoadLitFloat(1.0F);
          builder->addAddFloat();
          builder->addConvFloatString();
          builder->addPCall(vm::PCallCode::Print);
          builder->addPop();
        },
        "10000");
  }

  SECTION("Substract") {
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitFloat(0.1337F); // NOLINT: Magic numbers
          builder->addLoadLitFloat(0.1F);    // NOLINT: Magic numbers
          builder->addSubFloat();
          builder->addConvFloatString();
          builder->addPCall(vm::PCallCode::Print);
          builder->addPop();
        },
        "0.0337");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitFloat(0.1F);    // NOLINT: Magic numbers
          builder->addLoadLitFloat(0.1337F); // NOLINT: Magic numbers
          builder->addSubFloat();
          builder->addConvFloatString();
          builder->addPCall(vm::PCallCode::Print);
          builder->addPop();
        },
        "-0.0337");
  }

  SECTION("Multiply") {
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitFloat(0.1337F);  // NOLINT: Magic numbers
          builder->addLoadLitFloat(10000.0F); // NOLINT: Magic numbers
          builder->addMulFloat();
          builder->addConvFloatString();
          builder->addPCall(vm::PCallCode::Print);
          builder->addPop();
        },
        "1337");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitFloat(1337.0F); // NOLINT: Magic numbers
          builder->addLoadLitFloat(0.0001F); // NOLINT: Magic numbers
          builder->addMulFloat();
          builder->addConvFloatString();
          builder->addPCall(vm::PCallCode::Print);
          builder->addPop();
        },
        "0.1337");
  }

  SECTION("Divide") {
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitFloat(1.0F);
          builder->addLoadLitFloat(50.0F); // NOLINT: Magic numbers
          builder->addDivFloat();
          builder->addConvFloatString();
          builder->addPCall(vm::PCallCode::Print);
          builder->addPop();
        },
        "0.02");
    CHECK_EXPR_THROWS(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitFloat(1.0F);
          builder->addLoadLitFloat(0.0F);
          builder->addDivFloat();
          builder->addConvFloatString();
          builder->addPCall(vm::PCallCode::Print);
          builder->addPop();
        },
        vm::exceptions::DivByZero);
  }

  SECTION("Negate") {
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitFloat(0.1337F); // NOLINT: Magic numbers
          builder->addNegFloat();
          builder->addConvFloatString();
          builder->addPCall(vm::PCallCode::Print);
          builder->addPop();
        },
        "-0.1337");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitFloat(0);
          builder->addNegFloat();
          builder->addConvFloatString();
          builder->addPCall(vm::PCallCode::Print);
          builder->addPop();
        },
        "-0");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitFloat(0.1337F); // NOLINT: Magic numbers
          builder->addNegFloat();
          builder->addNegFloat();
          builder->addConvFloatString();
          builder->addPCall(vm::PCallCode::Print);
          builder->addPop();
        },
        "0.1337");
  }
}

} // namespace vm
