#include "catch2/catch.hpp"
#include "helpers.hpp"

namespace backend {

TEST_CASE("Generate assembly for lazy calls", "[backend]") {

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
          asmb->addLoadLitInt(0); // 0 to indicate that the values has not been computed.

          // Create closure struct pointing to f1.
          asmb->addLoadLitIp("f1");
          asmb->addMakeStruct(1);

          asmb->addLoadLitInt(0); // Set 0 as an 'empty' computed value.

          // Create the lazy struct containing the 3 fields we've just created.
          asmb->addMakeStruct(3);
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
          asmb->addLoadLitInt(0); // 0 to indicate that the values has not been computed.

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

          asmb->addLoadLitInt(0); // Set 0 as an 'empty' computed value.

          // Create the lazy struct containing the 3 fields we've just created.
          asmb->addMakeStruct(3);
          // <<< Create lazy object.

          asmb->addRet();

          asmb->label("entry");
          asmb->addRet();

          asmb->setEntrypoint("entry");
        });
  }

  SECTION("Lazy get") {
    CHECK_PROG("fun f(lazy{int} l) -> int l.get()", [](novasm::Assembler* asmb) -> void {
      asmb->label("f");

      // Load the lazy 'l' input constant.
      asmb->addStackLoad(0);
      asmb->addDup();

      // Check if a value has already been computed.
      asmb->addStructLoadField(0);
      asmb->addJumpIf("already-computed");

      // Duplicate the lazy a few times for the following accesses.
      asmb->addDup();
      asmb->addDup();
      asmb->addDup();

      // Compute the value.
      asmb->addStructLoadField(1);
      asmb->addCallDyn(0, novasm::CallMode::Normal);

      // Store the computed value.
      asmb->addStructStoreField(2);

      // Set the 'computed' flag to true.
      asmb->addLoadLitInt(1);
      asmb->addStructStoreField(0);

      // Load the already computed value.
      asmb->label("already-computed");
      asmb->addStructLoadField(2);

      asmb->addRet();

      asmb->label("entry");
      asmb->addRet();

      asmb->setEntrypoint("entry");
    });
  }
}

} // namespace backend
