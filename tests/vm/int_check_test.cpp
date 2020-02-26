#include "catch2/catch.hpp"
#include "helpers.hpp"

namespace vm {

TEST_CASE("Execute integer checks", "[vm]") {

  SECTION("Equal") {
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitInt(1);
          asmb->addLoadLitInt(-2);
          asmb->addCheckEqInt();
          asmb->addConvBoolString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "false");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitInt(2);
          asmb->addLoadLitInt(2);
          asmb->addCheckEqInt();
          asmb->addConvBoolString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "true");
  }

  SECTION("Greater") {
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitInt(-1);
          asmb->addLoadLitInt(2);
          asmb->addCheckGtInt();
          asmb->addConvBoolString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "false");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitInt(2);
          asmb->addLoadLitInt(1);
          asmb->addCheckGtInt();
          asmb->addConvBoolString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "true");
  }

  SECTION("Less") {
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitInt(3);
          asmb->addLoadLitInt(2);
          asmb->addCheckLeInt();
          asmb->addConvBoolString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "false");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitInt(2);
          asmb->addLoadLitInt(3);
          asmb->addCheckLeInt();
          asmb->addConvBoolString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "true");
  }
}

} // namespace vm
