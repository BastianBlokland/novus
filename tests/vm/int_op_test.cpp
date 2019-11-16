#include "catch2/catch.hpp"
#include "helpers.hpp"
#include "vm/exceptions/div_by_zero.hpp"

namespace vm {

TEST_CASE("Integer operations", "[vm]") {

  SECTION("Add") {
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitInt(1);
          builder->addLoadLitInt(2);
          builder->addAddInt();
          builder->addPrintInt();
        },
        "3");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitInt(2147483647);
          builder->addLoadLitInt(1);
          builder->addAddInt();
          builder->addPrintInt();
        },
        "-2147483648");
  }

  SECTION("Substract") {
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitInt(10);
          builder->addLoadLitInt(5);
          builder->addSubInt();
          builder->addPrintInt();
        },
        "5");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitInt(10);
          builder->addLoadLitInt(20);
          builder->addSubInt();
          builder->addPrintInt();
        },
        "-10");
  }

  SECTION("Multiply") {
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitInt(3);
          builder->addLoadLitInt(5);
          builder->addMulInt();
          builder->addPrintInt();
        },
        "15");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitInt(3);
          builder->addLoadLitInt(-5);
          builder->addMulInt();
          builder->addPrintInt();
        },
        "-15");
  }

  SECTION("Divide") {
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitInt(33);
          builder->addLoadLitInt(4);
          builder->addDivInt();
          builder->addPrintInt();
        },
        "8");
    CHECK_EXPR_THROWS(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitInt(1);
          builder->addLoadLitInt(0);
          builder->addDivInt();
          builder->addPrintInt();
        },
        vm::exceptions::DivByZero);
  }

  SECTION("Negate") {
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitInt(42);
          builder->addNegInt();
          builder->addPrintInt();
        },
        "-42");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitInt(0);
          builder->addNegInt();
          builder->addPrintInt();
        },
        "0");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitInt(42);
          builder->addNegInt();
          builder->addNegInt();
          builder->addPrintInt();
        },
        "42");
  }

  SECTION("Logic invert") {
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitInt(0);
          builder->addLogicInvInt();
          builder->addPrintInt();
        },
        "1");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitInt(1);
          builder->addLogicInvInt();
          builder->addPrintInt();
        },
        "0");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitInt(2);
          builder->addLogicInvInt();
          builder->addPrintInt();
        },
        "0");
  }
}

} // namespace vm
