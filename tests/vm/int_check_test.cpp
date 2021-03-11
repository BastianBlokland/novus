#include "catch2/catch.hpp"
#include "helpers.hpp"

namespace vm {

TEST_CASE("[vm] Execute integer checks", "vm") {

  SECTION("Equal") {
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitInt(1);
          asmb->addLoadLitInt(-2);
          asmb->addCheckEqInt();
          ADD_BOOL_TO_STRING(asmb);
          ADD_PRINT(asmb);
        },
        "input",
        "false");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitInt(2);
          asmb->addLoadLitInt(2);
          asmb->addCheckEqInt();
          ADD_BOOL_TO_STRING(asmb);
          ADD_PRINT(asmb);
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
          ADD_BOOL_TO_STRING(asmb);
          ADD_PRINT(asmb);
        },
        "input",
        "false");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitInt(2);
          asmb->addLoadLitInt(1);
          asmb->addCheckGtInt();
          ADD_BOOL_TO_STRING(asmb);
          ADD_PRINT(asmb);
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
          ADD_BOOL_TO_STRING(asmb);
          ADD_PRINT(asmb);
        },
        "input",
        "false");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitInt(2);
          asmb->addLoadLitInt(3);
          asmb->addCheckLeInt();
          ADD_BOOL_TO_STRING(asmb);
          ADD_PRINT(asmb);
        },
        "input",
        "true");
  }
}

} // namespace vm
