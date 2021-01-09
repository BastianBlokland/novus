#include "catch2/catch.hpp"
#include "helpers.hpp"

namespace vm {

TEST_CASE("[vm] Execute float checks", "vm") {

  SECTION("Equal") {
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitFloat(0.1337F);  // NOLINT: Magic numbers
          asmb->addLoadLitFloat(-0.1337F); // NOLINT: Magic numbers
          asmb->addCheckEqFloat();
          asmb->addConvBoolString();
          ADD_PRINT(asmb);
        },
        "input",
        "false");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitFloat(0.1337F); // NOLINT: Magic numbers
          asmb->addLoadLitFloat(0.1337F); // NOLINT: Magic numbers
          asmb->addCheckEqFloat();
          asmb->addConvBoolString();
          ADD_PRINT(asmb);
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
          ADD_PRINT(asmb);
        },
        "input",
        "false");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitFloat(0.2337F); // NOLINT: Magic numbers
          asmb->addLoadLitFloat(0.1337F); // NOLINT: Magic numbers
          asmb->addCheckGtFloat();
          asmb->addConvBoolString();
          ADD_PRINT(asmb);
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
          ADD_PRINT(asmb);
        },
        "input",
        "false");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitFloat(0.1337F); // NOLINT: Magic numbers
          asmb->addLoadLitFloat(0.2337F); // NOLINT: Magic numbers
          asmb->addCheckLeFloat();
          asmb->addConvBoolString();
          ADD_PRINT(asmb);
        },
        "input",
        "true");
  }
}

} // namespace vm
