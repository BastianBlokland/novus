#include "catch2/catch.hpp"
#include "helpers.hpp"

namespace backend {

TEST_CASE("[backend] Generate assembly for self call expressions", "backend") {

  SECTION("Function") {
    CHECK_PROG("fun f(int i) i < 0 ? self(0) : i ", [](novasm::Assembler* asmb) -> void {
      // Function.
      asmb->label("func");
      asmb->addStackLoad(0); // Load arg 'i'.
      asmb->addLoadLitInt(0);
      asmb->addCheckLeInt();
      asmb->addJumpIf("less");

      asmb->addStackLoad(0); // Load arg 'i'.
      asmb->addJump("end");

      asmb->label("less");
      asmb->addLoadLitInt(0);
      asmb->addCall("func", 1, novasm::CallMode::Tail);

      asmb->label("end");
      asmb->addRet();

      // Entry point.
      asmb->setEntrypoint("entry");
      asmb->label("entry");
      asmb->addRet();
    });
  }

  SECTION("Closure") {
    CHECK_PROG(
        "fun f(int i) lambda (bool b) b ? i : self(!b) ", [](novasm::Assembler* asmb) -> void {
          // Function.
          asmb->addStackLoad(0); // Load arg 'i'.
          asmb->addLoadLitIp("lambda");
          asmb->addMakeStruct(2); // Make closure containing 'i' and the lambda ip.
          asmb->addRet();

          // Lambda.
          asmb->label("lambda");
          asmb->addStackLoad(0); // Load arg 'b'.
          asmb->addJumpIf("bTrue");

          asmb->addStackLoad(0);  // Load arg 'b'.
          asmb->addLogicInvInt(); // !b
          asmb->addStackLoad(1);  // Load closure arg 'i'.
          asmb->addCall("lambda", 2, novasm::CallMode::Tail);
          asmb->addJump("lambdaEnd");

          asmb->label("bTrue");
          asmb->addStackLoad(1); // Load closure arg 'i'.

          asmb->label("lambdaEnd");
          asmb->addRet();

          // Entry point.
          asmb->setEntrypoint("entry");
          asmb->label("entry");
          asmb->addRet();
        });
  }
}

} // namespace backend
