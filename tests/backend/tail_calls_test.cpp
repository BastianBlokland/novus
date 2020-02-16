#include "catch2/catch.hpp"
#include "helpers.hpp"

namespace backend {

TEST_CASE("Generate assembly for tail calls", "[backend]") {

  SECTION("Tail recursion") {
    CHECK_PROG(
        "fun test(int a) -> int a > 0 ? test(0) : a "
        "conWrite(string(test(42)))",
        [](backend::Builder* builder) -> void {
          builder->label("test");
          builder->addStackLoad(0);
          builder->addLoadLitInt(0);
          builder->addCheckGtInt();
          builder->addJumpIf("true");

          builder->addStackLoad(0);
          builder->addJump("false");

          builder->label("true");
          builder->addLoadLitInt(0);
          builder->addCall("test", 1, CallMode::Tail);

          builder->label("false");
          builder->addRet();

          builder->label("prog");
          builder->addLoadLitInt(42);
          builder->addCall("test", 1, CallMode::Normal);
          builder->addConvIntString();
          builder->addPCall(vm::PCallCode::ConWriteString);
          builder->addRet();

          builder->setEntrypoint("prog");
        });
  }

  SECTION("Tail call") {
    CHECK_PROG(
        "fun f1() -> int 42 "
        "fun f2() -> int f1() "
        "conWrite(string(f2()))",
        [](backend::Builder* builder) -> void {
          builder->label("f2");
          builder->addCall("f1", 0, CallMode::Tail);
          builder->addRet();

          builder->label("f1");
          builder->addLoadLitInt(42);
          builder->addRet();

          builder->label("prog");
          builder->addCall("f2", 0, CallMode::Normal);
          builder->addConvIntString();
          builder->addPCall(vm::PCallCode::ConWriteString);
          builder->addRet();

          builder->setEntrypoint("prog");
        });
  }

  SECTION("Tail call at end of group expression") {
    CHECK_PROG(
        "fun f1(int i) -> int i "
        "fun f2() -> int v = 42; f1(v) "
        "conWrite(string(f2()))",
        [](backend::Builder* builder) -> void {
          builder->label("f2");
          builder->addStackAlloc(1);
          builder->addLoadLitInt(42);
          builder->addDup();
          builder->addStackStore(0);
          builder->addPop();
          builder->addStackLoad(0);
          builder->addCall("f1", 1, CallMode::Tail);
          builder->addRet();

          builder->label("f1");
          builder->addStackLoad(0);
          builder->addRet();

          builder->label("prog");
          builder->addCall("f2", 0, CallMode::Normal);
          builder->addConvIntString();
          builder->addPCall(vm::PCallCode::ConWriteString);
          builder->addRet();

          builder->setEntrypoint("prog");
        });
  }

  SECTION("Dynamic tail call") {
    CHECK_PROG(
        "fun f1() -> int 42 "
        "fun f2(function{int} func) -> int func() "
        "conWrite(string(f2(f1)))",
        [](backend::Builder* builder) -> void {
          builder->label("f2");
          builder->addStackLoad(0);
          builder->addCallDyn(0, CallMode::Tail);
          builder->addRet();

          builder->label("f1");
          builder->addLoadLitInt(42);
          builder->addRet();

          builder->label("prog");
          builder->addLoadLitIp("f1");
          builder->addCall("f2", 1, CallMode::Normal);
          builder->addConvIntString();
          builder->addPCall(vm::PCallCode::ConWriteString);
          builder->addRet();

          builder->setEntrypoint("prog");
        });
  }
}

} // namespace backend
