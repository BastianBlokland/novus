#include "catch2/catch.hpp"
#include "helpers.hpp"
#include "vm/exceptions/call_stack_overflow.hpp"
#include "vm/exceptions/const_stack_overflow.hpp"
#include "vm/exceptions/eval_stack_not_empty.hpp"
#include "vm/exceptions/eval_stack_overflow.hpp"
#include "vm/exceptions/invalid_assembly.hpp"

namespace vm {

TEST_CASE("Runtime errors", "[vm]") {

  SECTION("InvalidAssembly") {
    CHECK_EXPR_THROWS(
        [](backend::Builder* builder) -> void { builder->addFail(); },
        vm::exceptions::InvalidAssembly);
  }

  SECTION("Evaluation stack not empty") {
    CHECK_EXPR_THROWS(
        [](backend::Builder* builder) -> void { builder->addLoadLitInt(1); },
        vm::exceptions::EvalStackNotEmpty);
  }

  SECTION("Call stack overflow") {
    CHECK_PROG_THROWS(
        [](backend::Builder* builder) -> void {
          builder->label("func");
          builder->addCall("func");

          builder->addEntryPoint("func");
        },
        vm::exceptions::CallStackOverflow);
  }

  SECTION("Evaluation stack overflow") {
    CHECK_EXPR_THROWS(
        [](backend::Builder* builder) -> void {
          builder->label("push1");
          builder->addLoadLitInt(1);
          builder->addJump("push1");
        },
        vm::exceptions::EvalStackOverflow);
  }

  SECTION("Constants stack overflow") {
    CHECK_PROG_THROWS(
        [](backend::Builder* builder) -> void {
          builder->label("func");
          builder->addReserveConsts(10);
          builder->addCall("func");

          builder->addEntryPoint("func");
        },
        vm::exceptions::ConstStackOverflow);
  }
}

} // namespace vm
