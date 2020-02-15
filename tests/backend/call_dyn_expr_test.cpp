#include "catch2/catch.hpp"
#include "helpers.hpp"
#include <type_traits>

namespace backend {

TEST_CASE("Generate assembly for call dynamic expressions", "[backend]") {

  SECTION("User functions") {
    CHECK_PROG(
        "fun test(int a, int b) -> int a + b "
        "conWrite(op = test; op(42, 1337).string())",
        [](backend::Builder* builder) -> void {
          builder->label("test");
          builder->addStackLoad(0);
          builder->addStackLoad(1);
          builder->addAddInt();
          builder->addRet();

          builder->label("prog");
          builder->addStackAlloc(1);

          builder->addLoadLitIp("test");
          builder->addDup();
          builder->addStackStore(0);
          builder->addPop();

          builder->addLoadLitInt(42);
          builder->addLoadLitInt(1337);
          builder->addStackLoad(0);
          builder->addCallDyn(2, CallMode::Normal);

          builder->addConvIntString();
          builder->addPCall(vm::PCallCode::ConWriteString);
          builder->addRet();

          builder->setEntrypoint("prog");
        });
  }

  SECTION("Closure") {
    CHECK_PROG(
        "conWrite(i = 1337; (lambda () 42 + i)().string())", [](backend::Builder* builder) -> void {
          builder->label("anon func");
          builder->addLoadLitInt(42);
          builder->addStackLoad(0);
          builder->addAddInt();
          builder->addRet();

          builder->label("prog");
          builder->addStackAlloc(1);

          builder->addLoadLitInt(1337);
          builder->addDup();
          builder->addStackStore(0);
          builder->addPop();

          builder->addStackLoad(0);
          builder->addLoadLitIp("anon func");
          builder->addMakeStruct(2);

          builder->addCallDyn(0, CallMode::Normal);
          builder->addConvIntString();
          builder->addPCall(vm::PCallCode::ConWriteString);
          builder->addRet();

          builder->setEntrypoint("prog");
        });
  }

  SECTION("Closure") {
    CHECK_PROG(
        "conWrite(i = 1337; (lambda (float f) f + i)(.1).string())",
        [](backend::Builder* builder) -> void {
          builder->label("anon func");
          builder->addStackLoad(0);
          builder->addStackLoad(1);
          builder->addConvIntFloat();
          builder->addAddFloat();
          builder->addRet();

          builder->label("prog");
          builder->addStackAlloc(1);

          builder->addLoadLitInt(1337);
          builder->addDup();
          builder->addStackStore(0);
          builder->addPop();

          builder->addLoadLitFloat(.1F); // NOLINT: Magic numbers
          builder->addStackLoad(0);
          builder->addLoadLitIp("anon func");
          builder->addMakeStruct(2);

          builder->addCallDyn(1, CallMode::Normal);
          builder->addConvFloatString();
          builder->addPCall(vm::PCallCode::ConWriteString);
          builder->addRet();

          builder->setEntrypoint("prog");
        });
  }
}

} // namespace backend
