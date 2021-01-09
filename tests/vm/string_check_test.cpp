#include "catch2/catch.hpp"
#include "helpers.hpp"

namespace vm {

TEST_CASE("[vm] Execute string checks", "vm") {

  SECTION("Equal") {
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitString("hello");
          asmb->addLoadLitString("world");
          asmb->addCheckEqString();
          asmb->addConvBoolString();
          ADD_PRINT(asmb);
        },
        "input",
        "false");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitString("hello world");
          asmb->addLoadLitString("hello world");
          asmb->addCheckEqString();
          asmb->addConvBoolString();
          ADD_PRINT(asmb);
        },
        "input",
        "true");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitString("hello world");
          asmb->addLoadLitString("hello ");
          asmb->addLoadLitString("world");
          asmb->addAddString();
          asmb->addCheckEqString();
          asmb->addConvBoolString();
          ADD_PRINT(asmb);
        },
        "input",
        "true");
  }
}

} // namespace vm
