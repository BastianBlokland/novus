#include "catch2/catch.hpp"
#include "helpers.hpp"

namespace vm {

TEST_CASE("Execute input and output", "[vm]") {

  SECTION("PrintChar") {
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitInt('a');
          builder->addPCall(vm::PCallCode::PrintChar);
          builder->addPop();

          builder->addLoadLitInt('b');
          builder->addPCall(vm::PCallCode::PrintChar);
          builder->addPop();

          builder->addLoadLitInt('c');
          builder->addPCall(vm::PCallCode::PrintChar);
          builder->addPop();
        },
        "a",
        "b",
        "c");
  }

  SECTION("PrintString") {
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitString("hello world");
          builder->addPCall(vm::PCallCode::PrintString);
          builder->addPop();

          builder->addLoadLitInt(0);
          builder->addConvBoolString();
          builder->addPCall(vm::PCallCode::PrintString);
          builder->addPop();

          builder->addLoadLitInt(1);
          builder->addConvBoolString();
          builder->addPCall(vm::PCallCode::PrintString);
          builder->addPop();

          builder->addLoadLitInt(-42);
          builder->addConvIntString();
          builder->addPCall(vm::PCallCode::PrintString);
          builder->addPop();
        },
        "hello world",
        "false",
        "true",
        "-42");
  }

  SECTION("PrintStringLine") {
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitString("hello");
          builder->addPCall(vm::PCallCode::PrintStringLine);
          builder->addPop();

          builder->addLoadLitString("world");
          builder->addPCall(vm::PCallCode::PrintStringLine);
          builder->addPop();
        },
        "hello",
        "\n",
        "world",
        "\n");
  }
}

} // namespace vm
