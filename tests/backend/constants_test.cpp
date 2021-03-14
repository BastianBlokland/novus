#include "catch2/catch.hpp"
#include "helpers.hpp"

namespace backend {

TEST_CASE("[backend] Generate assembly for storing and loading constants", "backend") {

  CHECK_EXPR("x = 42; x", [](novasm::Assembler* asmb) -> void {
    asmb->addStackAlloc(1);

    asmb->addLoadLitInt(42);
    asmb->addStackStore(0);

    asmb->addStackLoad(0);
  });

  CHECK_EXPR("x = y = 42; y", [](novasm::Assembler* asmb) -> void {
    asmb->addStackAlloc(2);

    asmb->addLoadLitInt(42);
    asmb->addDup();
    asmb->addStackStore(0);

    asmb->addStackStore(1);

    asmb->addStackLoad(0);
  });

  CHECK_EXPR_INT(
      "x = 42; y = 1337; intrinsic{int_add_int}(x, y)", [](novasm::Assembler* asmb) -> void {
        asmb->addStackAlloc(2);

        asmb->addLoadLitInt(42);
        asmb->addStackStore(0);

        asmb->addLoadLitInt(1337);
        asmb->addStackStore(1);

        asmb->addStackLoad(0);
        asmb->addStackLoad(1);
        asmb->addAddInt();
      });

  CHECK_EXPR("x = y = 42; y", [](novasm::Assembler* asmb) -> void {
    asmb->addStackAlloc(2);

    asmb->addLoadLitInt(42);
    asmb->addDup();
    asmb->addStackStore(0);

    asmb->addStackStore(1);

    asmb->addStackLoad(0);
  });
}

} // namespace backend
