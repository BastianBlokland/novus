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
          builder->addPCall(vm::PCallCode::ConWriteString);
          builder->addPop();
          builder->addRet();

          builder->addEntryPoint("start");
        },
        "input",
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
          builder->addPCall(vm::PCallCode::ConWriteString);
          builder->addPop();
          builder->addRet();

          builder->addEntryPoint("start");
        },
        "input",
        "42");
  }
}

} // namespace vm
