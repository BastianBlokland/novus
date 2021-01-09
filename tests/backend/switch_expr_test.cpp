#include "catch2/catch.hpp"
#include "helpers.hpp"

namespace backend {

TEST_CASE("[backend] Generate assembly for switch expressions", "backend") {

  CHECK_EXPR(
      "if true -> 10 "
      "else    -> 20",
      [](novasm::Assembler* asmb) -> void {
        asmb->addLoadLitInt(1);
        asmb->addJumpIf("true");

        asmb->addLoadLitInt(20);
        asmb->addJump("end");

        asmb->label("true");
        asmb->addLoadLitInt(10);

        asmb->label("end");
      });

  CHECK_EXPR(
      "if true  -> 10 "
      "if true  -> 20 "
      "else     -> 30",
      [](novasm::Assembler* asmb) -> void {
        asmb->addLoadLitInt(1);
        asmb->addJumpIf("if1");

        asmb->addLoadLitInt(1);
        asmb->addJumpIf("if2");

        asmb->addLoadLitInt(30);
        asmb->addJump("end");

        asmb->label("if1");
        asmb->addLoadLitInt(10);
        asmb->addJump("end");

        asmb->label("if2");
        asmb->addLoadLitInt(20);

        asmb->label("end");
      });
}

} // namespace backend
