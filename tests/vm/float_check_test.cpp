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
          ADD_BOOL_TO_STRING(asmb);
          ADD_PRINT(asmb);
        },
        "input",
        "false");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitFloat(0.1337F); // NOLINT: Magic numbers
          asmb->addLoadLitFloat(0.1337F); // NOLINT: Magic numbers
          asmb->addCheckEqFloat();
          ADD_BOOL_TO_STRING(asmb);
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
          ADD_BOOL_TO_STRING(asmb);
          ADD_PRINT(asmb);
        },
        "input",
        "false");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitFloat(0.2337F); // NOLINT: Magic numbers
          asmb->addLoadLitFloat(0.1337F); // NOLINT: Magic numbers
          asmb->addCheckGtFloat();
          ADD_BOOL_TO_STRING(asmb);
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
          ADD_BOOL_TO_STRING(asmb);
          ADD_PRINT(asmb);
        },
        "input",
        "false");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitFloat(0.1337F); // NOLINT: Magic numbers
          asmb->addLoadLitFloat(0.2337F); // NOLINT: Magic numbers
          asmb->addCheckLeFloat();
          ADD_BOOL_TO_STRING(asmb);
          ADD_PRINT(asmb);
        },
        "input",
        "true");
  }
}

} // namespace vm
