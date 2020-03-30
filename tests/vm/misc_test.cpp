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
          ADD_PRINT(asmb);
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
          ADD_PRINT(asmb);
          asmb->addRet();

          asmb->setEntrypoint("start");
        },
        "input",
        "42");
  }

  SECTION("Swap") {
    CHECK_PROG(
        [](novasm::Assembler* asmb) -> void {
          asmb->label("start");
          asmb->addLoadLitInt(5);
          asmb->addLoadLitInt(10);
          asmb->addSwap();
          asmb->addDivInt();

          asmb->addConvIntString();
          ADD_PRINT(asmb);
          asmb->addRet();

          asmb->setEntrypoint("start");
        },
        "input",
        "2");
  }
}

} // namespace vm
