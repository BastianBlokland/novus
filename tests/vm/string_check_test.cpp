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
          ADD_BOOL_TO_STRING(asmb);
          ADD_PRINT(asmb);
        },
        "input",
        "false");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitString("hello world");
          asmb->addLoadLitString("hello world");
          asmb->addCheckEqString();
          ADD_BOOL_TO_STRING(asmb);
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
          ADD_BOOL_TO_STRING(asmb);
          ADD_PRINT(asmb);
        },
        "input",
        "true");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitString("");
          asmb->addCheckStringEmtpy();
          ADD_BOOL_TO_STRING(asmb);
          ADD_PRINT(asmb);
        },
        "input",
        "true");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitString("Hello");
          asmb->addCheckStringEmtpy();
          ADD_BOOL_TO_STRING(asmb);
          ADD_PRINT(asmb);
        },
        "input",
        "false");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitString("");
          asmb->addLoadLitString("");
          asmb->addAddString();
          asmb->addCheckStringEmtpy();
          ADD_BOOL_TO_STRING(asmb);
          ADD_PRINT(asmb);
        },
        "input",
        "true");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitString("Hello");
          asmb->addLoadLitString("");
          asmb->addAddString();
          asmb->addCheckStringEmtpy();
          ADD_BOOL_TO_STRING(asmb);
          ADD_PRINT(asmb);
        },
        "input",
        "false");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitString("");
          asmb->addLoadLitInt('H');
          asmb->addAppendChar();
          asmb->addCheckStringEmtpy();
          ADD_BOOL_TO_STRING(asmb);
          ADD_PRINT(asmb);
        },
        "input",
        "false");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitString("");
          asmb->addLoadLitInt('\0');
          asmb->addAppendChar();
          asmb->addCheckStringEmtpy();
          ADD_BOOL_TO_STRING(asmb);
          ADD_PRINT(asmb);
        },
        "input",
        "false");
  }
}

} // namespace vm
