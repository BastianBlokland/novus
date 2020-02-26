#include "catch2/catch.hpp"
#include "helpers.hpp"

namespace vm {

TEST_CASE("Execute string checks", "[vm]") {

  SECTION("Equal") {
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitString("hello");
          asmb->addLoadLitString("world");
          asmb->addCheckEqString();
          asmb->addConvBoolString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "false");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitString("hello world");
          asmb->addLoadLitString("hello world");
          asmb->addCheckEqString();
          asmb->addConvBoolString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
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
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "true");
  }
}

} // namespace vm
