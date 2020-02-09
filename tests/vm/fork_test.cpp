#include "catch2/catch.hpp"
#include "helpers.hpp"

namespace vm {

TEST_CASE("Execute forks", "[vm]") {

  SECTION("Waiting for fork results") {
    CHECK_PROG(
        [](backend::Builder* builder) -> void {
          builder->addEntryPoint("entry");
          // --- Main function start.
          builder->label("entry");
          builder->addStackAlloc(1); // Reserve space for a variable.

          // Start two worker functions as forks.
          builder->addLoadLitInt(100);
          builder->addCall("worker", 1, backend::CallMode::Forked);

          // Call one as a closure.
          builder->addLoadLitInt(10);
          builder->addLoadLitIp("worker");
          builder->addMakeStruct(2);
          builder->addCallDyn(0, backend::CallMode::Forked);

          // Wait for them to finish.
          builder->addWaitFuture();
          builder->addStackStore(0);

          builder->addWaitFuture();

          // Add the results together.
          builder->addStackLoad(0);
          builder->addAddInt();

          // Print the results.
          builder->addConvIntString();
          builder->addPCall(vm::PCallCode::ConWriteString);
          builder->addRet();
          // --- Main function end.

          // --- Worker function start (takes one int arg).
          builder->label("worker");
          // Loop until the input argument is 0.
          builder->addStackLoad(0); // Load arg 0.
          builder->addLoadLitInt(0);
          builder->addCheckEqInt();
          builder->addJumpIf("worker-end");

          // Call itself with 'arg - 1'.
          builder->addStackLoad(0); // Load arg 0.
          builder->addLoadLitInt(1);
          builder->addSubInt();
          builder->addCall("worker", 1, backend::CallMode::Tail);

          builder->label("worker-end");
          builder->addLoadLitInt(42); // Return 42.
          builder->addRet();
          // --- Worker function end.
        },
        "input",
        "84");
  }

  SECTION("Error in fork is transferred on wait") {
    CHECK_PROG_RESULTCODE(
        [](backend::Builder* builder) -> void {
          builder->addEntryPoint("entry");
          // --- Main function start.
          builder->label("entry");
          builder->addStackAlloc(1); // Reserve space for a variable.

          // Start the fark.
          builder->addCall("fork", 0, backend::CallMode::Forked);

          // Wait for it to finish.
          builder->addWaitFuture();
          builder->addRet();
          // --- Main function end.

          // --- Fork function start (Caused div-by-zero runtime error)
          builder->label("fork");
          builder->addLoadLitInt(0);
          builder->addLoadLitInt(0);
          builder->addDivInt();
          // --- Fork function end.
        },
        "input",
        ExecState::DivByZero);
  }
}

} // namespace vm
