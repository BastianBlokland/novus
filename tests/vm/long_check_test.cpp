#include "catch2/catch.hpp"
#include "helpers.hpp"

namespace vm {

TEST_CASE("Execute long checks", "[vm]") {

  SECTION("Equal") {
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitLong(4'200'000'000'000L);
          asmb->addLoadLitLong(-4'200'000'000'000L);
          asmb->addCheckEqLong();
          asmb->addConvBoolString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "false");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitLong(4'200'000'000'000L);
          asmb->addLoadLitLong(4'200'000'000'000L);
          asmb->addCheckEqLong();
          asmb->addConvBoolString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "true");
  }

  SECTION("Greater") {
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitLong(-1);
          asmb->addLoadLitLong(2);
          asmb->addCheckGtLong();
          asmb->addConvBoolString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "false");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitLong(2);
          asmb->addLoadLitLong(1);
          asmb->addCheckGtLong();
          asmb->addConvBoolString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "true");
  }

  SECTION("Less") {
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitLong(3);
          asmb->addLoadLitLong(2);
          asmb->addCheckLeLong();
          asmb->addConvBoolString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "false");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitLong(2);
          asmb->addLoadLitLong(3);
          asmb->addCheckLeLong();
          asmb->addConvBoolString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "true");
  }
}

} // namespace vm
