#include "catch2/catch.hpp"
#include "helpers.hpp"

namespace backend {

TEST_CASE("Generating assembly for group expressions", "[backend]") {

  CHECK_EXPR("x = 42; x", [](novasm::Assembler* asmb) -> void {
    asmb->addStackAlloc(1);

    asmb->addLoadLitInt(42);
    asmb->addDup();
    asmb->addStackStore(0);

    asmb->addPop();
    asmb->addStackLoad(0);
  });

  CHECK_EXPR("1; 2; 3", [](novasm::Assembler* asmb) -> void {
    asmb->addLoadLitInt(1);
    asmb->addPop();

    asmb->addLoadLitInt(2);
    asmb->addPop();

    asmb->addLoadLitInt(3);
  });
}

} // namespace backend
