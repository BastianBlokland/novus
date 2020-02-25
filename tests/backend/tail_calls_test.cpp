#include "catch2/catch.hpp"
#include "helpers.hpp"

namespace backend {

TEST_CASE("Generate assembly for tail calls", "[backend]") {

  SECTION("Tail recursion") {
    CHECK_PROG(
        "fun test(int a) -> int a > 0 ? test(0) : a "
        "conWrite(string(test(42)))",
        [](novasm::Assembler* asmb) -> void {
          asmb->label("test");
          asmb->addStackLoad(0);
          asmb->addLoadLitInt(0);
          asmb->addCheckGtInt();
          asmb->addJumpIf("true");

          asmb->addStackLoad(0);
          asmb->addJump("false");

          asmb->label("true");
          asmb->addLoadLitInt(0);
          asmb->addCall("test", 1, novasm::CallMode::Tail);

          asmb->label("false");
          asmb->addRet();

          asmb->label("prog");
          asmb->addLoadLitInt(42);
          asmb->addCall("test", 1, novasm::CallMode::Normal);
          asmb->addConvIntString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
          asmb->addRet();

          asmb->setEntrypoint("prog");
        });
  }

  SECTION("Tail call") {
    CHECK_PROG(
        "fun f1() -> int 42 "
        "fun f2() -> int f1() "
        "conWrite(string(f2()))",
        [](novasm::Assembler* asmb) -> void {
          asmb->label("f2");
          asmb->addCall("f1", 0, novasm::CallMode::Tail);
          asmb->addRet();

          asmb->label("f1");
          asmb->addLoadLitInt(42);
          asmb->addRet();

          asmb->label("prog");
          asmb->addCall("f2", 0, novasm::CallMode::Normal);
          asmb->addConvIntString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
          asmb->addRet();

          asmb->setEntrypoint("prog");
        });
  }

  SECTION("Tail call at end of group expression") {
    CHECK_PROG(
        "fun f1(int i) -> int i "
        "fun f2() -> int v = 42; f1(v) "
        "conWrite(string(f2()))",
        [](novasm::Assembler* asmb) -> void {
          asmb->label("f2");
          asmb->addStackAlloc(1);
          asmb->addLoadLitInt(42);
          asmb->addDup();
          asmb->addStackStore(0);
          asmb->addPop();
          asmb->addStackLoad(0);
          asmb->addCall("f1", 1, novasm::CallMode::Tail);
          asmb->addRet();

          asmb->label("f1");
          asmb->addStackLoad(0);
          asmb->addRet();

          asmb->label("prog");
          asmb->addCall("f2", 0, novasm::CallMode::Normal);
          asmb->addConvIntString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
          asmb->addRet();

          asmb->setEntrypoint("prog");
        });
  }

  SECTION("Dynamic tail call") {
    CHECK_PROG(
        "fun f1() -> int 42 "
        "fun f2(function{int} func) -> int func() "
        "conWrite(string(f2(f1)))",
        [](novasm::Assembler* asmb) -> void {
          asmb->label("f2");
          asmb->addStackLoad(0);
          asmb->addCallDyn(0, novasm::CallMode::Tail);
          asmb->addRet();

          asmb->label("f1");
          asmb->addLoadLitInt(42);
          asmb->addRet();

          asmb->label("prog");
          asmb->addLoadLitIp("f1");
          asmb->addCall("f2", 1, novasm::CallMode::Normal);
          asmb->addConvIntString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
          asmb->addRet();

          asmb->setEntrypoint("prog");
        });
  }
}

} // namespace backend
