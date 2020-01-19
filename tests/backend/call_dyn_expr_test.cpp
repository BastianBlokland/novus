#include "catch2/catch.hpp"
#include "helpers.hpp"
#include <type_traits>

namespace backend {

TEST_CASE("Generate assembly for call dynamic expressions", "[backend]") {

  SECTION("User functions") {
    CHECK_PROG(
        "fun test(int a, int b) -> int a + b "
        "print(op = test; op(42, 1337))",
        [](backend::Builder* builder) -> void {
          builder->label("test");
          builder->addReserveConsts(2);
          builder->addStoreConst(1);
          builder->addStoreConst(0);
          builder->addLoadConst(0);
          builder->addLoadConst(1);
          builder->addAddInt();
          builder->addRet();

          builder->label("print");
          builder->addReserveConsts(1);

          builder->addLoadLitIp("test");
          builder->addDup();
          builder->addStoreConst(0);
          builder->addPop();

          builder->addLoadLitInt(42);
          builder->addLoadLitInt(1337);
          builder->addLoadConst(0);
          builder->addCallDyn(false);

          builder->addConvIntString();
          builder->addPrintString();
          builder->addPop();
          builder->addRet();

          builder->addEntryPoint("print");
        });
  }

  SECTION("Closure") {
    CHECK_PROG("print(i = 1337; (lambda () 42 + i)())", [](backend::Builder* builder) -> void {
      builder->label("anon func");
      builder->addReserveConsts(1);
      builder->addStoreConst(0);
      builder->addLoadLitInt(42);
      builder->addLoadConst(0);
      builder->addAddInt();
      builder->addRet();

      builder->label("print");
      builder->addReserveConsts(1);

      builder->addLoadLitInt(1337);
      builder->addDup();
      builder->addStoreConst(0);
      builder->addPop();

      builder->addLoadConst(0);
      builder->addLoadLitIp("anon func");
      builder->addMakeStruct(2);

      builder->addCallDyn(false);
      builder->addConvIntString();
      builder->addPrintString();
      builder->addPop();
      builder->addRet();

      builder->addEntryPoint("print");
    });
  }

  SECTION("Closure") {
    CHECK_PROG(
        "print(i = 1337; (lambda (float f) f + i)(.1))", [](backend::Builder* builder) -> void {
          builder->label("anon func");
          builder->addReserveConsts(2);
          builder->addStoreConst(1);
          builder->addStoreConst(0);
          builder->addLoadConst(0);
          builder->addLoadConst(1);
          builder->addConvIntFloat();
          builder->addAddFloat();
          builder->addRet();

          builder->label("print");
          builder->addReserveConsts(1);

          builder->addLoadLitInt(1337);
          builder->addDup();
          builder->addStoreConst(0);
          builder->addPop();

          builder->addLoadLitFloat(.1F); // NOLINT: Magic numbers
          builder->addLoadConst(0);
          builder->addLoadLitIp("anon func");
          builder->addMakeStruct(2);

          builder->addCallDyn(false);
          builder->addConvFloatString();
          builder->addPrintString();
          builder->addPop();
          builder->addRet();

          builder->addEntryPoint("print");
        });
  }
}

} // namespace backend
