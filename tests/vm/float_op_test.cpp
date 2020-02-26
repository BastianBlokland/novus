#include "catch2/catch.hpp"
#include "helpers.hpp"

namespace vm {

TEST_CASE("Execute float operations", "[vm]") {

  SECTION("Add") {
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitFloat(0.1337F); // NOLINT: Magic numbers
          asmb->addLoadLitFloat(0.1F);    // NOLINT: Magic numbers
          asmb->addAddFloat();
          asmb->addConvFloatString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "0.2337");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitFloat(9999.0F); // NOLINT: Magic numbers
          asmb->addLoadLitFloat(1.0F);
          asmb->addAddFloat();
          asmb->addConvFloatString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
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
          asmb->addConvFloatString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "0.0337");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitFloat(0.1F);    // NOLINT: Magic numbers
          asmb->addLoadLitFloat(0.1337F); // NOLINT: Magic numbers
          asmb->addSubFloat();
          asmb->addConvFloatString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
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
          asmb->addConvFloatString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "1337");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitFloat(1337.0F); // NOLINT: Magic numbers
          asmb->addLoadLitFloat(0.0001F); // NOLINT: Magic numbers
          asmb->addMulFloat();
          asmb->addConvFloatString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
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
          asmb->addConvFloatString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "0.02");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitFloat(1.0F);
          asmb->addLoadLitFloat(0.0F);
          asmb->addDivFloat();
          asmb->addConvFloatString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "inf");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitFloat(0.0F);
          asmb->addLoadLitFloat(0.0F);
          asmb->addDivFloat();
          asmb->addConvFloatString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
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
          asmb->addConvFloatString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "0.3");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitFloat(1.25F); // NOLINT: Magic numbers
          asmb->addLoadLitFloat(.25F);  // NOLINT: Magic numbers
          asmb->addModFloat();
          asmb->addConvFloatString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "0");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitFloat(1.3F); // NOLINT: Magic numbers
          asmb->addLoadLitFloat(-1.0F);
          asmb->addModFloat();
          asmb->addConvFloatString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "0.3");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitFloat(1.3F); // NOLINT: Magic numbers
          asmb->addLoadLitFloat(0.0F);
          asmb->addModFloat();
          asmb->addConvFloatString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
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
          asmb->addConvFloatString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "1");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitFloat(2.0F); // NOLINT: Magic numbers
          asmb->addLoadLitFloat(2.0F); // NOLINT: Magic numbers
          asmb->addPowFloat();
          asmb->addConvFloatString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "4");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitFloat(4.0F); // NOLINT: Magic numbers
          asmb->addLoadLitFloat(4.0F); // NOLINT: Magic numbers
          asmb->addPowFloat();
          asmb->addConvFloatString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "256");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitFloat(4.0F); // NOLINT: Magic numbers
          asmb->addLoadLitFloat(0.0F);
          asmb->addPowFloat();
          asmb->addConvFloatString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "1");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitFloat(-2.0F); // NOLINT: Magic numbers
          asmb->addLoadLitFloat(2.0F);  // NOLINT: Magic numbers
          asmb->addPowFloat();
          asmb->addConvFloatString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "4");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitFloat(2.0F);  // NOLINT: Magic numbers
          asmb->addLoadLitFloat(-2.0F); // NOLINT: Magic numbers
          asmb->addPowFloat();
          asmb->addConvFloatString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "0.25");
  }

  SECTION("Sqrt") {
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitFloat(2.0F); // NOLINT: Magic numbers
          asmb->addSqrtFloat();
          asmb->addConvFloatString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "1.41421");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitFloat(4.0F); // NOLINT: Magic numbers
          asmb->addSqrtFloat();
          asmb->addConvFloatString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "2");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitFloat(0.0F);
          asmb->addSqrtFloat();
          asmb->addConvFloatString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "0");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitFloat(-1.0F); // NOLINT: Magic numbers
          asmb->addSqrtFloat();
          asmb->addConvFloatString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "nan");
  }

  SECTION("Sin") {
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitFloat(0.0F);
          asmb->addSinFloat();
          asmb->addConvFloatString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "0");
  }

  SECTION("Cos") {
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitFloat(0.0F);
          asmb->addCosFloat();
          asmb->addConvFloatString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "1");
  }

  SECTION("Tan") {
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitFloat(0.0F);
          asmb->addTanFloat();
          asmb->addConvFloatString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
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
          asmb->addConvFloatString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
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
          asmb->addConvFloatString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
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
          asmb->addConvFloatString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
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
          asmb->addConvFloatString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "0");
  }

  SECTION("Negate") {
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitFloat(0.1337F); // NOLINT: Magic numbers
          asmb->addNegFloat();
          asmb->addConvFloatString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "-0.1337");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitFloat(0.0F);
          asmb->addNegFloat();
          asmb->addConvFloatString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "-0");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitFloat(0.1337F); // NOLINT: Magic numbers
          asmb->addNegFloat();
          asmb->addNegFloat();
          asmb->addConvFloatString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "0.1337");
  }
}

} // namespace vm
