#include "catch2/catch.hpp"
#include "helpers.hpp"

namespace vm {

TEST_CASE("Execute long checks", "[vm]") {

  SECTION("Equal") {
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitLong(4'200'000'000'000L);
          builder->addLoadLitLong(-4'200'000'000'000L);
          builder->addCheckEqLong();
          builder->addConvBoolString();
          builder->addPCall(vm::PCallCode::ConWriteString);
        },
        "input",
        "false");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitLong(4'200'000'000'000L);
          builder->addLoadLitLong(4'200'000'000'000L);
          builder->addCheckEqLong();
          builder->addConvBoolString();
          builder->addPCall(vm::PCallCode::ConWriteString);
        },
        "input",
        "true");
  }
}

} // namespace vm
