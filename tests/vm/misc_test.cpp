#include "catch2/catch.hpp"
#include "helpers.hpp"

namespace vm {

TEST_CASE("Execute miscellaneous instructions", "[vm]") {

  SECTION("Dup") {
    CHECK_PROG(
        [](novasm::Assembler* asmb) -> void {
          asmb->label("start");
          asmb->addLoadLitInt(42);
          asmb->addDup();

          asmb->addAddInt();
          asmb->addConvIntString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
          asmb->addRet();

          asmb->setEntrypoint("start");
        },
        "input",
        "84");
  }

  SECTION("Pop") {
    CHECK_PROG(
        [](novasm::Assembler* asmb) -> void {
          asmb->label("start");
          asmb->addLoadLitInt(42);
          asmb->addLoadLitInt(1337);
          asmb->addPop();

          asmb->addConvIntString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
          asmb->addRet();

          asmb->setEntrypoint("start");
        },
        "input",
        "42");
  }
}

} // namespace vm
