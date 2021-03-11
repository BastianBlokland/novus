#include "catch2/catch.hpp"
#include "helpers.hpp"

namespace vm {

TEST_CASE("[vm] Execute forks", "vm") {

  SECTION("Waiting for fork results") {
    CHECK_PROG(
        [](novasm::Assembler* asmb) -> void {
          asmb->setEntrypoint("entry");
          // --- Main function start.
          asmb->label("entry");
          asmb->addStackAlloc(1); // Reserve space for a variable.

          // Start two worker functions as forks.
          asmb->addLoadLitInt(100);
          asmb->addCall("worker", 1, novasm::CallMode::Forked);

          // Call one as a closure.
          asmb->addLoadLitInt(10);
          asmb->addLoadLitIp("worker");
          asmb->addMakeStruct(2);
          asmb->addCallDyn(0, novasm::CallMode::Forked);

          // Wait for them to finish.
          asmb->addFutureBlock();
          asmb->addStackStore(0);

          asmb->addFutureBlock();

          // Add the results together.
          asmb->addStackLoad(0);
          asmb->addAddInt();

          // Print the results.
          asmb->addConvIntString();
          ADD_PRINT(asmb);
          asmb->addRet();
          // --- Main function end.

          // --- Worker function start (takes one int arg).
          asmb->label("worker");
          // Loop until the input argument is 0.
          asmb->addStackLoad(0); // Load arg 0.
          asmb->addLoadLitInt(0);
          asmb->addCheckEqInt();
          asmb->addJumpIf("worker-end");

          // Call itself with 'arg - 1'.
          asmb->addStackLoad(0); // Load arg 0.
          asmb->addLoadLitInt(1);
          asmb->addSubInt();
          asmb->addCall("worker", 1, novasm::CallMode::Tail);

          asmb->label("worker-end");
          asmb->addLoadLitInt(42); // Return 42.
          asmb->addRet();
          // --- Worker function end.
        },
        "input",
        "84");
  }

  SECTION("Error in fork is transferred on wait") {
    CHECK_PROG_RESULTCODE(
        [](novasm::Assembler* asmb) -> void {
          asmb->setEntrypoint("entry");
          // --- Main function start.
          asmb->label("entry");
          asmb->addStackAlloc(1); // Reserve space for a variable.

          // Start the fark.
          asmb->addCall("fork", 0, novasm::CallMode::Forked);

          // Wait for it to finish.
          asmb->addFutureBlock();
          asmb->addRet();
          // --- Main function end.

          // --- Fork function start (Caused div-by-zero runtime error)
          asmb->label("fork");
          asmb->addLoadLitInt(0);
          asmb->addLoadLitInt(0);
          asmb->addDivInt();
          // --- Fork function end.
        },
        "input",
        ExecState::DivByZero);
  }

  SECTION("Poll on finished fork returns true") {
    CHECK_PROG(
        [](novasm::Assembler* asmb) -> void {
          asmb->setEntrypoint("entry");
          // --- Main function start.
          asmb->label("entry");

          // Start worker functions as forks.
          asmb->addLoadLitInt(100);
          asmb->addCall("worker", 1, novasm::CallMode::Forked);
          asmb->addDup();

          // Wait for fork to finish and discard result.
          asmb->addFutureBlock();
          asmb->addPop();

          // Poll the fork state (wait with 0 ns timeout).
          asmb->addLoadLitLong(0);
          asmb->addFutureWaitNano();

          // Print the result.
          ADD_BOOL_TO_STRING(asmb);
          ADD_PRINT(asmb);
          asmb->addRet();
          // --- Main function end.

          // --- Worker function start (takes one int arg).
          asmb->label("worker");
          asmb->addStackLoad(0); // Load arg 0.
          asmb->addRet();
          // --- Worker function end.
        },
        "input",
        "true");
  }
}

} // namespace vm
