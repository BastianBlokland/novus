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
          builder->addPop();
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
          builder->addPop();
        },
        "0");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitString("hello");
          builder->addLengthString();
          builder->addConvIntString();
          builder->addPrintString();
          builder->addPop();
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
          builder->addPop();
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
          builder->addPop();
        },
        "h");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitString("hello");
          builder->addLoadLitInt(4);
          builder->addIndexString();
          builder->addConvCharString();
          builder->addPrintString();
          builder->addPop();
        },
        "o");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitString("hello");
          builder->addLoadLitInt(-1);
          builder->addIndexString();
          builder->addConvIntString(); // NOTE: Using int to string conversion, is '\0' char.
          builder->addPrintString();
          builder->addPop();
        },
        "0");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitString("hello");
          builder->addLoadLitInt(5);
          builder->addIndexString();
          builder->addConvIntString(); // NOTE: Using int to string conversion, is '\0' char.
          builder->addPrintString();
          builder->addPop();
        },
        "0");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitString("");
          builder->addLoadLitInt(0);
          builder->addIndexString();
          builder->addConvIntString(); // NOTE: Using int to string conversion, is '\0' char.
          builder->addPrintString();
          builder->addPop();
        },
        "0");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitString(".");
          builder->addLoadLitInt(0);
          builder->addIndexString();
          builder->addConvCharString();
          builder->addPrintString();
          builder->addPop();
        },
        ".");
  }

  SECTION("Slicing") {
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitString("hello world");
          builder->addLoadLitInt(0);
          builder->addLoadLitInt(4);
          builder->addSliceString();
          builder->addPrintString();
          builder->addPop();
        },
        "hello");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitString("hello world");
          builder->addLoadLitInt(4);
          builder->addLoadLitInt(4);
          builder->addSliceString();
          builder->addPrintString();
          builder->addPop();
        },
        "o");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitString("hello world");
          builder->addLoadLitInt(-99);
          builder->addLoadLitInt(4);
          builder->addSliceString();
          builder->addPrintString();
          builder->addPop();
        },
        "hello");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitString("hello world");
          builder->addLoadLitInt(6);
          builder->addLoadLitInt(10);
          builder->addSliceString();
          builder->addPrintString();
          builder->addPop();
        },
        "world");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitString("hello world");
          builder->addLoadLitInt(6);
          builder->addLoadLitInt(99);
          builder->addSliceString();
          builder->addPrintString();
          builder->addPop();
        },
        "world");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitString("hello world");
          builder->addLoadLitInt(2);
          builder->addLoadLitInt(8);
          builder->addSliceString();
          builder->addPrintString();
          builder->addPop();
        },
        "llo wor");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitString("hello world");
          builder->addLoadLitInt(8);
          builder->addLoadLitInt(2);
          builder->addSliceString();
          builder->addPrintString();
          builder->addPop();
        },
        "l");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitString("hello world");
          builder->addLoadLitInt(0);
          builder->addLoadLitInt(10);
          builder->addSliceString();
          builder->addPrintString();
          builder->addPop();
        },
        "hello world");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitString("hello world");
          builder->addLoadLitInt(0);
          builder->addLoadLitInt(99);
          builder->addSliceString();
          builder->addPrintString();
          builder->addPop();
        },
        "hello world");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitString("");
          builder->addLoadLitInt(0);
          builder->addLoadLitInt(0);
          builder->addSliceString();
          builder->addPrintString();
          builder->addPop();
        },
        "");
  }
}
} // namespace vm
