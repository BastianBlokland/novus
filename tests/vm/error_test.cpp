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

  SECTION("Stack overflow") {
    CHECK_PROG_RESULTCODE(
        [](backend::Builder* builder) -> void {
          builder->label("func");
          builder->addCall("func", 0, false);

          builder->addEntryPoint("func");
        },
        "input",
        ResultCode::StackOverflow);
  }

  SECTION("Stack overflow") {
    CHECK_EXPR_RESULTCODE(
        [](backend::Builder* builder) -> void {
          builder->label("push1");
          builder->addLoadLitInt(1);
          builder->addJump("push1");
        },
        "input",
        ResultCode::StackOverflow);
  }

  SECTION("Stack overflow") {
    CHECK_PROG_RESULTCODE(
        [](backend::Builder* builder) -> void {
          builder->label("func");
          builder->addStackAlloc(10);
          builder->addCall("func", 0, false);

          builder->addEntryPoint("func");
        },
        "input",
        ResultCode::StackOverflow);
  }
}

} // namespace vm
