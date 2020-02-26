#include "catch2/catch.hpp"
#include "helpers.hpp"
#include <type_traits>

namespace backend {

TEST_CASE("Generate assembly for call dynamic expressions", "[backend]") {

  SECTION("User functions") {
    CHECK_PROG(
        "fun test(int a, int b) -> int a + b "
        "conWrite(op = test; op(42, 1337).string())",
        [](novasm::Assembler* asmb) -> void {
          asmb->label("test");
          asmb->addStackLoad(0);
          asmb->addStackLoad(1);
          asmb->addAddInt();
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

          asmb->addConvIntString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
          asmb->addRet();

          asmb->setEntrypoint("prog");
        });
  }

  SECTION("Closure") {
    CHECK_PROG(
        "conWrite(i = 1337; (lambda () 42 + i)().string())", [](novasm::Assembler* asmb) -> void {
          asmb->label("anon func");
          asmb->addLoadLitInt(42);
          asmb->addStackLoad(0);
          asmb->addAddInt();
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
          asmb->addConvIntString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
          asmb->addRet();

          asmb->setEntrypoint("prog");
        });
  }

  SECTION("Closure") {
    CHECK_PROG(
        "conWrite(i = 1337; (lambda (float f) f + i)(.1).string())",
        [](novasm::Assembler* asmb) -> void {
          asmb->label("anon func");
          asmb->addStackLoad(0);
          asmb->addStackLoad(1);
          asmb->addConvIntFloat();
          asmb->addAddFloat();
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
          asmb->addConvFloatString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
          asmb->addRet();

          asmb->setEntrypoint("prog");
        });
  }
}

} // namespace backend
