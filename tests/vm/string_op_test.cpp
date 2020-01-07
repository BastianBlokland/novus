#include "catch2/catch.hpp"
#include "helpers.hpp"

namespace vm {

TEST_CASE("Execute string operations", "[vm]") {

  SECTION("Add") {
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitString("hello");
          builder->addLoadLitString(" ");
          builder->addAddString();
          builder->addLoadLitString("world");
          builder->addAddString();
          builder->addPrintString();
        },
        "hello world");
  }

  SECTION("Length") {
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitString("");
          builder->addLengthString();
          builder->addConvIntString();
          builder->addPrintString();
        },
        "0");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitString("hello");
          builder->addLengthString();
          builder->addConvIntString();
          builder->addPrintString();
        },
        "5");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitString("hello");
          builder->addLoadLitString(" ");
          builder->addAddString();
          builder->addLoadLitString("world");
          builder->addAddString();
          builder->addLengthString();
          builder->addConvIntString();
          builder->addPrintString();
        },
        "11");
  }
}

} // namespace vm
