#include "catch2/catch.hpp"
#include "helpers.hpp"

namespace backend {

TEST_CASE("Generate assembly for self call expressions", "[backend]") {

  SECTION("Function") {
    CHECK_PROG("fun f(int i) i < 0 ? self(0) : i ", [](backend::Builder* builder) -> void {
      // Function.
      builder->label("func");
      builder->addStackLoad(0); // Load arg 'i'.
      builder->addLoadLitInt(0);
      builder->addCheckLeInt();
      builder->addJumpIf("less");

      builder->addStackLoad(0); // Load arg 'i'.
      builder->addJump("end");

      builder->label("less");
      builder->addLoadLitInt(0);
      builder->addCall("func", 1, CallMode::Tail);

      builder->label("end");
      builder->addRet();

      // Entry point.
      builder->setEntrypoint("entry");
      builder->label("entry");
      builder->addRet();
    });
  }

  SECTION("Closure") {
    CHECK_PROG(
        "fun f(int i) lambda (bool b) b ? i : self(!b) ", [](backend::Builder* builder) -> void {
          // Function.
          builder->addStackLoad(0); // Load arg 'i'.
          builder->addLoadLitIp("lambda");
          builder->addMakeStruct(2); // Make closure containing 'i' and the lambda ip.
          builder->addRet();

          // Lambda.
          builder->label("lambda");
          builder->addStackLoad(0); // Load arg 'b'.
          builder->addJumpIf("bTrue");

          builder->addStackLoad(0);  // Load arg 'b'.
          builder->addLogicInvInt(); // !b
          builder->addStackLoad(1);  // Load closure arg 'i'.
          builder->addCall("lambda", 2, CallMode::Tail);
          builder->addJump("lambdaEnd");

          builder->label("bTrue");
          builder->addStackLoad(1); // Load closure arg 'i'.

          builder->label("lambdaEnd");
          builder->addRet();

          // Entry point.
          builder->setEntrypoint("entry");
          builder->label("entry");
          builder->addRet();
        });
  }
}

} // namespace backend
