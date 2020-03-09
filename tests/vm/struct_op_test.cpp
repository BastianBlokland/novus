#include "catch2/catch.hpp"
#include "helpers.hpp"

namespace vm {

TEST_CASE("Execute struct operations", "[vm]") {

  SECTION("Create struct") {
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          // Create a struct.
          asmb->addLoadLitString("hello ");
          asmb->addLoadLitString("world");
          asmb->addMakeStruct(2);

          // Assert its not null.
          asmb->addCheckStructNull();
          asmb->addLogicInvInt();
          asmb->addLoadLitString("Struct null");
          asmb->addPCall(novasm::PCallCode::Assert);
        },
        "input");
  }

  SECTION("Create null struct") {
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          // Create a null struct.
          asmb->addMakeNullStruct();

          // Assert its null.
          asmb->addCheckStructNull();
          asmb->addLoadLitString("Struct not null");
          asmb->addPCall(novasm::PCallCode::Assert);
        },
        "input");
  }

  SECTION("Load field") {
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addStackAlloc(1);

          asmb->addLoadLitString("hello ");
          asmb->addLoadLitString("world");
          asmb->addMakeStruct(2);
          asmb->addStackStore(0);

          asmb->addStackLoad(0);
          asmb->addLoadStructField(0);

          asmb->addStackLoad(0);
          asmb->addLoadStructField(1);

          asmb->addAddString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
          asmb->addPop();
        },
        "input",
        "hello world");
  }
}

} // namespace vm
