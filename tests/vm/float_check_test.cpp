#include "catch2/catch.hpp"
#include "helpers.hpp"

namespace vm {

TEST_CASE("Execute float checks", "[vm]") {

  SECTION("Equal") {
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitFloat(0.1337F);  // NOLINT: Magic numbers
          asmb->addLoadLitFloat(-0.1337F); // NOLINT: Magic numbers
          asmb->addCheckEqFloat();
          asmb->addConvBoolString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "false");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitFloat(0.1337F); // NOLINT: Magic numbers
          asmb->addLoadLitFloat(0.1337F); // NOLINT: Magic numbers
          asmb->addCheckEqFloat();
          asmb->addConvBoolString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "true");
  }

  SECTION("Greater") {
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitFloat(-0.1337F); // NOLINT: Magic numbers
          asmb->addLoadLitFloat(0.1337F);  // NOLINT: Magic numbers
          asmb->addCheckGtFloat();
          asmb->addConvBoolString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "false");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitFloat(0.2337F); // NOLINT: Magic numbers
          asmb->addLoadLitFloat(0.1337F); // NOLINT: Magic numbers
          asmb->addCheckGtFloat();
          asmb->addConvBoolString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "true");
  }

  SECTION("Less") {
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitFloat(0.2337F); // NOLINT: Magic numbers
          asmb->addLoadLitFloat(0.1337F); // NOLINT: Magic numbers
          asmb->addCheckLeFloat();
          asmb->addConvBoolString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "false");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitFloat(0.1337F); // NOLINT: Magic numbers
          asmb->addLoadLitFloat(0.2337F); // NOLINT: Magic numbers
          asmb->addCheckLeFloat();
          asmb->addConvBoolString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "true");
  }
}

} // namespace vm
