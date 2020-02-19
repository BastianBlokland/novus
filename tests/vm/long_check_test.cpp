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

  SECTION("Greater") {
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitLong(-1);
          builder->addLoadLitLong(2);
          builder->addCheckGtLong();
          builder->addConvBoolString();
          builder->addPCall(vm::PCallCode::ConWriteString);
        },
        "input",
        "false");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitLong(2);
          builder->addLoadLitLong(1);
          builder->addCheckGtLong();
          builder->addConvBoolString();
          builder->addPCall(vm::PCallCode::ConWriteString);
        },
        "input",
        "true");
  }

  SECTION("Less") {
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitLong(3);
          builder->addLoadLitLong(2);
          builder->addCheckLeLong();
          builder->addConvBoolString();
          builder->addPCall(vm::PCallCode::ConWriteString);
        },
        "input",
        "false");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitLong(2);
          builder->addLoadLitLong(3);
          builder->addCheckLeLong();
          builder->addConvBoolString();
          builder->addPCall(vm::PCallCode::ConWriteString);
        },
        "input",
        "true");
  }
}

} // namespace vm
