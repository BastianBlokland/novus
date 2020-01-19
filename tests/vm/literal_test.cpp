#include "catch2/catch.hpp"
#include "helpers.hpp"

namespace vm {

TEST_CASE("Execute literals", "[vm]") {

  SECTION("Integer literals") {
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitInt(42);
          builder->addConvIntString();
          builder->addPrintString();
          builder->addPop();
        },
        "42");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitInt(-42);
          builder->addConvIntString();
          builder->addPrintString();
          builder->addPop();
        },
        "-42");
  }

  SECTION("Float literals") {
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitFloat(0.1337F); // NOLINT: Magic numbers
          builder->addConvFloatString();
          builder->addPrintString();
          builder->addPop();
        },
        "0.1337");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitFloat(-0.1337F); // NOLINT: Magic numbers
          builder->addConvFloatString();
          builder->addPrintString();
          builder->addPop();
        },
        "-0.1337");
  }

  SECTION("String literals") {
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitString("");
          builder->addPrintString();
          builder->addPop();

          builder->addLoadLitString("hello");
          builder->addPrintString();
          builder->addPop();

          builder->addLoadLitString(" ");
          builder->addPrintString();
          builder->addPop();

          builder->addLoadLitString("world");
          builder->addPrintString();
          builder->addPop();

          builder->addLoadLitString(" ");
          builder->addPrintString();
          builder->addPop();

          builder->addLoadLitString("!");
          builder->addPrintString();
          builder->addPop();
        },
        "",
        "hello",
        " ",
        "world",
        " ",
        "!");
  }
}

} // namespace vm
