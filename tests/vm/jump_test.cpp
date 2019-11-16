#include "catch2/catch.hpp"
#include "helpers.hpp"

namespace vm {

TEST_CASE("Jump", "[vm]") {

  SECTION("Unconditional Jump") {
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addJump("print42");

          builder->addLoadLitInt(1337);
          builder->addPrintInt();

          builder->label("print42");
          builder->addLoadLitInt(42);
          builder->addPrintInt();
        },
        "42");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addJump("jump1");

          builder->label("jump3");
          builder->addLoadLitInt(1337);
          builder->addPrintInt();
          builder->addJump("end");

          builder->label("jump2");
          builder->addJump("jump3");

          builder->label("jump1");
          builder->addJump("jump2");

          builder->label("end");
        },
        "1337");
  }

  SECTION("Conditional Jump") {
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitInt(0);
          builder->addJumpIf("print42");

          builder->addLoadLitInt(1337);
          builder->addPrintInt();

          builder->label("print42");
          builder->addLoadLitInt(42);
          builder->addPrintInt();
        },
        "1337",
        "42");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitInt(1);
          builder->addJumpIf("print42");

          builder->addLoadLitInt(1337);
          builder->addPrintInt();

          builder->label("print42");
          builder->addLoadLitInt(42);
          builder->addPrintInt();
        },
        "42");
  }
}

} // namespace vm
