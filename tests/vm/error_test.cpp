#include "catch2/catch.hpp"
#include "helpers.hpp"
#include "vm/exceptions/invalid_assembly.hpp"
#include "vm/exceptions/stack_not_empty.hpp"
#include "vm/exceptions/stack_overflow.hpp"

namespace vm {

TEST_CASE("Runtime errors", "[vm]") {

  SECTION("InvalidAssembly") {
    CHECK_EXPR_THROWS(
        [](backend::Builder* builder) -> void { builder->addFail(); },
        vm::exceptions::InvalidAssembly);
  }

  SECTION("Stack not empty") {
    CHECK_EXPR_THROWS(
        [](backend::Builder* builder) -> void { builder->addLoadLitInt(1); },
        vm::exceptions::StackNotEmpty);
  }

  SECTION("Stack overflow") {
    CHECK_EXPR_THROWS(
        [](backend::Builder* builder) -> void {
          builder->label("push1");
          builder->addLoadLitInt(1);
          builder->addJump("push1");
        },
        vm::exceptions::StackOverflow);
  }
}

} // namespace vm
