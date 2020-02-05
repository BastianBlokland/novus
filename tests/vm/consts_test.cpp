#include "catch2/catch.hpp"
#include "helpers.hpp"

namespace vm {

TEST_CASE("Execute constants", "[vm]") {

  CHECK_EXPR(
      [](backend::Builder* builder) -> void {
        builder->addStackAlloc(1);
        builder->addLoadLitInt(42);
        builder->addStackStore(0);

        builder->addStackLoad(0);
        builder->addConvIntString();
        builder->addPCall(vm::PCallCode::ConWriteString);
        builder->addPop();
      },
      "input",
      "42");

  CHECK_EXPR(
      [](backend::Builder* builder) -> void {
        builder->addStackAlloc(2);
        builder->addLoadLitInt(42);
        builder->addStackStore(0);

        builder->addLoadLitInt(1337);
        builder->addStackStore(1);

        builder->addStackLoad(0);
        builder->addConvIntString();
        builder->addPCall(vm::PCallCode::ConWriteString);
        builder->addPop();

        builder->addStackLoad(1);
        builder->addConvIntString();
        builder->addPCall(vm::PCallCode::ConWriteString);
        builder->addPop();
      },
      "input",
      "42",
      "1337");

  CHECK_EXPR(
      [](backend::Builder* builder) -> void {
        // First reserve one const.
        builder->addStackAlloc(1);
        builder->addLoadLitInt(42);
        builder->addStackStore(0);

        // Now reserve two.
        builder->addStackAlloc(2);
        builder->addLoadLitInt(1337);
        builder->addStackStore(1);

        builder->addStackLoad(0);
        builder->addConvIntString();
        builder->addPCall(vm::PCallCode::ConWriteString);
        builder->addPop();

        builder->addStackLoad(1);
        builder->addConvIntString();
        builder->addPCall(vm::PCallCode::ConWriteString);
        builder->addPop();
      },
      "input",
      "42",
      "1337");

  CHECK_EXPR(
      [](backend::Builder* builder) -> void {
        // First reserve three consts.
        builder->addStackAlloc(3);
        builder->addLoadLitInt(42);
        builder->addStackStore(0);

        // Now reserve one (does nothing as we've already reserved more).
        builder->addStackAlloc(1);
        builder->addLoadLitInt(1337);
        builder->addStackStore(1);

        builder->addStackLoad(0);
        builder->addConvIntString();
        builder->addPCall(vm::PCallCode::ConWriteString);
        builder->addPop();

        builder->addStackLoad(1);
        builder->addConvIntString();
        builder->addPCall(vm::PCallCode::ConWriteString);
        builder->addPop();
      },
      "input",
      "42",
      "1337");
}

} // namespace vm
