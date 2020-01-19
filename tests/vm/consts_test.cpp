#include "catch2/catch.hpp"
#include "helpers.hpp"

namespace vm {

TEST_CASE("Execute constants", "[vm]") {

  CHECK_EXPR(
      [](backend::Builder* builder) -> void {
        builder->addReserveConsts(1);
        builder->addLoadLitInt(42);
        builder->addStoreConst(0);

        builder->addLoadConst(0);
        builder->addConvIntString();
        builder->addPCall(vm::PCallCode::PrintString);
        builder->addPop();
      },
      "input",
      "42");

  CHECK_EXPR(
      [](backend::Builder* builder) -> void {
        builder->addReserveConsts(2);
        builder->addLoadLitInt(42);
        builder->addStoreConst(0);

        builder->addLoadLitInt(1337);
        builder->addStoreConst(1);

        builder->addLoadConst(0);
        builder->addConvIntString();
        builder->addPCall(vm::PCallCode::PrintString);
        builder->addPop();

        builder->addLoadConst(1);
        builder->addConvIntString();
        builder->addPCall(vm::PCallCode::PrintString);
        builder->addPop();
      },
      "input",
      "42",
      "1337");

  CHECK_EXPR(
      [](backend::Builder* builder) -> void {
        // First reserve one const.
        builder->addReserveConsts(1);
        builder->addLoadLitInt(42);
        builder->addStoreConst(0);

        // Now reserve two.
        builder->addReserveConsts(2);
        builder->addLoadLitInt(1337);
        builder->addStoreConst(1);

        builder->addLoadConst(0);
        builder->addConvIntString();
        builder->addPCall(vm::PCallCode::PrintString);
        builder->addPop();

        builder->addLoadConst(1);
        builder->addConvIntString();
        builder->addPCall(vm::PCallCode::PrintString);
        builder->addPop();
      },
      "input",
      "42",
      "1337");

  CHECK_EXPR(
      [](backend::Builder* builder) -> void {
        // First reserve three consts.
        builder->addReserveConsts(3);
        builder->addLoadLitInt(42);
        builder->addStoreConst(0);

        // Now reserve one (does nothing as we've already reserved more).
        builder->addReserveConsts(1);
        builder->addLoadLitInt(1337);
        builder->addStoreConst(1);

        builder->addLoadConst(0);
        builder->addConvIntString();
        builder->addPCall(vm::PCallCode::PrintString);
        builder->addPop();

        builder->addLoadConst(1);
        builder->addConvIntString();
        builder->addPCall(vm::PCallCode::PrintString);
        builder->addPop();
      },
      "input",
      "42",
      "1337");
}

} // namespace vm
