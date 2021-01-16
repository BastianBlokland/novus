#include "catch2/catch.hpp"
#include "helpers.hpp"

namespace vm {

TEST_CASE("[vm] Execute atomic operations", "vm") {

  SECTION("Create and load atomic") {
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addMakeAtomic(42);
          asmb->addAtomicLoad();
          asmb->addConvIntString();
          ADD_PRINT(asmb);
        },
        "input",
        "42");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addMakeAtomic(-1337);
          asmb->addAtomicLoad();
          asmb->addConvIntString();
          ADD_PRINT(asmb);
        },
        "input",
        "-1337");
  }

  SECTION("Compare and swap returns the previous value") {
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addMakeAtomic(42);
          asmb->addDup();

          asmb->addAtomicCompareSwap(42, 1337);
          asmb->addConvIntString();
          ADD_PRINT(asmb);
        },
        "input",
        "42");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addMakeAtomic(42);
          asmb->addDup();
          asmb->addDup();

          asmb->addAtomicCompareSwap(42, 1337);
          asmb->addConvIntString();
          ADD_PRINT(asmb);
          asmb->addPop();

          asmb->addAtomicCompareSwap(1337, 42);
          asmb->addConvIntString();
          ADD_PRINT(asmb);
          asmb->addPop();
        },
        "input",
        "421337");
  }

  SECTION("Compare and swap transitions if the expected value is met") {
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addMakeAtomic(42);
          asmb->addDup();

          asmb->addAtomicCompareSwap(42, 1337);
          asmb->addPop(); // Ignore the cas result.

          asmb->addAtomicLoad();
          asmb->addConvIntString();
          ADD_PRINT(asmb);
        },
        "input",
        "1337");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addMakeAtomic(42);
          asmb->addDup();
          asmb->addDup();

          asmb->addAtomicCompareSwap(42, 1337);
          asmb->addPop(); // Ignore the cas result.

          asmb->addAtomicCompareSwap(1337, 42);
          asmb->addPop(); // Ignore the cas result.

          asmb->addAtomicLoad();
          asmb->addConvIntString();
          ADD_PRINT(asmb);
        },
        "input",
        "42");
  }

  SECTION("Atomic block returns immediate if the value matched the expected") {
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addMakeAtomic(42);
          asmb->addAtomicBlock(42);
        },
        "input",
        "");
  }

  SECTION("Only a single executer will transition the atomic") {
    CHECK_PROG(
        [&](novasm::Assembler* asmb) -> void {
          constexpr uint8_t numForks = 15u;

          asmb->setEntrypoint("entry");
          // --- Main function start.
          asmb->label("entry");
          asmb->addStackAlloc(2 + numForks); // Reserve stack space:
          // Stack var 0:                     atomic.
          // Stack var 1:                     loop counter integer.
          // Stack var 2 - ( numForks + 2 ):  future handles to the forked workers.

          // Create an atomic and save it in a variable.
          asmb->addMakeAtomic(0); // Start with value 0.
          asmb->addStackStore(0);

          // Start multiple workers functions as forks that attempt to change the atomic.
          for (auto i = 0u; i != numForks; ++i) {
            asmb->addStackLoad(0); // Pass the atomic to the worker.
            asmb->addCall("worker", 1, novasm::CallMode::Forked);
            asmb->addStackStore(2u + i); // Store the future to the fork on the stack.
          }

          // Wait for all workers to complete.
          for (auto i = 0u; i != numForks; ++i) {
            asmb->addStackLoad(2u + i); // Load the future from the stack.
            asmb->addFutureBlock();
            asmb->addPop(); // Ignore the return value.
          }
          asmb->addRet();
          // --- Main function end.

          // --- Worker function start (takes one atomic, cas to 1 and print if succeeds).
          asmb->label("worker");
          asmb->addStackLoad(0); // Load the atomic from arg 0.
          asmb->addAtomicCompareSwap(0, 1);
          asmb->addJumpIf("worker-ret");
          asmb->addLoadLitString("Hello from worker");
          ADD_PRINT(asmb);
          asmb->addPop(); // Ignore print result.
          asmb->label("worker-ret");
          asmb->addLoadLitInt(0); // Return 0.
          asmb->addRet();
          // --- Worker function end.
        },
        "input",
        "Hello from worker");
  }

  SECTION("Block waits until the atomic has the expected value") {
    CHECK_PROG(
        [&](novasm::Assembler* asmb) -> void {
          asmb->setEntrypoint("entry");
          // --- Main function start.
          asmb->label("entry");
          asmb->addStackAlloc(1); // Reserve stack space:
          // Stack var 0: struct with atomic and value.

          // Make a struct with an atomic and a 'value to observe'.
          asmb->addMakeAtomic(0); // Start with value 0.
          asmb->addLoadLitInt(42);
          asmb->addMakeStruct(2);
          asmb->addStackStore(0); // Store the struct on the stack.

          // Start a worker function as a fork that will change the value to observe.
          asmb->addStackLoad(0); // Load the struct.
          asmb->addCall("worker", 1, novasm::CallMode::Forked);
          asmb->addPop(); // Ignore the future to the worker.

          // Wait until the atomic has the value 1.
          asmb->addStackLoad(0);       // Load the struct.
          asmb->addStructLoadField(0); // Load the atomic.
          asmb->addAtomicBlock(1);

          // Print the value to observe.
          asmb->addStackLoad(0);       // Load the struct.
          asmb->addStructLoadField(1); // Load the value to observe
          asmb->addConvIntString();
          ADD_PRINT(asmb);

          asmb->addRet();
          // --- Main function end.

          // --- Worker function start (takes the struct, set the value and cas the atomic to 1).
          asmb->label("worker");
          asmb->addStackLoad(0); // Load the struct from arg 0.
          asmb->addDup();

          // Sleep for a milli-second to make sure the main executor has to wait for us.
          asmb->addLoadLitLong(1'000'000);
          asmb->addPCall(novasm::PCallCode::SleepNano);
          asmb->addPop(); // Ignore the sleep result.

          // Change the value to observe.
          asmb->addLoadLitInt(1337);
          asmb->addStructStoreField(1);

          // Change the atomic to 1.
          asmb->addStructLoadField(0); // Load the atomic.
          asmb->addAtomicCompareSwap(0, 1);
          asmb->addRet();
          // --- Worker function end.
        },
        "input",
        "1337");
  }
}

} // namespace vm
