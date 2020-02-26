#include "catch2/catch.hpp"
#include "helpers.hpp"
#include "vm/exec_state.hpp"

namespace vm {

TEST_CASE("Execute long operations", "[vm]") {

  SECTION("Add") {
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitLong(1);
          asmb->addLoadLitLong(2);
          asmb->addAddLong();
          asmb->addConvLongString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "3");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitLong(9223372036854775807L);
          asmb->addLoadLitLong(1);
          asmb->addAddLong();
          asmb->addConvLongString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "-9223372036854775808");
  }

  SECTION("Substract") {
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitLong(10);
          asmb->addLoadLitLong(5);
          asmb->addSubLong();
          asmb->addConvLongString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "5");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitLong(10);
          asmb->addLoadLitLong(20);
          asmb->addSubLong();
          asmb->addConvLongString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "-10");
  }

  SECTION("Multiply") {
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitLong(3);
          asmb->addLoadLitLong(5);
          asmb->addMulLong();
          asmb->addConvLongString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "15");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitLong(3);
          asmb->addLoadLitLong(-5);
          asmb->addMulLong();
          asmb->addConvLongString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "-15");
  }

  SECTION("Divide") {
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitLong(33);
          asmb->addLoadLitLong(4);
          asmb->addDivLong();
          asmb->addConvLongString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "8");
    CHECK_EXPR_RESULTCODE(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitLong(1);
          asmb->addLoadLitLong(0);
          asmb->addDivLong();
          asmb->addConvLongString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        ExecState::DivByZero);
  }

  SECTION("Remainder") {
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitLong(33);
          asmb->addLoadLitLong(4);
          asmb->addRemLong();
          asmb->addConvLongString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "1");
    CHECK_EXPR_RESULTCODE(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitLong(1);
          asmb->addLoadLitLong(0);
          asmb->addRemLong();
          asmb->addConvLongString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        ExecState::DivByZero);
  }

  SECTION("Negate") {
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitLong(42);
          asmb->addNegLong();
          asmb->addConvLongString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "-42");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitLong(0);
          asmb->addNegLong();
          asmb->addConvLongString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "0");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitLong(42);
          asmb->addNegLong();
          asmb->addNegLong();
          asmb->addConvLongString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "42");
  }
}
} // namespace vm
