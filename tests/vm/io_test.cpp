#include "catch2/catch.hpp"
#include "helpers.hpp"

namespace vm {

TEST_CASE("Execute input and output", "[vm]") {

  SECTION("Print") {
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitString("hello world");
          builder->addPCall(vm::PCallCode::Print);
          builder->addPop();

          builder->addLoadLitInt(0);
          builder->addConvBoolString();
          builder->addPCall(vm::PCallCode::Print);
          builder->addPop();

          builder->addLoadLitInt(1);
          builder->addConvBoolString();
          builder->addPCall(vm::PCallCode::Print);
          builder->addPop();

          builder->addLoadLitInt(-42);
          builder->addConvIntString();
          builder->addPCall(vm::PCallCode::Print);
          builder->addPop();
        },
        "hello world",
        "false",
        "true",
        "-42");
  }
  SECTION("PrintLine") {
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitString("hello");
          builder->addPCall(vm::PCallCode::PrintLine);
          builder->addPop();

          builder->addLoadLitString("world");
          builder->addPCall(vm::PCallCode::PrintLine);
          builder->addPop();
        },
        "hello",
        "\n",
        "world",
        "\n");
  }
}

} // namespace vm
