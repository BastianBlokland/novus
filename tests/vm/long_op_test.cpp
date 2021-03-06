#include "catch2/catch.hpp"
#include "helpers.hpp"
#include "vm/exec_state.hpp"

namespace vm {

TEST_CASE("[vm] Execute long operations", "vm") {

  SECTION("Add") {
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitLong(1);
          asmb->addLoadLitLong(2);
          asmb->addAddLong();
          asmb->addConvLongString();
          ADD_PRINT(asmb);
        },
        "input",
        "3");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitLong(9223372036854775807L);
          asmb->addLoadLitLong(1);
          asmb->addAddLong();
          asmb->addConvLongString();
          ADD_PRINT(asmb);
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
          ADD_PRINT(asmb);
        },
        "input",
        "5");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitLong(10);
          asmb->addLoadLitLong(20);
          asmb->addSubLong();
          asmb->addConvLongString();
          ADD_PRINT(asmb);
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
          ADD_PRINT(asmb);
        },
        "input",
        "15");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitLong(3);
          asmb->addLoadLitLong(-5);
          asmb->addMulLong();
          asmb->addConvLongString();
          ADD_PRINT(asmb);
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
          ADD_PRINT(asmb);
        },
        "input",
        "8");
    CHECK_EXPR_RESULTCODE(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitLong(1);
          asmb->addLoadLitLong(0);
          asmb->addDivLong();
          asmb->addConvLongString();
          ADD_PRINT(asmb);
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
          ADD_PRINT(asmb);
        },
        "input",
        "1");
    CHECK_EXPR_RESULTCODE(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitLong(1);
          asmb->addLoadLitLong(0);
          asmb->addRemLong();
          asmb->addConvLongString();
          ADD_PRINT(asmb);
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
          ADD_PRINT(asmb);
        },
        "input",
        "-42");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitLong(0);
          asmb->addNegLong();
          asmb->addConvLongString();
          ADD_PRINT(asmb);
        },
        "input",
        "0");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitLong(42);
          asmb->addNegLong();
          asmb->addNegLong();
          asmb->addConvLongString();
          ADD_PRINT(asmb);
        },
        "input",
        "42");
  }

  SECTION("Shift left") {
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitLong(1);
          asmb->addLoadLitInt(1);
          asmb->addShiftLeftLong();
          asmb->addConvLongString();
          ADD_PRINT(asmb);
        },
        "input",
        "2");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitLong(2);
          asmb->addLoadLitInt(2);
          asmb->addShiftLeftLong();
          asmb->addConvLongString();
          ADD_PRINT(asmb);
        },
        "input",
        "8");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitLong(4);
          asmb->addLoadLitInt(0);
          asmb->addShiftLeftLong();
          asmb->addConvLongString();
          ADD_PRINT(asmb);
        },
        "input",
        "4");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitLong(1);
          asmb->addLoadLitInt(32);
          asmb->addShiftLeftLong();
          asmb->addConvLongString();
          ADD_PRINT(asmb);
        },
        "input",
        "4294967296");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitLong(1);
          asmb->addLoadLitInt(40);
          asmb->addShiftLeftLong();
          asmb->addConvLongString();
          ADD_PRINT(asmb);
        },
        "input",
        "1099511627776");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitLong(1);
          asmb->addLoadLitInt(64);
          asmb->addShiftLeftLong();
          asmb->addConvLongString();
          ADD_PRINT(asmb);
        },
        "input",
        "1");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitLong(2);
          asmb->addLoadLitInt(-2);
          asmb->addShiftLeftLong();
          asmb->addConvLongString();
          ADD_PRINT(asmb);
        },
        "input",
        "-9223372036854775808");
  }

  SECTION("Shift right") {
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitLong(1);
          asmb->addLoadLitInt(1);
          asmb->addShiftRightLong();
          asmb->addConvLongString();
          ADD_PRINT(asmb);
        },
        "input",
        "0");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitLong(8);
          asmb->addLoadLitInt(2);
          asmb->addShiftRightLong();
          asmb->addConvLongString();
          ADD_PRINT(asmb);
        },
        "input",
        "2");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitLong(8);
          asmb->addLoadLitInt(0);
          asmb->addShiftRightLong();
          asmb->addConvLongString();
          ADD_PRINT(asmb);
        },
        "input",
        "8");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitLong(2);
          asmb->addLoadLitInt(-1);
          asmb->addShiftRightLong();
          asmb->addConvLongString();
          ADD_PRINT(asmb);
        },
        "input",
        "0");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitLong(1099511627776L);
          asmb->addLoadLitInt(40);
          asmb->addShiftRightLong();
          asmb->addConvLongString();
          ADD_PRINT(asmb);
        },
        "input",
        "1");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitLong(~0L);
          asmb->addLoadLitInt(1);
          asmb->addShiftRightLong();
          asmb->addConvLongString();
          ADD_PRINT(asmb);
        },
        "input",
        "9223372036854775807");
  }

  SECTION("And") {
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitLong(1);
          asmb->addLoadLitLong(1);
          asmb->addAndLong();
          asmb->addConvLongString();
          ADD_PRINT(asmb);
        },
        "input",
        "1");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitLong(10);
          asmb->addLoadLitLong(9);
          asmb->addAndLong();
          asmb->addConvLongString();
          ADD_PRINT(asmb);
        },
        "input",
        "8");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitLong(-42);
          asmb->addLoadLitLong(-42);
          asmb->addAndLong();
          asmb->addConvLongString();
          ADD_PRINT(asmb);
        },
        "input",
        "-42");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitLong(-42);
          asmb->addLoadLitLong(42);
          asmb->addAndLong();
          asmb->addConvLongString();
          ADD_PRINT(asmb);
        },
        "input",
        "2");
  }

  SECTION("Or") {
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitLong(1);
          asmb->addLoadLitLong(1);
          asmb->addOrLong();
          asmb->addConvLongString();
          ADD_PRINT(asmb);
        },
        "input",
        "1");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitLong(10);
          asmb->addLoadLitLong(9);
          asmb->addOrLong();
          asmb->addConvLongString();
          ADD_PRINT(asmb);
        },
        "input",
        "11");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitLong(-42);
          asmb->addLoadLitLong(-42);
          asmb->addOrLong();
          asmb->addConvLongString();
          ADD_PRINT(asmb);
        },
        "input",
        "-42");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitLong(-42);
          asmb->addLoadLitLong(42);
          asmb->addOrLong();
          asmb->addConvLongString();
          ADD_PRINT(asmb);
        },
        "input",
        "-2");
  }

  SECTION("Xor") {
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitLong(1);
          asmb->addLoadLitLong(1);
          asmb->addXorLong();
          asmb->addConvLongString();
          ADD_PRINT(asmb);
        },
        "input",
        "0");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitLong(10);
          asmb->addLoadLitLong(9);
          asmb->addXorLong();
          asmb->addConvLongString();
          ADD_PRINT(asmb);
        },
        "input",
        "3");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitLong(-42);
          asmb->addLoadLitLong(-42);
          asmb->addXorLong();
          asmb->addConvLongString();
          ADD_PRINT(asmb);
        },
        "input",
        "0");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitLong(-42);
          asmb->addLoadLitLong(42);
          asmb->addXorLong();
          asmb->addConvLongString();
          ADD_PRINT(asmb);
        },
        "input",
        "-4");
  }

  SECTION("Inv") {
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitLong(0);
          asmb->addInvLong();
          asmb->addConvLongString();
          ADD_PRINT(asmb);
        },
        "input",
        "-1");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitLong(-1);
          asmb->addInvLong();
          asmb->addConvLongString();
          ADD_PRINT(asmb);
        },
        "input",
        "0");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitLong(0xFFFF);
          asmb->addInvLong();
          asmb->addConvLongString();
          ADD_PRINT(asmb);
        },
        "input",
        "-65536");
  }
}

} // namespace vm
