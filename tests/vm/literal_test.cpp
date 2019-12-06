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
        },
        "42");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitInt(-42);
          builder->addConvIntString();
          builder->addPrintString();
        },
        "-42");
  }

  SECTION("Float literals") {
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitFloat(0.1337F); // NOLINT: Magic numbers
          builder->addConvFloatString();
          builder->addPrintString();
        },
        "0.1337");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitFloat(-0.1337F); // NOLINT: Magic numbers
          builder->addConvFloatString();
          builder->addPrintString();
        },
        "-0.1337");
  }

  SECTION("String literals") {
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitString("");
          builder->addPrintString();

          builder->addLoadLitString("hello");
          builder->addPrintString();

          builder->addLoadLitString(" ");
          builder->addPrintString();

          builder->addLoadLitString("world");
          builder->addPrintString();

          builder->addLoadLitString(" ");
          builder->addPrintString();

          builder->addLoadLitString("!");
          builder->addPrintString();
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
