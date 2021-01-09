#include "catch2/catch.hpp"
#include "helpers.hpp"

namespace vm {

TEST_CASE("[vm] Execute literals", "vm") {

  SECTION("Integer literals") {
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitInt(42);
          asmb->addConvIntString();
          ADD_PRINT(asmb);
        },
        "input",
        "42");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitInt(-42);
          asmb->addConvIntString();
          ADD_PRINT(asmb);
        },
        "input",
        "-42");
  }

  SECTION("Long literals") {
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitLong(4'200'000'000'000L);
          asmb->addConvLongString();
          ADD_PRINT(asmb);
        },
        "input",
        "4200000000000");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitLong(-4'200'000'000'000L);
          asmb->addConvLongString();
          ADD_PRINT(asmb);
        },
        "input",
        "-4200000000000");
  }

  SECTION("Float literals") {
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitFloat(0.1337F); // NOLINT: Magic numbers
          asmb->addConvFloatString();
          ADD_PRINT(asmb);
        },
        "input",
        "0.1337");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitFloat(-0.1337F); // NOLINT: Magic numbers
          asmb->addConvFloatString();
          ADD_PRINT(asmb);
        },
        "input",
        "-0.1337");
  }

  SECTION("String literals") {
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitString("hello");
          ADD_PRINT(asmb);
          asmb->addPop();

          asmb->addLoadLitString(" ");
          ADD_PRINT(asmb);
          asmb->addPop();

          asmb->addLoadLitString("world");
          ADD_PRINT(asmb);
          asmb->addPop();

          asmb->addLoadLitString(" ");
          ADD_PRINT(asmb);
          asmb->addPop();

          asmb->addLoadLitString("!");
          ADD_PRINT(asmb);
        },
        "input",
        "hello world !");
  }
}

} // namespace vm
