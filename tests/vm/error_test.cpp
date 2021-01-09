#include "catch2/catch.hpp"
#include "helpers.hpp"
#include "vm/exec_state.hpp"

namespace vm {

TEST_CASE("[vm] Runtime errors", "vm") {

  SECTION("Fail") {
    CHECK_EXPR_RESULTCODE(
        [](novasm::Assembler* asmb) -> void { asmb->addFail(); }, "input", ExecState::Failed);
  }

  SECTION("Stack overflow") {
    CHECK_PROG_RESULTCODE(
        [](novasm::Assembler* asmb) -> void {
          asmb->label("func");
          asmb->addCall("func", 0, novasm::CallMode::Normal);

          asmb->setEntrypoint("func");
        },
        "input",
        ExecState::StackOverflow);
  }

  SECTION("Stack overflow") {
    CHECK_EXPR_RESULTCODE(
        [](novasm::Assembler* asmb) -> void {
          asmb->label("push1");
          asmb->addLoadLitInt(1);
          asmb->addJump("push1");
        },
        "input",
        ExecState::StackOverflow);
  }

  SECTION("Stack overflow") {
    CHECK_PROG_RESULTCODE(
        [](novasm::Assembler* asmb) -> void {
          asmb->label("func");
          asmb->addStackAlloc(10);
          asmb->addCall("func", 0, novasm::CallMode::Normal);

          asmb->setEntrypoint("func");
        },
        "input",
        ExecState::StackOverflow);
  }
}

} // namespace vm
