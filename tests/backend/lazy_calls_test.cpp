#include "catch2/catch.hpp"
#include "helpers.hpp"

namespace backend {

TEST_CASE("[backend] Generate assembly for lazy calls", "backend") {

  SECTION("Lazy call to static function") {
    CHECK_PROG(
        "fun f1() -> int 42 "
        "fun f2() -> lazy{int} lazy f1()",
        [](novasm::Assembler* asmb) -> void {
          asmb->label("f1");
          asmb->addLoadLitInt(42); // NOLINT: Magic numbers
          asmb->addRet();

          asmb->label("f2");

          // >>> Create lazy object.
          asmb->addMakeAtomic(0); // 0 to indicate that the values has not been computed.

          // Create closure struct pointing to f1.
          asmb->addLoadLitIp("f1");
          asmb->addMakeStruct(1);

          // Create the lazy struct containing the 2 fields we've just created.
          asmb->addMakeStruct(2);
          // <<< Create lazy object.

          asmb->addRet();

          asmb->label("entry");
          asmb->addRet();

          asmb->setEntrypoint("entry");
        });
  }

  SECTION("Lazy call to dynamic function") {
    CHECK_PROG(
        "fun f(function{int} func) -> lazy{int} lazy func()", [](novasm::Assembler* asmb) -> void {
          asmb->label("f");

          // >>> Create lazy object.
          asmb->addMakeAtomic(0); // 0 to indicate that the values has not been computed.

          // Create closure struct pointing to a 'trampoline' function that will the 'func'
          // delegate.
          asmb->addStackLoad(0);
          asmb->addLoadLitIp("trampoline-begin");
          asmb->addMakeStruct(2);
          asmb->addJump("trampoline-end"); // Jump 'over' the trampoline function.

          // Create 'trampoline' function.
          asmb->label("trampoline-begin");
          asmb->addCallDyn(0, novasm::CallMode::Tail);
          asmb->label("trampoline-end");

          asmb->addMakeStruct(2);
          // <<< Create lazy object.

          asmb->addRet();

          asmb->label("entry");
          asmb->addRet();

          asmb->setEntrypoint("entry");
        });
  }

  SECTION("Lazy get") {
    CHECK_PROG(
        "fun f(lazy{int} l) -> int intrinsic{lazy_get}(l)", [](novasm::Assembler* asmb) -> void {
          asmb->label("f");

          // Load the lazy 'l' input constant.
          asmb->addStackLoad(0);
          asmb->addDup(); // Duplicate the lazy struct for the following accesses.
          asmb->addDup();

          // Attempt to set the atomic from 0 to 1, if the original value was not 0: jump.
          asmb->addStructLoadField(0);
          asmb->addAtomicCompareSwap(0, 1);
          asmb->addJumpIf("not-idle");

          asmb->addDup(); // Duplicate the lazy struct for the following accesses.
          asmb->addDup();

          // Execute the closure and store the result.
          asmb->addStructLoadField(1);
          asmb->addCallDyn(0, novasm::CallMode::Normal);
          asmb->addStructStoreField(1);

          // Set the atomic to 2 and jump to the end.
          asmb->addStructLoadField(0);
          asmb->addAtomicCompareSwap(1, 2);
          asmb->addPop(); // Ignore the result of the compare and swap.
          asmb->addJump("return-result");

          // Getting here was that the value is already computed or is being computed.
          asmb->label("not-idle");

          // Wait for the atomic to have the value 2.
          asmb->addStructLoadField(0);
          asmb->addAtomicBlock(2);

          // Return.
          asmb->label("return-result");
          asmb->addStructLoadField(1); // Load the computed value.

          asmb->addRet();

          asmb->label("entry");
          asmb->addRet();

          asmb->setEntrypoint("entry");
        });
  }
}

} // namespace backend
