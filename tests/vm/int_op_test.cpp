#include "catch2/catch.hpp"
#include "helpers.hpp"
#include "vm/exec_state.hpp"

namespace vm {

TEST_CASE("[vm] Execute integer operations", "vm") {

  SECTION("Add") {
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitInt(1);
          asmb->addLoadLitInt(2);
          asmb->addAddInt();
          asmb->addConvIntString();
          ADD_PRINT(asmb);
        },
        "input",
        "3");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitInt(2147483647);
          asmb->addLoadLitInt(1);
          asmb->addAddInt();
          asmb->addConvIntString();
          ADD_PRINT(asmb);
        },
        "input",
        "-2147483648");
  }

  SECTION("Substract") {
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitInt(10);
          asmb->addLoadLitInt(5);
          asmb->addSubInt();
          asmb->addConvIntString();
          ADD_PRINT(asmb);
        },
        "input",
        "5");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitInt(10);
          asmb->addLoadLitInt(20);
          asmb->addSubInt();
          asmb->addConvIntString();
          ADD_PRINT(asmb);
        },
        "input",
        "-10");
  }

  SECTION("Multiply") {
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitInt(3);
          asmb->addLoadLitInt(5);
          asmb->addMulInt();
          asmb->addConvIntString();
          ADD_PRINT(asmb);
        },
        "input",
        "15");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitInt(3);
          asmb->addLoadLitInt(-5);
          asmb->addMulInt();
          asmb->addConvIntString();
          ADD_PRINT(asmb);
        },
        "input",
        "-15");
  }

  SECTION("Divide") {
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitInt(33);
          asmb->addLoadLitInt(4);
          asmb->addDivInt();
          asmb->addConvIntString();
          ADD_PRINT(asmb);
        },
        "input",
        "8");
    CHECK_EXPR_RESULTCODE(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitInt(1);
          asmb->addLoadLitInt(0);
          asmb->addDivInt();
          asmb->addConvIntString();
          ADD_PRINT(asmb);
        },
        "input",
        ExecState::DivByZero);
  }

  SECTION("Remainder") {
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitInt(33);
          asmb->addLoadLitInt(4);
          asmb->addRemInt();
          asmb->addConvIntString();
          ADD_PRINT(asmb);
        },
        "input",
        "1");
    CHECK_EXPR_RESULTCODE(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitInt(1);
          asmb->addLoadLitInt(0);
          asmb->addRemInt();
          asmb->addConvIntString();
          ADD_PRINT(asmb);
        },
        "input",
        ExecState::DivByZero);
  }

  SECTION("Negate") {
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitInt(42);
          asmb->addNegInt();
          asmb->addConvIntString();
          ADD_PRINT(asmb);
        },
        "input",
        "-42");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitInt(0);
          asmb->addNegInt();
          asmb->addConvIntString();
          ADD_PRINT(asmb);
        },
        "input",
        "0");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitInt(42);
          asmb->addNegInt();
          asmb->addNegInt();
          asmb->addConvIntString();
          ADD_PRINT(asmb);
        },
        "input",
        "42");
  }

  SECTION("Logic invert") {
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitInt(0);
          asmb->addLogicInvInt();
          asmb->addConvIntString();
          ADD_PRINT(asmb);
        },
        "input",
        "1");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitInt(1);
          asmb->addLogicInvInt();
          asmb->addConvIntString();
          ADD_PRINT(asmb);
        },
        "input",
        "0");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitInt(2);
          asmb->addLogicInvInt();
          asmb->addConvIntString();
          ADD_PRINT(asmb);
        },
        "input",
        "0");
  }

  SECTION("Shift left") {
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitInt(1);
          asmb->addLoadLitInt(1);
          asmb->addShiftLeftInt();
          asmb->addConvIntString();
          ADD_PRINT(asmb);
        },
        "input",
        "2");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitInt(2);
          asmb->addLoadLitInt(2);
          asmb->addShiftLeftInt();
          asmb->addConvIntString();
          ADD_PRINT(asmb);
        },
        "input",
        "8");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitInt(4);
          asmb->addLoadLitInt(0);
          asmb->addShiftLeftInt();
          asmb->addConvIntString();
          ADD_PRINT(asmb);
        },
        "input",
        "4");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitInt(2);
          asmb->addLoadLitInt(-2);
          asmb->addShiftLeftInt();
          asmb->addConvIntString();
          ADD_PRINT(asmb);
        },
        "input",
        "-2147483648");
  }

  SECTION("Shift right") {
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitInt(1);
          asmb->addLoadLitInt(1);
          asmb->addShiftRightInt();
          asmb->addConvIntString();
          ADD_PRINT(asmb);
        },
        "input",
        "0");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitInt(8);
          asmb->addLoadLitInt(2);
          asmb->addShiftRightInt();
          asmb->addConvIntString();
          ADD_PRINT(asmb);
        },
        "input",
        "2");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitInt(8);
          asmb->addLoadLitInt(0);
          asmb->addShiftRightInt();
          asmb->addConvIntString();
          ADD_PRINT(asmb);
        },
        "input",
        "8");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitInt(2);
          asmb->addLoadLitInt(-1);
          asmb->addShiftRightInt();
          asmb->addConvIntString();
          ADD_PRINT(asmb);
        },
        "input",
        "0");
  }

  SECTION("And") {
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitInt(1);
          asmb->addLoadLitInt(1);
          asmb->addAndInt();
          asmb->addConvIntString();
          ADD_PRINT(asmb);
        },
        "input",
        "1");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitInt(10);
          asmb->addLoadLitInt(9);
          asmb->addAndInt();
          asmb->addConvIntString();
          ADD_PRINT(asmb);
        },
        "input",
        "8");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitInt(-42);
          asmb->addLoadLitInt(-42);
          asmb->addAndInt();
          asmb->addConvIntString();
          ADD_PRINT(asmb);
        },
        "input",
        "-42");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitInt(-42);
          asmb->addLoadLitInt(42);
          asmb->addAndInt();
          asmb->addConvIntString();
          ADD_PRINT(asmb);
        },
        "input",
        "2");
  }

  SECTION("Or") {
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitInt(1);
          asmb->addLoadLitInt(1);
          asmb->addOrInt();
          asmb->addConvIntString();
          ADD_PRINT(asmb);
        },
        "input",
        "1");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitInt(10);
          asmb->addLoadLitInt(9);
          asmb->addOrInt();
          asmb->addConvIntString();
          ADD_PRINT(asmb);
        },
        "input",
        "11");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitInt(-42);
          asmb->addLoadLitInt(-42);
          asmb->addOrInt();
          asmb->addConvIntString();
          ADD_PRINT(asmb);
        },
        "input",
        "-42");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitInt(-42);
          asmb->addLoadLitInt(42);
          asmb->addOrInt();
          asmb->addConvIntString();
          ADD_PRINT(asmb);
        },
        "input",
        "-2");
  }

  SECTION("Xor") {
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitInt(1);
          asmb->addLoadLitInt(1);
          asmb->addXorInt();
          asmb->addConvIntString();
          ADD_PRINT(asmb);
        },
        "input",
        "0");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitInt(10);
          asmb->addLoadLitInt(9);
          asmb->addXorInt();
          asmb->addConvIntString();
          ADD_PRINT(asmb);
        },
        "input",
        "3");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitInt(-42);
          asmb->addLoadLitInt(-42);
          asmb->addXorInt();
          asmb->addConvIntString();
          ADD_PRINT(asmb);
        },
        "input",
        "0");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitInt(-42);
          asmb->addLoadLitInt(42);
          asmb->addXorInt();
          asmb->addConvIntString();
          ADD_PRINT(asmb);
        },
        "input",
        "-4");
  }

  SECTION("Inv") {
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitInt(0);
          asmb->addInvInt();
          asmb->addConvIntString();
          ADD_PRINT(asmb);
        },
        "input",
        "-1");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitInt(-1);
          asmb->addInvInt();
          asmb->addConvIntString();
          ADD_PRINT(asmb);
        },
        "input",
        "0");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitInt(0xFFFF);
          asmb->addInvInt();
          asmb->addConvIntString();
          ADD_PRINT(asmb);
        },
        "input",
        "-65536");
  }
}
} // namespace vm
