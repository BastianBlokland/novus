#include "catch2/catch.hpp"
#include "helpers.hpp"
#include "vm/result_code.hpp"

namespace vm {

TEST_CASE("Runtime errors", "[vm]") {

  SECTION("InvalidAssembly") {
    CHECK_EXPR_RESULTCODE(
        [](backend::Builder* builder) -> void { builder->addFail(); },
        "input",
        ResultCode::InvalidAssembly);
  }

  SECTION("Evaluation stack not empty") {
    CHECK_EXPR_RESULTCODE(
        [](backend::Builder* builder) -> void { builder->addLoadLitInt(1); },
        "input",
        ResultCode::EvalStackNotEmpty);
  }

  SECTION("Call stack overflow") {
    CHECK_PROG_RESULTCODE(
        [](backend::Builder* builder) -> void {
          builder->label("func");
          builder->addCall("func", false);

          builder->addEntryPoint("func");
        },
        "input",
        ResultCode::CallStackOverflow);
  }

  SECTION("Evaluation stack overflow") {
    CHECK_EXPR_RESULTCODE(
        [](backend::Builder* builder) -> void {
          builder->label("push1");
          builder->addLoadLitInt(1);
          builder->addJump("push1");
        },
        "input",
        ResultCode::EvalStackOverflow);
  }

  SECTION("Constants stack overflow") {
    CHECK_PROG_RESULTCODE(
        [](backend::Builder* builder) -> void {
          builder->label("func");
          builder->addReserveConsts(10);
          builder->addCall("func", false);

          builder->addEntryPoint("func");
        },
        "input",
        ResultCode::ConstStackOverflow);
  }
}

} // namespace vm
