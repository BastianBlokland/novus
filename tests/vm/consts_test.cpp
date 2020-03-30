#include "catch2/catch.hpp"
#include "helpers.hpp"

namespace vm {

TEST_CASE("Execute constants", "[vm]") {

  CHECK_EXPR(
      [](novasm::Assembler* asmb) -> void {
        asmb->addStackAlloc(1);
        asmb->addLoadLitInt(42);
        asmb->addStackStore(0);

        asmb->addStackLoad(0);
        asmb->addConvIntString();
        ADD_PRINT(asmb);
      },
      "input",
      "42");

  CHECK_EXPR(
      [](novasm::Assembler* asmb) -> void {
        asmb->addStackAlloc(2);
        asmb->addLoadLitInt(42);
        asmb->addStackStore(0);

        asmb->addLoadLitInt(1337);
        asmb->addStackStore(1);

        asmb->addStackLoad(0);
        asmb->addConvIntString();
        ADD_PRINT(asmb);
        asmb->addPop();

        asmb->addLoadLitString(" ");
        ADD_PRINT(asmb);
        asmb->addPop();

        asmb->addStackLoad(1);
        asmb->addConvIntString();
        ADD_PRINT(asmb);
      },
      "input",
      "42 1337");
}

} // namespace vm
