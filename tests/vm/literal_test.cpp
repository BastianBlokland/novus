#include "catch2/catch.hpp"
#include "helpers.hpp"

namespace vm {

TEST_CASE("Execute literals", "[vm]") {

  SECTION("Integer literals") {
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitInt(42);
          builder->addConvIntString();
          builder->addPCall(vm::PCallCode::PrintString);
          builder->addPop();
        },
        "input",
        "42");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitInt(-42);
          builder->addConvIntString();
          builder->addPCall(vm::PCallCode::PrintString);
          builder->addPop();
        },
        "input",
        "-42");
  }

  SECTION("Float literals") {
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitFloat(0.1337F); // NOLINT: Magic numbers
          builder->addConvFloatString();
          builder->addPCall(vm::PCallCode::PrintString);
          builder->addPop();
        },
        "input",
        "0.1337");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitFloat(-0.1337F); // NOLINT: Magic numbers
          builder->addConvFloatString();
          builder->addPCall(vm::PCallCode::PrintString);
          builder->addPop();
        },
        "input",
        "-0.1337");
  }

  SECTION("String literals") {
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitString("");
          builder->addPCall(vm::PCallCode::PrintString);
          builder->addPop();

          builder->addLoadLitString("hello");
          builder->addPCall(vm::PCallCode::PrintString);
          builder->addPop();

          builder->addLoadLitString(" ");
          builder->addPCall(vm::PCallCode::PrintString);
          builder->addPop();

          builder->addLoadLitString("world");
          builder->addPCall(vm::PCallCode::PrintString);
          builder->addPop();

          builder->addLoadLitString(" ");
          builder->addPCall(vm::PCallCode::PrintString);
          builder->addPop();

          builder->addLoadLitString("!");
          builder->addPCall(vm::PCallCode::PrintString);
          builder->addPop();
        },
        "input",
        "",
        "hello",
        " ",
        "world",
        " ",
        "!");
  }
}

} // namespace vm
