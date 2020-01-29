#include "catch2/catch.hpp"
#include "helpers.hpp"

namespace vm {

TEST_CASE("Execute jump", "[vm]") {

  SECTION("Unconditional Jump") {
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addJump("write42");

          builder->addLoadLitString("1337");
          builder->addPCall(vm::PCallCode::ConWriteString);
          builder->addPop();

          builder->label("write42");
          builder->addLoadLitString("42");
          builder->addPCall(vm::PCallCode::ConWriteString);
          builder->addPop();
        },
        "input",
        "42");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addJump("jump1");

          builder->label("jump3");
          builder->addLoadLitString("1337");
          builder->addPCall(vm::PCallCode::ConWriteString);
          builder->addPop();
          builder->addJump("end");

          builder->label("jump2");
          builder->addJump("jump3");

          builder->label("jump1");
          builder->addJump("jump2");

          builder->label("end");
        },
        "input",
        "1337");
  }

  SECTION("Conditional Jump") {
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitInt(0);
          builder->addJumpIf("write42");

          builder->addLoadLitString("1337");
          builder->addPCall(vm::PCallCode::ConWriteString);
          builder->addPop();

          builder->label("write42");
          builder->addLoadLitString("42");
          builder->addPCall(vm::PCallCode::ConWriteString);
          builder->addPop();
        },
        "input",
        "1337",
        "42");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitInt(1);
          builder->addJumpIf("write42");

          builder->addLoadLitString("1337");
          builder->addPCall(vm::PCallCode::ConWriteString);
          builder->addPop();

          builder->label("write42");
          builder->addLoadLitString("42");
          builder->addPCall(vm::PCallCode::ConWriteString);
          builder->addPop();
        },
        "input",
        "42");
  }
}

} // namespace vm
