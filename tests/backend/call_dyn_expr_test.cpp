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
          builder->label("check_eq_func");
          builder->addCheckEqIp();

          builder->label("test");
          builder->addReserveConsts(2);
          builder->addStoreConst(1);
          builder->addStoreConst(0);
          builder->addLoadConst(0);
          builder->addLoadConst(1);
          builder->addAddInt();
          builder->addRet();
          builder->addFail();

          builder->label("print");
          builder->addReserveConsts(1);

          builder->addLoadLitIp("test");
          builder->addDup();
          builder->addStoreConst(0);
          builder->addPop();

          builder->addLoadLitInt(42);
          builder->addLoadLitInt(1337);
          builder->addLoadConst(0);
          builder->addCallDyn();

          builder->addConvIntString();
          builder->addPrintString();
          builder->addRet();
          builder->addFail();

          builder->addEntryPoint("print");
        });
  }
}

} // namespace backend
