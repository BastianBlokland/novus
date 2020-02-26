#include "catch2/catch.hpp"
#include "helpers.hpp"

namespace vm {

TEST_CASE("Execute literals", "[vm]") {

  SECTION("Integer literals") {
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitInt(42);
          asmb->addConvIntString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "42");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitInt(-42);
          asmb->addConvIntString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "-42");
  }

  SECTION("Long literals") {
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitLong(4'200'000'000'000L);
          asmb->addConvLongString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "4200000000000");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitLong(-4'200'000'000'000L);
          asmb->addConvLongString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "-4200000000000");
  }

  SECTION("Float literals") {
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitFloat(0.1337F); // NOLINT: Magic numbers
          asmb->addConvFloatString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "0.1337");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitFloat(-0.1337F); // NOLINT: Magic numbers
          asmb->addConvFloatString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "-0.1337");
  }

  SECTION("String literals") {
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitString("");
          asmb->addPCall(novasm::PCallCode::ConWriteString);
          asmb->addPop();

          asmb->addLoadLitString("hello");
          asmb->addPCall(novasm::PCallCode::ConWriteString);
          asmb->addPop();

          asmb->addLoadLitString(" ");
          asmb->addPCall(novasm::PCallCode::ConWriteString);
          asmb->addPop();

          asmb->addLoadLitString("world");
          asmb->addPCall(novasm::PCallCode::ConWriteString);
          asmb->addPop();

          asmb->addLoadLitString(" ");
          asmb->addPCall(novasm::PCallCode::ConWriteString);
          asmb->addPop();

          asmb->addLoadLitString("!");
          asmb->addPCall(novasm::PCallCode::ConWriteString);
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
