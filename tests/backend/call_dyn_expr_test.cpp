#include "catch2/catch.hpp"
#include "helpers.hpp"
#include <type_traits>

namespace backend {

TEST_CASE("[backend] Generate assembly for dynamic call expressions", "backend") {

  SECTION("User functions") {
    CHECK_PROG(
        "fun funcA(int a, int b) -> bool intrinsic{int_eq_int}(a, b) "
        "fun test(bool b) b "
        "test(op = funcA; op(42, 1337))",
        [](novasm::Assembler* asmb) -> void {
          asmb->label("funcA");
          asmb->addStackLoad(0);
          asmb->addStackLoad(1);
          asmb->addCheckEqInt();
          asmb->addRet();

          // --- test function start.
          asmb->label("func-test");
          asmb->addStackLoad(0);
          asmb->addRet();
          // --- test function end.

          asmb->label("prog");
          asmb->addStackAlloc(1);

          asmb->addLoadLitIp("funcA");
          asmb->addStackStore(0);

          asmb->addLoadLitInt(42);
          asmb->addLoadLitInt(1337);
          asmb->addStackLoad(0);
          asmb->addCallDyn(2, novasm::CallMode::Normal);

          asmb->addCall("func-test", 1, novasm::CallMode::Normal);
          asmb->addRet();

          asmb->setEntrypoint("prog");
        });
  }

  SECTION("Closure") {
    CHECK_PROG(
        "fun test(bool b) b "
        "test(i = 1337; (lambda () -> bool intrinsic{int_eq_int}(i, 42))())",
        [](novasm::Assembler* asmb) -> void {
          // --- test function start.
          asmb->label("func-test");
          asmb->addStackLoad(0);
          asmb->addRet();
          // --- test function end.

          asmb->label("anon func");
          asmb->addStackLoad(0);
          asmb->addLoadLitInt(42);
          asmb->addCheckEqInt();
          asmb->addRet();

          asmb->label("prog");
          asmb->addStackAlloc(1);

          asmb->addLoadLitInt(1337);
          asmb->addStackStore(0);

          asmb->addStackLoad(0);
          asmb->addLoadLitIp("anon func");
          asmb->addMakeStruct(2);

          asmb->addCallDyn(0, novasm::CallMode::Normal);
          asmb->addCall("func-test", 1, novasm::CallMode::Normal);
          asmb->addRet();

          asmb->setEntrypoint("prog");
        });
  }

  SECTION("Closure") {
    CHECK_PROG(
        "fun test(bool b) b "
        "test(i = 1337.0; (lambda (float f) -> bool intrinsic{float_eq_float}(f, i))(.1))",
        [](novasm::Assembler* asmb) -> void {
          // --- test function start.
          asmb->label("func-test");
          asmb->addStackLoad(0);
          asmb->addRet();
          // --- test function end.

          asmb->label("anon func");
          asmb->addStackLoad(0);
          asmb->addStackLoad(1);
          asmb->addCheckEqFloat();
          asmb->addRet();

          asmb->label("prog");
          asmb->addStackAlloc(1);

          asmb->addLoadLitFloat(1337.0f);
          asmb->addStackStore(0);

          asmb->addLoadLitFloat(.1F); // NOLINT: Magic numbers
          asmb->addStackLoad(0);
          asmb->addLoadLitIp("anon func");
          asmb->addMakeStruct(2);

          asmb->addCallDyn(1, novasm::CallMode::Normal);
          asmb->addCall("func-test", 1, novasm::CallMode::Normal);
          asmb->addRet();

          asmb->setEntrypoint("prog");
        });
  }
}

} // namespace backend
