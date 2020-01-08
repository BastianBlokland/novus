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

  SECTION("Indexing") {
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitString("hello");
          builder->addLoadLitInt(0);
          builder->addIndexString();
          builder->addConvCharString();
          builder->addPrintString();
        },
        "h");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitString("hello");
          builder->addLoadLitInt(4);
          builder->addIndexString();
          builder->addConvCharString();
          builder->addPrintString();
        },
        "o");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitString("hello");
          builder->addLoadLitInt(-1);
          builder->addIndexString();
          builder->addConvIntString(); // NOTE: Using int to string conversion, is '\0' char.
          builder->addPrintString();
        },
        "0");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitString("hello");
          builder->addLoadLitInt(5);
          builder->addIndexString();
          builder->addConvIntString(); // NOTE: Using int to string conversion, is '\0' char.
          builder->addPrintString();
        },
        "0");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitString("");
          builder->addLoadLitInt(0);
          builder->addIndexString();
          builder->addConvIntString(); // NOTE: Using int to string conversion, is '\0' char.
          builder->addPrintString();
        },
        "0");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitString(".");
          builder->addLoadLitInt(0);
          builder->addIndexString();
          builder->addConvCharString();
          builder->addPrintString();
        },
        ".");
  }
}

} // namespace vm
