#include "catch2/catch.hpp"
#include "helpers.hpp"

namespace backend {

TEST_CASE("Generating assembly for switch expressions", "[backend]") {

  CHECK_EXPR_INT(
      "if true -> 10 "
      "else    -> 20",
      [](backend::Builder* builder) -> void {
        builder->addLoadLitInt(1);
        builder->addJumpIf("true");

        builder->addLoadLitInt(20);
        builder->addJump("end");

        builder->label("true");
        builder->addLoadLitInt(10);

        builder->label("end");
      });

  CHECK_EXPR_INT(
      "if true  -> 10 "
      "if true  -> 20 "
      "else     -> 30",
      [](backend::Builder* builder) -> void {
        builder->addLoadLitInt(1);
        builder->addJumpIf("if1");

        builder->addLoadLitInt(1);
        builder->addJumpIf("if2");

        builder->addLoadLitInt(30);
        builder->addJump("end");

        builder->label("if1");
        builder->addLoadLitInt(10);
        builder->addJump("end");

        builder->label("if2");
        builder->addLoadLitInt(20);

        builder->label("end");
      });
}

} // namespace backend
