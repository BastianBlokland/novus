#include "catch2/catch.hpp"
#include "helpers.hpp"

namespace backend {

TEST_CASE("Generate assembly for tail calls", "[backend]") {

  SECTION("Tail recursion") {
    CHECK_PROG(
        "fun test(int a) -> int a > 0 ? test(0) : a "
        "print(string(test(42)))",
        [](backend::Builder* builder) -> void {
          builder->label("test");
          builder->addReserveConsts(1);
          builder->addStoreConst(0);
          builder->addLoadConst(0);
          builder->addLoadLitInt(0);
          builder->addCheckGtInt();
          builder->addJumpIf("true");

          builder->addLoadConst(0);
          builder->addJump("false");

          builder->label("true");
          builder->addLoadLitInt(0);
          builder->addCall("test", true);

          builder->label("false");
          builder->addRet();
          builder->addFail();

          builder->label("print");
          builder->addLoadLitInt(42);
          builder->addCall("test", false);
          builder->addConvIntString();
          builder->addPrintString();
          builder->addRet();
          builder->addFail();

          builder->addEntryPoint("print");
        });
  }

  SECTION("Tail call") {
    CHECK_PROG(
        "fun f1() -> int 42 "
        "fun f2() -> int f1() "
        "print(string(f2()))",
        [](backend::Builder* builder) -> void {
          builder->label("f2");
          builder->addCall("f1", true);
          builder->addRet();
          builder->addFail();

          builder->label("f1");
          builder->addLoadLitInt(42);
          builder->addRet();
          builder->addFail();

          builder->label("print");
          builder->addCall("f2", false);
          builder->addConvIntString();
          builder->addPrintString();
          builder->addRet();
          builder->addFail();

          builder->addEntryPoint("print");
        });
  }

  SECTION("Tail call at end of group expression") {
    CHECK_PROG(
        "fun f1(int i) -> int i "
        "fun f2() -> int v = 42; f1(v) "
        "print(string(f2()))",
        [](backend::Builder* builder) -> void {
          builder->label("f2");
          builder->addReserveConsts(1);
          builder->addLoadLitInt(42);
          builder->addDup();
          builder->addStoreConst(0);
          builder->addPop();
          builder->addLoadConst(0);
          builder->addCall("f1", true);
          builder->addRet();
          builder->addFail();

          builder->label("f1");
          builder->addReserveConsts(1);
          builder->addStoreConst(0);
          builder->addLoadConst(0);
          builder->addRet();
          builder->addFail();

          builder->label("print");
          builder->addCall("f2", false);
          builder->addConvIntString();
          builder->addPrintString();
          builder->addRet();
          builder->addFail();

          builder->addEntryPoint("print");
        });
  }

  SECTION("Dynamic tail call") {
    CHECK_PROG(
        "fun f1() -> int 42 "
        "fun f2(delegate{int} func) -> int func() "
        "print(string(f2(f1)))",
        [](backend::Builder* builder) -> void {
          builder->label("f2");
          builder->addReserveConsts(1);
          builder->addStoreConst(0);
          builder->addLoadConst(0);
          builder->addCallDyn(true);
          builder->addRet();
          builder->addFail();

          builder->label("f1");
          builder->addLoadLitInt(42);
          builder->addRet();
          builder->addFail();

          builder->label("print");
          builder->addLoadLitIp("f1");
          builder->addCall("f2", false);
          builder->addConvIntString();
          builder->addPrintString();
          builder->addRet();
          builder->addFail();

          builder->addEntryPoint("print");
        });
  }
}

} // namespace backend
