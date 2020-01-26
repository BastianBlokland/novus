#include "catch2/catch.hpp"
#include "helpers.hpp"
#include "vm/exceptions/div_by_zero.hpp"

namespace vm {

TEST_CASE("Execute integer operations", "[vm]") {

  SECTION("Add") {
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitInt(1);
          builder->addLoadLitInt(2);
          builder->addAddInt();
          builder->addConvIntString();
          builder->addPCall(vm::PCallCode::PrintString);
          builder->addPop();
        },
        "input",
        "3");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitInt(2147483647);
          builder->addLoadLitInt(1);
          builder->addAddInt();
          builder->addConvIntString();
          builder->addPCall(vm::PCallCode::PrintString);
          builder->addPop();
        },
        "input",
        "-2147483648");
  }

  SECTION("Substract") {
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitInt(10);
          builder->addLoadLitInt(5);
          builder->addSubInt();
          builder->addConvIntString();
          builder->addPCall(vm::PCallCode::PrintString);
          builder->addPop();
        },
        "input",
        "5");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitInt(10);
          builder->addLoadLitInt(20);
          builder->addSubInt();
          builder->addConvIntString();
          builder->addPCall(vm::PCallCode::PrintString);
          builder->addPop();
        },
        "input",
        "-10");
  }

  SECTION("Multiply") {
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitInt(3);
          builder->addLoadLitInt(5);
          builder->addMulInt();
          builder->addConvIntString();
          builder->addPCall(vm::PCallCode::PrintString);
          builder->addPop();
        },
        "input",
        "15");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitInt(3);
          builder->addLoadLitInt(-5);
          builder->addMulInt();
          builder->addConvIntString();
          builder->addPCall(vm::PCallCode::PrintString);
          builder->addPop();
        },
        "input",
        "-15");
  }

  SECTION("Divide") {
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitInt(33);
          builder->addLoadLitInt(4);
          builder->addDivInt();
          builder->addConvIntString();
          builder->addPCall(vm::PCallCode::PrintString);
          builder->addPop();
        },
        "input",
        "8");
    CHECK_EXPR_THROWS(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitInt(1);
          builder->addLoadLitInt(0);
          builder->addDivInt();
          builder->addConvIntString();
          builder->addPCall(vm::PCallCode::PrintString);
          builder->addPop();
        },
        "input",
        vm::exceptions::DivByZero);
  }

  SECTION("Remainder") {
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitInt(33);
          builder->addLoadLitInt(4);
          builder->addRemInt();
          builder->addConvIntString();
          builder->addPCall(vm::PCallCode::PrintString);
          builder->addPop();
        },
        "input",
        "1");
    CHECK_EXPR_THROWS(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitInt(1);
          builder->addLoadLitInt(0);
          builder->addRemInt();
          builder->addConvIntString();
          builder->addPCall(vm::PCallCode::PrintString);
          builder->addPop();
        },
        "input",
        vm::exceptions::DivByZero);
  }

  SECTION("Negate") {
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitInt(42);
          builder->addNegInt();
          builder->addConvIntString();
          builder->addPCall(vm::PCallCode::PrintString);
          builder->addPop();
        },
        "input",
        "-42");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitInt(0);
          builder->addNegInt();
          builder->addConvIntString();
          builder->addPCall(vm::PCallCode::PrintString);
          builder->addPop();
        },
        "input",
        "0");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitInt(42);
          builder->addNegInt();
          builder->addNegInt();
          builder->addConvIntString();
          builder->addPCall(vm::PCallCode::PrintString);
          builder->addPop();
        },
        "input",
        "42");
  }

  SECTION("Logic invert") {
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitInt(0);
          builder->addLogicInvInt();
          builder->addConvIntString();
          builder->addPCall(vm::PCallCode::PrintString);
          builder->addPop();
        },
        "input",
        "1");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitInt(1);
          builder->addLogicInvInt();
          builder->addConvIntString();
          builder->addPCall(vm::PCallCode::PrintString);
          builder->addPop();
        },
        "input",
        "0");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitInt(2);
          builder->addLogicInvInt();
          builder->addConvIntString();
          builder->addPCall(vm::PCallCode::PrintString);
          builder->addPop();
        },
        "input",
        "0");
  }

  SECTION("Shift left") {
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitInt(1);
          builder->addLoadLitInt(1);
          builder->addShiftLeftInt();
          builder->addConvIntString();
          builder->addPCall(vm::PCallCode::PrintString);
          builder->addPop();
        },
        "input",
        "2");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitInt(2);
          builder->addLoadLitInt(2);
          builder->addShiftLeftInt();
          builder->addConvIntString();
          builder->addPCall(vm::PCallCode::PrintString);
          builder->addPop();
        },
        "input",
        "8");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitInt(4);
          builder->addLoadLitInt(0);
          builder->addShiftLeftInt();
          builder->addConvIntString();
          builder->addPCall(vm::PCallCode::PrintString);
          builder->addPop();
        },
        "input",
        "4");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitInt(2);
          builder->addLoadLitInt(-2);
          builder->addShiftLeftInt();
          builder->addConvIntString();
          builder->addPCall(vm::PCallCode::PrintString);
          builder->addPop();
        },
        "input",
        "-2147483648");
  }

  SECTION("Shift right") {
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitInt(1);
          builder->addLoadLitInt(1);
          builder->addShiftRightInt();
          builder->addConvIntString();
          builder->addPCall(vm::PCallCode::PrintString);
          builder->addPop();
        },
        "input",
        "0");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitInt(8);
          builder->addLoadLitInt(2);
          builder->addShiftRightInt();
          builder->addConvIntString();
          builder->addPCall(vm::PCallCode::PrintString);
          builder->addPop();
        },
        "input",
        "2");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitInt(8);
          builder->addLoadLitInt(0);
          builder->addShiftRightInt();
          builder->addConvIntString();
          builder->addPCall(vm::PCallCode::PrintString);
          builder->addPop();
        },
        "input",
        "8");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitInt(2);
          builder->addLoadLitInt(-1);
          builder->addShiftRightInt();
          builder->addConvIntString();
          builder->addPCall(vm::PCallCode::PrintString);
          builder->addPop();
        },
        "input",
        "0");
  }

  SECTION("And") {
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitInt(1);
          builder->addLoadLitInt(1);
          builder->addAndInt();
          builder->addConvIntString();
          builder->addPCall(vm::PCallCode::PrintString);
          builder->addPop();
        },
        "input",
        "1");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitInt(10);
          builder->addLoadLitInt(9);
          builder->addAndInt();
          builder->addConvIntString();
          builder->addPCall(vm::PCallCode::PrintString);
          builder->addPop();
        },
        "input",
        "8");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitInt(-42);
          builder->addLoadLitInt(-42);
          builder->addAndInt();
          builder->addConvIntString();
          builder->addPCall(vm::PCallCode::PrintString);
          builder->addPop();
        },
        "input",
        "-42");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitInt(-42);
          builder->addLoadLitInt(42);
          builder->addAndInt();
          builder->addConvIntString();
          builder->addPCall(vm::PCallCode::PrintString);
          builder->addPop();
        },
        "input",
        "2");
  }

  SECTION("Or") {
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitInt(1);
          builder->addLoadLitInt(1);
          builder->addOrInt();
          builder->addConvIntString();
          builder->addPCall(vm::PCallCode::PrintString);
          builder->addPop();
        },
        "input",
        "1");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitInt(10);
          builder->addLoadLitInt(9);
          builder->addOrInt();
          builder->addConvIntString();
          builder->addPCall(vm::PCallCode::PrintString);
          builder->addPop();
        },
        "input",
        "11");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitInt(-42);
          builder->addLoadLitInt(-42);
          builder->addOrInt();
          builder->addConvIntString();
          builder->addPCall(vm::PCallCode::PrintString);
          builder->addPop();
        },
        "input",
        "-42");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitInt(-42);
          builder->addLoadLitInt(42);
          builder->addOrInt();
          builder->addConvIntString();
          builder->addPCall(vm::PCallCode::PrintString);
          builder->addPop();
        },
        "input",
        "-2");
  }

  SECTION("Xor") {
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitInt(1);
          builder->addLoadLitInt(1);
          builder->addXorInt();
          builder->addConvIntString();
          builder->addPCall(vm::PCallCode::PrintString);
          builder->addPop();
        },
        "input",
        "0");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitInt(10);
          builder->addLoadLitInt(9);
          builder->addXorInt();
          builder->addConvIntString();
          builder->addPCall(vm::PCallCode::PrintString);
          builder->addPop();
        },
        "input",
        "3");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitInt(-42);
          builder->addLoadLitInt(-42);
          builder->addXorInt();
          builder->addConvIntString();
          builder->addPCall(vm::PCallCode::PrintString);
          builder->addPop();
        },
        "input",
        "0");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitInt(-42);
          builder->addLoadLitInt(42);
          builder->addXorInt();
          builder->addConvIntString();
          builder->addPCall(vm::PCallCode::PrintString);
          builder->addPop();
        },
        "input",
        "-4");
  }

  SECTION("Inv") {
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitInt(0);
          builder->addInvInt();
          builder->addConvIntString();
          builder->addPCall(vm::PCallCode::PrintString);
          builder->addPop();
        },
        "input",
        "-1");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitInt(-1);
          builder->addInvInt();
          builder->addConvIntString();
          builder->addPCall(vm::PCallCode::PrintString);
          builder->addPop();
        },
        "input",
        "0");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitInt(0xFFFF);
          builder->addInvInt();
          builder->addConvIntString();
          builder->addPCall(vm::PCallCode::PrintString);
          builder->addPop();
        },
        "input",
        "-65536");
  }
}
} // namespace vm
