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
          builder->addPCall(vm::PCallCode::ConWriteString);
        },
        "input",
        "hello world");
  }

  SECTION("Length") {
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitString("");
          builder->addLengthString();
          builder->addConvIntString();
          builder->addPCall(vm::PCallCode::ConWriteString);
        },
        "input",
        "0");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitString("hello");
          builder->addLengthString();
          builder->addConvIntString();
          builder->addPCall(vm::PCallCode::ConWriteString);
        },
        "input",
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
          builder->addPCall(vm::PCallCode::ConWriteString);
        },
        "input",
        "11");
  }

  SECTION("Indexing") {
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitString("hello");
          builder->addLoadLitInt(0);
          builder->addIndexString();
          builder->addConvCharString();
          builder->addPCall(vm::PCallCode::ConWriteString);
        },
        "input",
        "h");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitString("hello");
          builder->addLoadLitInt(4);
          builder->addIndexString();
          builder->addConvCharString();
          builder->addPCall(vm::PCallCode::ConWriteString);
        },
        "input",
        "o");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitString("hello");
          builder->addLoadLitInt(-1);
          builder->addIndexString();
          builder->addConvIntString(); // NOTE: Using int to string conversion, is '\0' char.
          builder->addPCall(vm::PCallCode::ConWriteString);
        },
        "input",
        "0");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitString("hello");
          builder->addLoadLitInt(5);
          builder->addIndexString();
          builder->addConvIntString(); // NOTE: Using int to string conversion, is '\0' char.
          builder->addPCall(vm::PCallCode::ConWriteString);
        },
        "input",
        "0");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitString("");
          builder->addLoadLitInt(0);
          builder->addIndexString();
          builder->addConvIntString(); // NOTE: Using int to string conversion, is '\0' char.
          builder->addPCall(vm::PCallCode::ConWriteString);
        },
        "input",
        "0");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitString(".");
          builder->addLoadLitInt(0);
          builder->addIndexString();
          builder->addConvCharString();
          builder->addPCall(vm::PCallCode::ConWriteString);
        },
        "input",
        ".");
  }

  SECTION("Slicing") {
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitString("hello world");
          builder->addLoadLitInt(0);
          builder->addLoadLitInt(5);
          builder->addSliceString();
          builder->addPCall(vm::PCallCode::ConWriteString);
        },
        "input",
        "hello");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitString("hello world");
          builder->addLoadLitInt(4);
          builder->addLoadLitInt(4);
          builder->addSliceString();
          builder->addPCall(vm::PCallCode::ConWriteString);
        },
        "input",
        "");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitString("hello world");
          builder->addLoadLitInt(4);
          builder->addLoadLitInt(5);
          builder->addSliceString();
          builder->addPCall(vm::PCallCode::ConWriteString);
        },
        "input",
        "o");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitString("hello world");
          builder->addLoadLitInt(-99);
          builder->addLoadLitInt(5);
          builder->addSliceString();
          builder->addPCall(vm::PCallCode::ConWriteString);
        },
        "input",
        "hello");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitString("hello world");
          builder->addLoadLitInt(6);
          builder->addLoadLitInt(11);
          builder->addSliceString();
          builder->addPCall(vm::PCallCode::ConWriteString);
        },
        "input",
        "world");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitString("hello world");
          builder->addLoadLitInt(6);
          builder->addLoadLitInt(99);
          builder->addSliceString();
          builder->addPCall(vm::PCallCode::ConWriteString);
        },
        "input",
        "world");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitString("hello world");
          builder->addLoadLitInt(2);
          builder->addLoadLitInt(9);
          builder->addSliceString();
          builder->addPCall(vm::PCallCode::ConWriteString);
        },
        "input",
        "llo wor");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitString("hello world");
          builder->addLoadLitInt(8);
          builder->addLoadLitInt(3);
          builder->addSliceString();
          builder->addPCall(vm::PCallCode::ConWriteString);
        },
        "input",
        "");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitString("hello world");
          builder->addLoadLitInt(0);
          builder->addLoadLitInt(11);
          builder->addSliceString();
          builder->addPCall(vm::PCallCode::ConWriteString);
        },
        "input",
        "hello world");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitString("hello world");
          builder->addLoadLitInt(0);
          builder->addLoadLitInt(99);
          builder->addSliceString();
          builder->addPCall(vm::PCallCode::ConWriteString);
        },
        "input",
        "hello world");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitString("");
          builder->addLoadLitInt(0);
          builder->addLoadLitInt(0);
          builder->addSliceString();
          builder->addPCall(vm::PCallCode::ConWriteString);
        },
        "input",
        "");
  }
}
} // namespace vm
