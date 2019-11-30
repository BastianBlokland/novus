#include "catch2/catch.hpp"
#include "helpers.hpp"

namespace vm {

TEST_CASE("Execute struct operations", "[vm]") {

  SECTION("Load field") {
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addReserveConsts(1);

          builder->addLoadLitString("hello ");
          builder->addLoadLitString("world");
          builder->addMakeStruct(2);
          builder->addStoreConst(0);

          builder->addLoadConst(0);
          builder->addLoadStructField(0);

          builder->addLoadConst(0);
          builder->addLoadStructField(1);

          builder->addAddString();
          builder->addPrintString();
        },
        "hello world");
  }
}

} // namespace vm
