#include "catch2/catch.hpp"
#include "helpers.hpp"

namespace backend {

TEST_CASE("Generating assembly for enums", "[backend]") {

  SECTION("Create and convert to int") {
    CHECK_PROG(
        "enum E = a : 42 "
        "print(E.a == 1337)",
        [](backend::Builder* builder) -> void {
          // --- Print statement start.
          builder->label("print");
          builder->addLoadLitInt(42);
          builder->addLoadLitInt(1337);
          builder->addCheckEqInt();

          builder->addConvBoolString();
          builder->addPCall(vm::PCallCode::PrintString);
          builder->addPop();
          builder->addRet();
          // --- Print statement end.

          builder->addEntryPoint("print");
        });
  }

  SECTION("Bitwise ops") {
    CHECK_PROG(
        "enum E = a:0b01, b:0b10, ab:0b11 "
        "print((E.a | E.b) == E.ab)",
        [](backend::Builder* builder) -> void {
          // --- Print statement start.
          builder->label("print");
          builder->addLoadLitInt(0b01);
          builder->addLoadLitInt(0b10);
          builder->addOrInt();

          builder->addLoadLitInt(0b11);
          builder->addCheckEqInt();

          builder->addConvBoolString();
          builder->addPCall(vm::PCallCode::PrintString);
          builder->addPop();
          builder->addRet();
          // --- Print statement end.

          builder->addEntryPoint("print");
        });
  }

  SECTION("Convert from int") {
    CHECK_PROG(
        "enum E = a "
        "print(toE(0) == E.a)",
        [](backend::Builder* builder) -> void {
          // --- Print statement start.
          builder->label("print");
          builder->addLoadLitInt(0); // Conversion is a no-op at runtime.
          builder->addLoadLitInt(0);
          builder->addCheckEqInt();

          builder->addConvBoolString();
          builder->addPCall(vm::PCallCode::PrintString);
          builder->addPop();
          builder->addRet();
          // --- Print statement end.

          builder->addEntryPoint("print");
        });
  }
}

} // namespace backend
