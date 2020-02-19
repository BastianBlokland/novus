#include "catch2/catch.hpp"
#include "helpers.hpp"
#include "vm/exec_state.hpp"

namespace vm {

TEST_CASE("Execute long operations", "[vm]") {

  SECTION("Add") {
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitLong(1);
          builder->addLoadLitLong(2);
          builder->addAddLong();
          builder->addConvLongString();
          builder->addPCall(vm::PCallCode::ConWriteString);
        },
        "input",
        "3");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitLong(9223372036854775807L);
          builder->addLoadLitLong(1);
          builder->addAddLong();
          builder->addConvLongString();
          builder->addPCall(vm::PCallCode::ConWriteString);
        },
        "input",
        "-9223372036854775808");
  }

  SECTION("Substract") {
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitLong(10);
          builder->addLoadLitLong(5);
          builder->addSubLong();
          builder->addConvLongString();
          builder->addPCall(vm::PCallCode::ConWriteString);
        },
        "input",
        "5");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitLong(10);
          builder->addLoadLitLong(20);
          builder->addSubLong();
          builder->addConvLongString();
          builder->addPCall(vm::PCallCode::ConWriteString);
        },
        "input",
        "-10");
  }

  SECTION("Multiply") {
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitLong(3);
          builder->addLoadLitLong(5);
          builder->addMulLong();
          builder->addConvLongString();
          builder->addPCall(vm::PCallCode::ConWriteString);
        },
        "input",
        "15");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitLong(3);
          builder->addLoadLitLong(-5);
          builder->addMulLong();
          builder->addConvLongString();
          builder->addPCall(vm::PCallCode::ConWriteString);
        },
        "input",
        "-15");
  }

  SECTION("Divide") {
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitLong(33);
          builder->addLoadLitLong(4);
          builder->addDivLong();
          builder->addConvLongString();
          builder->addPCall(vm::PCallCode::ConWriteString);
        },
        "input",
        "8");
    CHECK_EXPR_RESULTCODE(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitLong(1);
          builder->addLoadLitLong(0);
          builder->addDivLong();
          builder->addConvLongString();
          builder->addPCall(vm::PCallCode::ConWriteString);
        },
        "input",
        ExecState::DivByZero);
  }

  SECTION("Remainder") {
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitLong(33);
          builder->addLoadLitLong(4);
          builder->addRemLong();
          builder->addConvLongString();
          builder->addPCall(vm::PCallCode::ConWriteString);
        },
        "input",
        "1");
    CHECK_EXPR_RESULTCODE(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitLong(1);
          builder->addLoadLitLong(0);
          builder->addRemLong();
          builder->addConvLongString();
          builder->addPCall(vm::PCallCode::ConWriteString);
        },
        "input",
        ExecState::DivByZero);
  }

  SECTION("Negate") {
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitLong(42);
          builder->addNegLong();
          builder->addConvLongString();
          builder->addPCall(vm::PCallCode::ConWriteString);
        },
        "input",
        "-42");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitLong(0);
          builder->addNegLong();
          builder->addConvLongString();
          builder->addPCall(vm::PCallCode::ConWriteString);
        },
        "input",
        "0");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitLong(42);
          builder->addNegLong();
          builder->addNegLong();
          builder->addConvLongString();
          builder->addPCall(vm::PCallCode::ConWriteString);
        },
        "input",
        "42");
  }
}
} // namespace vm
