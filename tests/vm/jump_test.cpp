#include "catch2/catch.hpp"
#include "helpers.hpp"

namespace vm {

TEST_CASE("Execute jump", "[vm]") {

  SECTION("Unconditional Jump") {
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addJump("write42");

          asmb->addLoadLitString("1337");
          asmb->addPCall(novasm::PCallCode::ConWriteString);
          asmb->addPop();

          asmb->label("write42");
          asmb->addLoadLitString("42");
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "42");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addJump("jump1");

          asmb->label("jump3");
          asmb->addLoadLitString("1337");
          asmb->addPCall(novasm::PCallCode::ConWriteString);
          asmb->addJump("end");

          asmb->label("jump2");
          asmb->addJump("jump3");

          asmb->label("jump1");
          asmb->addJump("jump2");

          asmb->label("end");
        },
        "input",
        "1337");
  }

  SECTION("Conditional Jump") {
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitInt(0);
          asmb->addJumpIf("write42");

          asmb->addLoadLitString("1337");
          asmb->addPCall(novasm::PCallCode::ConWriteString);
          asmb->addPop();

          asmb->label("write42");
          asmb->addLoadLitString("42");
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "1337",
        "42");
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitInt(1);
          asmb->addJumpIf("write42");

          asmb->addLoadLitString("1337");
          asmb->addPCall(novasm::PCallCode::ConWriteString);
          asmb->addPop();

          asmb->label("write42");
          asmb->addLoadLitString("42");
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "42");
  }
}

} // namespace vm
