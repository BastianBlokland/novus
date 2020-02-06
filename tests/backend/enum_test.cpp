#include "catch2/catch.hpp"
#include "helpers.hpp"

namespace backend {

TEST_CASE("Generating assembly for enums", "[backend]") {

  SECTION("Create and convert to int") {
    CHECK_PROG(
        "enum E = a : 42 "
        "conWrite(string(E.a == 1337))",
        [](backend::Builder* builder) -> void {
          // --- conWite statement start.
          builder->label("prog");
          builder->addLoadLitInt(42);
          builder->addLoadLitInt(1337);
          builder->addCheckEqInt();

          builder->addConvBoolString();
          builder->addPCall(vm::PCallCode::ConWriteString);
          builder->addRet();
          // --- conWite statement end.

          builder->addEntryPoint("prog");
        });
  }

  SECTION("Bitwise ops") {
    CHECK_PROG(
        "enum E = a:0b01, b:0b10, ab:0b11 "
        "conWrite(string((E.a | E.b) == E.ab))",
        [](backend::Builder* builder) -> void {
          // --- conWite statement start.
          builder->label("prog");
          builder->addLoadLitInt(0b01);
          builder->addLoadLitInt(0b10);
          builder->addOrInt();

          builder->addLoadLitInt(0b11);
          builder->addCheckEqInt();

          builder->addConvBoolString();
          builder->addPCall(vm::PCallCode::ConWriteString);
          builder->addRet();
          // --- conWite statement end.

          builder->addEntryPoint("prog");
        });
  }

  SECTION("Convert from int") {
    CHECK_PROG(
        "enum E = a "
        "conWrite(string(E(0) == E.a))",
        [](backend::Builder* builder) -> void {
          // --- conWite statement start.
          builder->label("prog");
          builder->addLoadLitInt(0); // Conversion is a no-op at runtime.
          builder->addLoadLitInt(0);
          builder->addCheckEqInt();

          builder->addConvBoolString();
          builder->addPCall(vm::PCallCode::ConWriteString);
          builder->addRet();
          // --- conWite statement end.

          builder->addEntryPoint("prog");
        });
  }
}

} // namespace backend
