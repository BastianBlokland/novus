#include "catch2/catch.hpp"
#include "helpers.hpp"

namespace backend {

TEST_CASE("Generating assembly for enums", "[backend]") {

  SECTION("Create and convert to int") {
    CHECK_PROG(
        "enum E = a : 42 "
        "conWrite(string(E.a == 1337))",
        [](novasm::Assembler* asmb) -> void {
          // --- conWite statement start.
          asmb->label("prog");
          asmb->addLoadLitInt(42);
          asmb->addLoadLitInt(1337);
          asmb->addCheckEqInt();

          asmb->addConvBoolString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
          asmb->addRet();
          // --- conWite statement end.

          asmb->setEntrypoint("prog");
        });
  }

  SECTION("Bitwise ops") {
    CHECK_PROG(
        "enum E = a:0b01, b:0b10, ab:0b11 "
        "conWrite(string((E.a | E.b) == E.ab))",
        [](novasm::Assembler* asmb) -> void {
          // --- conWite statement start.
          asmb->label("prog");
          asmb->addLoadLitInt(0b01);
          asmb->addLoadLitInt(0b10);
          asmb->addOrInt();

          asmb->addLoadLitInt(0b11);
          asmb->addCheckEqInt();

          asmb->addConvBoolString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
          asmb->addRet();
          // --- conWite statement end.

          asmb->setEntrypoint("prog");
        });
  }

  SECTION("Convert from int") {
    CHECK_PROG(
        "enum E = a "
        "conWrite(string(E(0) == E.a))",
        [](novasm::Assembler* asmb) -> void {
          // --- conWite statement start.
          asmb->label("prog");
          asmb->addLoadLitInt(0); // Conversion is a no-op at runtime.
          asmb->addLoadLitInt(0);
          asmb->addCheckEqInt();

          asmb->addConvBoolString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
          asmb->addRet();
          // --- conWite statement end.

          asmb->setEntrypoint("prog");
        });
  }
}

} // namespace backend
