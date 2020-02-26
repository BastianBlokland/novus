#include "catch2/catch.hpp"
#include "helpers.hpp"

namespace vm {

TEST_CASE("Execute string operations", "[vm]") {

  SECTION("Add") {
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitString("hello");
          asmb->addLoadLitString(" ");
          asmb->addAddString();
          asmb->addLoadLitString("world");
          asmb->addAddString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "hello world");
  }

  SECTION("Length") {
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitString("");
          asmb->addLengthString();
          asmb->addConvIntString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "0");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitString("hello");
          asmb->addLengthString();
          asmb->addConvIntString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "5");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitString("hello");
          asmb->addLoadLitString(" ");
          asmb->addAddString();
          asmb->addLoadLitString("world");
          asmb->addAddString();
          asmb->addLengthString();
          asmb->addConvIntString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "11");
  }

  SECTION("Indexing") {
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitString("hello");
          asmb->addLoadLitInt(0);
          asmb->addIndexString();
          asmb->addConvCharString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "h");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitString("hello");
          asmb->addLoadLitInt(4);
          asmb->addIndexString();
          asmb->addConvCharString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "o");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitString("hello");
          asmb->addLoadLitInt(-1);
          asmb->addIndexString();
          asmb->addConvIntString(); // NOTE: Using int to string conversion, is '\0' char.
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "0");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitString("hello");
          asmb->addLoadLitInt(5);
          asmb->addIndexString();
          asmb->addConvIntString(); // NOTE: Using int to string conversion, is '\0' char.
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "0");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitString("");
          asmb->addLoadLitInt(0);
          asmb->addIndexString();
          asmb->addConvIntString(); // NOTE: Using int to string conversion, is '\0' char.
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "0");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitString(".");
          asmb->addLoadLitInt(0);
          asmb->addIndexString();
          asmb->addConvCharString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        ".");
  }

  SECTION("Slicing") {
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitString("hello world");
          asmb->addLoadLitInt(0);
          asmb->addLoadLitInt(5);
          asmb->addSliceString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "hello");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitString("hello world");
          asmb->addLoadLitInt(4);
          asmb->addLoadLitInt(4);
          asmb->addSliceString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitString("hello world");
          asmb->addLoadLitInt(4);
          asmb->addLoadLitInt(5);
          asmb->addSliceString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "o");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitString("hello world");
          asmb->addLoadLitInt(-99);
          asmb->addLoadLitInt(5);
          asmb->addSliceString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "hello");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitString("hello world");
          asmb->addLoadLitInt(6);
          asmb->addLoadLitInt(11);
          asmb->addSliceString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "world");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitString("hello world");
          asmb->addLoadLitInt(6);
          asmb->addLoadLitInt(99);
          asmb->addSliceString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "world");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitString("hello world");
          asmb->addLoadLitInt(2);
          asmb->addLoadLitInt(9);
          asmb->addSliceString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "llo wor");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitString("hello world");
          asmb->addLoadLitInt(8);
          asmb->addLoadLitInt(3);
          asmb->addSliceString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitString("hello world");
          asmb->addLoadLitInt(0);
          asmb->addLoadLitInt(11);
          asmb->addSliceString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "hello world");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitString("hello world");
          asmb->addLoadLitInt(0);
          asmb->addLoadLitInt(99);
          asmb->addSliceString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "hello world");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitString("");
          asmb->addLoadLitInt(0);
          asmb->addLoadLitInt(0);
          asmb->addSliceString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "");
  }
}
} // namespace vm
