#include "catch2/catch.hpp"
#include "helpers.hpp"
#include <type_traits>

namespace backend {

TEST_CASE("Generate assembly for call dynamic expressions", "[backend]") {

  SECTION("User functions") {
    CHECK_PROG(
        "fun test(int a, int b) -> bool a == b "
        "assert(op = test; op(42, 1337), \"test\")",
        [](novasm::Assembler* asmb) -> void {
          asmb->label("test");
          asmb->addStackLoad(0);
          asmb->addStackLoad(1);
          asmb->addCheckEqInt();
          asmb->addRet();

          asmb->label("prog");
          asmb->addStackAlloc(1);

          asmb->addLoadLitIp("test");
          asmb->addDup();
          asmb->addStackStore(0);
          asmb->addPop();

          asmb->addLoadLitInt(42);
          asmb->addLoadLitInt(1337);
          asmb->addStackLoad(0);
          asmb->addCallDyn(2, novasm::CallMode::Normal);

          asmb->addLoadLitString("test");
          asmb->addPCall(novasm::PCallCode::Assert);
          asmb->addRet();

          asmb->setEntrypoint("prog");
        });
  }

  SECTION("Closure") {
    CHECK_PROG(
        "assert(i = 1337; (lambda () i == 42)(), \"test\")", [](novasm::Assembler* asmb) -> void {
          asmb->label("anon func");
          asmb->addStackLoad(0);
          asmb->addLoadLitInt(42);
          asmb->addCheckEqInt();
          asmb->addRet();

          asmb->label("prog");
          asmb->addStackAlloc(1);

          asmb->addLoadLitInt(1337);
          asmb->addDup();
          asmb->addStackStore(0);
          asmb->addPop();

          asmb->addStackLoad(0);
          asmb->addLoadLitIp("anon func");
          asmb->addMakeStruct(2);

          asmb->addCallDyn(0, novasm::CallMode::Normal);
          asmb->addLoadLitString("test");
          asmb->addPCall(novasm::PCallCode::Assert);
          asmb->addRet();

          asmb->setEntrypoint("prog");
        });
  }

  SECTION("Closure") {
    CHECK_PROG(
        "assert(i = 1337; (lambda (float f) f == i)(.1), \"test\")",
        [](novasm::Assembler* asmb) -> void {
          asmb->label("anon func");
          asmb->addStackLoad(0);
          asmb->addStackLoad(1);
          asmb->addConvIntFloat();
          asmb->addCheckEqFloat();
          asmb->addRet();

          asmb->label("prog");
          asmb->addStackAlloc(1);

          asmb->addLoadLitInt(1337);
          asmb->addDup();
          asmb->addStackStore(0);
          asmb->addPop();

          asmb->addLoadLitFloat(.1F); // NOLINT: Magic numbers
          asmb->addStackLoad(0);
          asmb->addLoadLitIp("anon func");
          asmb->addMakeStruct(2);

          asmb->addCallDyn(1, novasm::CallMode::Normal);
          asmb->addLoadLitString("test");
          asmb->addPCall(novasm::PCallCode::Assert);
          asmb->addRet();

          asmb->setEntrypoint("prog");
        });
  }
}

} // namespace backend
