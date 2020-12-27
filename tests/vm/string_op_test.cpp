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
          ADD_PRINT(asmb);
        },
        "input",
        "hello world");
  }

  SECTION("Combine chars") {
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitInt('a');
          asmb->addLoadLitInt('b');
          asmb->addCombineChar();

          ADD_PRINT(asmb);
        },
        "input",
        "ab");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitInt(256 + 'a');
          asmb->addLoadLitInt(256 + 'b');
          asmb->addCombineChar();

          ADD_PRINT(asmb);
        },
        "input",
        "ab");
  }

  SECTION("Append char") {
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitString("hello worl");
          asmb->addLoadLitInt('d');
          asmb->addAppendChar();

          ADD_PRINT(asmb);
        },
        "input",
        "hello world");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitString("");
          asmb->addLoadLitInt('h');
          asmb->addAppendChar();

          ADD_PRINT(asmb);
        },
        "input",
        "h");
  }

  SECTION("Length") {
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitString("");
          asmb->addLengthString();
          asmb->addConvIntString();
          ADD_PRINT(asmb);
        },
        "input",
        "0");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitString("hello");
          asmb->addLengthString();
          asmb->addConvIntString();
          ADD_PRINT(asmb);
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
          ADD_PRINT(asmb);
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
          ADD_PRINT(asmb);
        },
        "input",
        "h");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitString("hello");
          asmb->addLoadLitInt(4);
          asmb->addIndexString();
          asmb->addConvCharString();
          ADD_PRINT(asmb);
        },
        "input",
        "o");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitString("hello");
          asmb->addLoadLitInt(-1);
          asmb->addIndexString();
          asmb->addConvIntString(); // NOTE: Using int to string conversion, is '\0' char.
          ADD_PRINT(asmb);
        },
        "input",
        "0");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitString("hello");
          asmb->addLoadLitInt(5);
          asmb->addIndexString();
          asmb->addConvIntString(); // NOTE: Using int to string conversion, is '\0' char.
          ADD_PRINT(asmb);
        },
        "input",
        "0");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitString("");
          asmb->addLoadLitInt(0);
          asmb->addIndexString();
          asmb->addConvIntString(); // NOTE: Using int to string conversion, is '\0' char.
          ADD_PRINT(asmb);
        },
        "input",
        "0");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitString(".");
          asmb->addLoadLitInt(0);
          asmb->addIndexString();
          asmb->addConvCharString();
          ADD_PRINT(asmb);
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
          ADD_PRINT(asmb);
        },
        "input",
        "hello");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitString("hello world");
          asmb->addLoadLitInt(4);
          asmb->addLoadLitInt(4);
          asmb->addSliceString();
          ADD_PRINT(asmb);
        },
        "input",
        "");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitString("hello world");
          asmb->addLoadLitInt(4);
          asmb->addLoadLitInt(5);
          asmb->addSliceString();
          ADD_PRINT(asmb);
        },
        "input",
        "o");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitString("hello world");
          asmb->addLoadLitInt(-99);
          asmb->addLoadLitInt(5);
          asmb->addSliceString();
          ADD_PRINT(asmb);
        },
        "input",
        "hello");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitString("hello world");
          asmb->addLoadLitInt(1);
          asmb->addLoadLitInt(-1);
          asmb->addSliceString();
          ADD_PRINT(asmb);
        },
        "input",
        "");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitString("hello world");
          asmb->addLoadLitInt(6);
          asmb->addLoadLitInt(11);
          asmb->addSliceString();
          ADD_PRINT(asmb);
        },
        "input",
        "world");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitString("hello world");
          asmb->addLoadLitInt(6);
          asmb->addLoadLitInt(99);
          asmb->addSliceString();
          ADD_PRINT(asmb);
        },
        "input",
        "world");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitString("hello world");
          asmb->addLoadLitInt(2);
          asmb->addLoadLitInt(9);
          asmb->addSliceString();
          ADD_PRINT(asmb);
        },
        "input",
        "llo wor");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitString("hello world");
          asmb->addLoadLitInt(8);
          asmb->addLoadLitInt(3);
          asmb->addSliceString();
          ADD_PRINT(asmb);
        },
        "input",
        "");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitString("hello world");
          asmb->addLoadLitInt(0);
          asmb->addLoadLitInt(11);
          asmb->addSliceString();
          ADD_PRINT(asmb);
        },
        "input",
        "hello world");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitString("hello world");
          asmb->addLoadLitInt(0);
          asmb->addLoadLitInt(99);
          asmb->addSliceString();
          ADD_PRINT(asmb);
        },
        "input",
        "hello world");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitString("");
          asmb->addLoadLitInt(0);
          asmb->addLoadLitInt(0);
          asmb->addSliceString();
          ADD_PRINT(asmb);
        },
        "input",
        "");
  }

  SECTION("Unsigned chars") {
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitInt(128);
          asmb->addLoadLitInt(137);
          asmb->addCombineChar();

          asmb->addLoadLitInt(0);
          asmb->addIndexString();

          // Print as integer.
          asmb->addConvIntString();
          ADD_PRINT(asmb);
        },
        "input",
        "128");
  }
}

} // namespace vm
