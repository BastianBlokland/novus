#include "catch2/catch.hpp"
#include "helpers.hpp"

namespace vm {

TEST_CASE("Execute instruction pointer checks", "[vm]") {

  SECTION("Equal") {
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitIp("label");
          asmb->addLoadLitIp("label");

          asmb->label("label");
          asmb->addCheckEqIp();
          asmb->addConvBoolString();
          ADD_PRINT(asmb);
        },
        "input",
        "true");

    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitIp("label1");
          asmb->addLoadLitIp("label2");

          asmb->label("label1");
          asmb->addCheckEqIp();

          asmb->label("label2");
          asmb->addConvBoolString();
          ADD_PRINT(asmb);
        },
        "input",
        "false");

    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitIp("label");
          asmb->addLoadLitIp("label");

          asmb->label("label");
          asmb->addCheckEqCallDynTgt();
          asmb->addConvBoolString();
          ADD_PRINT(asmb);
        },
        "input",
        "true");

    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitIp("label1");
          asmb->addLoadLitIp("label2");

          asmb->label("label1");
          asmb->addCheckEqCallDynTgt();

          asmb->label("label2");
          asmb->addConvBoolString();
          ADD_PRINT(asmb);
        },
        "input",
        "false");

    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          // Make a closure struct.
          asmb->addLoadLitInt(42);
          asmb->addLoadLitIp("label");
          asmb->addMakeStruct(2);

          // Make a closure struct.
          asmb->addLoadLitInt(1337);
          asmb->addLoadLitInt(42);
          asmb->addLoadLitIp("label");
          asmb->addMakeStruct(3);

          asmb->label("label");
          asmb->addCheckEqCallDynTgt();

          asmb->addConvBoolString();
          ADD_PRINT(asmb);
        },
        "input",
        "true");

    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          // Make a closure struct.
          asmb->addLoadLitInt(42);
          asmb->addLoadLitIp("label1");
          asmb->addMakeStruct(2);

          // Make a closure struct.
          asmb->addLoadLitInt(1337);
          asmb->addLoadLitInt(42);
          asmb->addLoadLitIp("label2");
          asmb->addMakeStruct(3);

          asmb->label("label1");
          asmb->addCheckEqCallDynTgt();

          asmb->label("label2");
          asmb->addConvBoolString();
          ADD_PRINT(asmb);
        },
        "input",
        "false");

    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          // Make a closure struct.
          asmb->addLoadLitInt(42);
          asmb->addLoadLitIp("label");
          asmb->addMakeStruct(2);

          // Make a raw instruction pointer.
          asmb->addLoadLitIp("label");

          asmb->label("label");
          asmb->addCheckEqCallDynTgt();

          asmb->addConvBoolString();
          ADD_PRINT(asmb);
        },
        "input",
        "true");

    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          // Make a closure struct.
          asmb->addLoadLitInt(42);
          asmb->addLoadLitIp("label1");
          asmb->addMakeStruct(2);

          // Make a raw instruction pointer.
          asmb->addLoadLitIp("label2");

          asmb->label("label1");
          asmb->addCheckEqCallDynTgt();

          asmb->label("label2");
          asmb->addConvBoolString();
          ADD_PRINT(asmb);
        },
        "input",
        "false");
  }
}

} // namespace vm
