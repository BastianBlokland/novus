#include "catch2/catch.hpp"
#include "helpers.hpp"

namespace vm {

TEST_CASE("Execute miscellaneous instructions", "[vm]") {

  SECTION("Dup") {
    CHECK_PROG(
        [](backend::Builder* builder) -> void {
          builder->label("start");
          builder->addLoadLitInt(42);
          builder->addDup();

          builder->addAddInt();
          builder->addConvIntString();
          builder->addPrintString();
          builder->addRet();

          builder->addEntryPoint("start");
        },
        "84");
  }

  SECTION("Pop") {
    CHECK_PROG(
        [](backend::Builder* builder) -> void {
          builder->label("start");
          builder->addLoadLitInt(42);
          builder->addLoadLitInt(1337);
          builder->addPop();

          builder->addConvIntString();
          builder->addPrintString();
          builder->addRet();

          builder->addEntryPoint("start");
        },
        "42");
  }
}

} // namespace vm
