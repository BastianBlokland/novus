#include "catch2/catch.hpp"
#include "helpers.hpp"

namespace vm {

TEST_CASE("Execute string checks", "[vm]") {

  SECTION("Equal") {
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitString("hello");
          builder->addLoadLitString("world");
          builder->addCheckEqString();
          builder->addConvBoolString();
          builder->addPCall(vm::PCallCode::Print);
          builder->addPop();
        },
        "false");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitString("hello world");
          builder->addLoadLitString("hello world");
          builder->addCheckEqString();
          builder->addConvBoolString();
          builder->addPCall(vm::PCallCode::Print);
          builder->addPop();
        },
        "true");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitString("hello world");
          builder->addLoadLitString("hello ");
          builder->addLoadLitString("world");
          builder->addAddString();
          builder->addCheckEqString();
          builder->addConvBoolString();
          builder->addPCall(vm::PCallCode::Print);
          builder->addPop();
        },
        "true");
  }
}

} // namespace vm
