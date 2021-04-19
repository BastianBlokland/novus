#include "catch2/catch.hpp"
#include "helpers.hpp"

namespace vm {

TEST_CASE("[vm] Execute float operations", "vm") {

  SECTION("Add") {
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitFloat(0.1337F); // NOLINT: Magic numbers
          asmb->addLoadLitFloat(0.1F);    // NOLINT: Magic numbers
          asmb->addAddFloat();
          ADD_FLOAT_TO_STRING(asmb, 6, 0);
          ADD_PRINT(asmb);
        },
        "input",
        "0.2337");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitFloat(9999.0F); // NOLINT: Magic numbers
          asmb->addLoadLitFloat(1.0F);
          asmb->addAddFloat();
          ADD_FLOAT_TO_STRING(asmb, 6, 0);
          ADD_PRINT(asmb);
        },
        "input",
        "10000");
  }

  SECTION("Substract") {
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitFloat(0.1337F); // NOLINT: Magic numbers
          asmb->addLoadLitFloat(0.1F);    // NOLINT: Magic numbers
          asmb->addSubFloat();
          ADD_FLOAT_TO_STRING(asmb, 6, 0);
          ADD_PRINT(asmb);
        },
        "input",
        "0.0337");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitFloat(0.1F);    // NOLINT: Magic numbers
          asmb->addLoadLitFloat(0.1337F); // NOLINT: Magic numbers
          asmb->addSubFloat();
          ADD_FLOAT_TO_STRING(asmb, 6, 0);
          ADD_PRINT(asmb);
        },
        "input",
        "-0.0337");
  }

  SECTION("Multiply") {
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitFloat(0.1337F);  // NOLINT: Magic numbers
          asmb->addLoadLitFloat(10000.0F); // NOLINT: Magic numbers
          asmb->addMulFloat();
          ADD_FLOAT_TO_STRING(asmb, 6, 0);
          ADD_PRINT(asmb);
        },
        "input",
        "1337");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitFloat(1337.0F); // NOLINT: Magic numbers
          asmb->addLoadLitFloat(0.0001F); // NOLINT: Magic numbers
          asmb->addMulFloat();
          ADD_FLOAT_TO_STRING(asmb, 6, 0);
          ADD_PRINT(asmb);
        },
        "input",
        "0.1337");
  }

  SECTION("Divide") {
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitFloat(1.0F);
          asmb->addLoadLitFloat(50.0F); // NOLINT: Magic numbers
          asmb->addDivFloat();
          ADD_FLOAT_TO_STRING(asmb, 6, 0);
          ADD_PRINT(asmb);
        },
        "input",
        "0.02");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitFloat(1.0F);
          asmb->addLoadLitFloat(0.0F);
          asmb->addDivFloat();
          ADD_FLOAT_TO_STRING(asmb, 6, 0);
          ADD_PRINT(asmb);
        },
        "input",
        "inf");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitFloat(0.0F);
          asmb->addLoadLitFloat(0.0F);
          asmb->addDivFloat();
          ADD_FLOAT_TO_STRING(asmb, 6, 0);
          ADD_PRINT(asmb);
        },
        "input",
        "nan");
  }

  SECTION("Mod") {
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitFloat(1.3F); // NOLINT: Magic numbers
          asmb->addLoadLitFloat(1.0F);
          asmb->addModFloat();
          ADD_FLOAT_TO_STRING(asmb, 6, 0);
          ADD_PRINT(asmb);
        },
        "input",
        "0.3");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitFloat(1.25F); // NOLINT: Magic numbers
          asmb->addLoadLitFloat(.25F);  // NOLINT: Magic numbers
          asmb->addModFloat();
          ADD_FLOAT_TO_STRING(asmb, 6, 0);
          ADD_PRINT(asmb);
        },
        "input",
        "0");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitFloat(1.3F); // NOLINT: Magic numbers
          asmb->addLoadLitFloat(-1.0F);
          asmb->addModFloat();
          ADD_FLOAT_TO_STRING(asmb, 6, 0);
          ADD_PRINT(asmb);
        },
        "input",
        "0.3");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitFloat(1.3F); // NOLINT: Magic numbers
          asmb->addLoadLitFloat(0.0F);
          asmb->addModFloat();
          ADD_FLOAT_TO_STRING(asmb, 6, 0);
          ADD_PRINT(asmb);
        },
        "input",
        "nan");
  }

  SECTION("Pow") {
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitFloat(1.0F);
          asmb->addLoadLitFloat(2.0F); // NOLINT: Magic numbers
          asmb->addPowFloat();
          ADD_FLOAT_TO_STRING(asmb, 6, 0);
          ADD_PRINT(asmb);
        },
        "input",
        "1");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitFloat(2.0F); // NOLINT: Magic numbers
          asmb->addLoadLitFloat(2.0F); // NOLINT: Magic numbers
          asmb->addPowFloat();
          ADD_FLOAT_TO_STRING(asmb, 6, 0);
          ADD_PRINT(asmb);
        },
        "input",
        "4");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitFloat(4.0F); // NOLINT: Magic numbers
          asmb->addLoadLitFloat(4.0F); // NOLINT: Magic numbers
          asmb->addPowFloat();
          ADD_FLOAT_TO_STRING(asmb, 6, 0);
          ADD_PRINT(asmb);
        },
        "input",
        "256");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitFloat(4.0F); // NOLINT: Magic numbers
          asmb->addLoadLitFloat(0.0F);
          asmb->addPowFloat();
          ADD_FLOAT_TO_STRING(asmb, 6, 0);
          ADD_PRINT(asmb);
        },
        "input",
        "1");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitFloat(-2.0F); // NOLINT: Magic numbers
          asmb->addLoadLitFloat(2.0F);  // NOLINT: Magic numbers
          asmb->addPowFloat();
          ADD_FLOAT_TO_STRING(asmb, 6, 0);
          ADD_PRINT(asmb);
        },
        "input",
        "4");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitFloat(2.0F);  // NOLINT: Magic numbers
          asmb->addLoadLitFloat(-2.0F); // NOLINT: Magic numbers
          asmb->addPowFloat();
          ADD_FLOAT_TO_STRING(asmb, 6, 0);
          ADD_PRINT(asmb);
        },
        "input",
        "0.25");
  }

  SECTION("Sqrt") {
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitFloat(2.0F); // NOLINT: Magic numbers
          asmb->addSqrtFloat();
          ADD_FLOAT_TO_STRING(asmb, 6, 0);
          ADD_PRINT(asmb);
        },
        "input",
        "1.41421");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitFloat(4.0F); // NOLINT: Magic numbers
          asmb->addSqrtFloat();
          ADD_FLOAT_TO_STRING(asmb, 6, 0);
          ADD_PRINT(asmb);
        },
        "input",
        "2");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitFloat(0.0F);
          asmb->addSqrtFloat();
          ADD_FLOAT_TO_STRING(asmb, 6, 0);
          ADD_PRINT(asmb);
        },
        "input",
        "0");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitFloat(-1.0F); // NOLINT: Magic numbers
          asmb->addSqrtFloat();
          ADD_FLOAT_TO_STRING(asmb, 6, 0);
          ADD_PRINT(asmb);
        },
        "input",
        "nan");
  }

  SECTION("Sin") {
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitFloat(0.0F);
          asmb->addSinFloat();
          ADD_FLOAT_TO_STRING(asmb, 6, 0);
          ADD_PRINT(asmb);
        },
        "input",
        "0");
  }

  SECTION("Cos") {
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitFloat(0.0F);
          asmb->addCosFloat();
          ADD_FLOAT_TO_STRING(asmb, 6, 0);
          ADD_PRINT(asmb);
        },
        "input",
        "1");
  }

  SECTION("Tan") {
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitFloat(0.0F);
          asmb->addTanFloat();
          ADD_FLOAT_TO_STRING(asmb, 6, 0);
          ADD_PRINT(asmb);
        },
        "input",
        "0");
  }

  SECTION("ASin") {
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitFloat(1.0F);
          asmb->addSinFloat();
          asmb->addASinFloat();
          ADD_FLOAT_TO_STRING(asmb, 6, 0);
          ADD_PRINT(asmb);
        },
        "input",
        "1");
  }

  SECTION("ACos") {
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitFloat(1.0F);
          asmb->addCosFloat();
          asmb->addACosFloat();
          ADD_FLOAT_TO_STRING(asmb, 6, 0);
          ADD_PRINT(asmb);
        },
        "input",
        "1");
  }

  SECTION("ATan") {
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitFloat(1.0F);
          asmb->addTanFloat();
          asmb->addATanFloat();
          ADD_FLOAT_TO_STRING(asmb, 6, 0);
          ADD_PRINT(asmb);
        },
        "input",
        "1");
  }

  SECTION("ATan2") {
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitFloat(0.0F);
          asmb->addLoadLitFloat(0.0F);
          asmb->addATan2Float();
          ADD_FLOAT_TO_STRING(asmb, 6, 0);
          ADD_PRINT(asmb);
        },
        "input",
        "0");
  }

  SECTION("Negate") {
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitFloat(0.1337F); // NOLINT: Magic numbers
          asmb->addNegFloat();
          ADD_FLOAT_TO_STRING(asmb, 6, 0);
          ADD_PRINT(asmb);
        },
        "input",
        "-0.1337");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitFloat(0.0F);
          asmb->addNegFloat();
          ADD_FLOAT_TO_STRING(asmb, 6, 0);
          ADD_PRINT(asmb);
        },
        "input",
        "-0");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitFloat(0.1337F); // NOLINT: Magic numbers
          asmb->addNegFloat();
          asmb->addNegFloat();
          ADD_FLOAT_TO_STRING(asmb, 6, 0);
          ADD_PRINT(asmb);
        },
        "input",
        "0.1337");
  }
}

} // namespace vm
