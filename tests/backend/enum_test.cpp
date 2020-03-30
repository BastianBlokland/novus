#include "catch2/catch.hpp"
#include "helpers.hpp"

namespace backend {

TEST_CASE("Generating assembly for enums", "[backend]") {

  SECTION("Create and convert to int") {
    CHECK_PROG(
        "enum E = a : 42 "
        "assert(E.a == 1337, \"test\")",
        [](novasm::Assembler* asmb) -> void {
          // --- conWite statement start.
          asmb->label("prog");
          asmb->addLoadLitInt(42);
          asmb->addLoadLitInt(1337);
          asmb->addCheckEqInt();

          asmb->addLoadLitString("test");
          asmb->addPCall(novasm::PCallCode::Assert);
          asmb->addRet();
          // --- conWite statement end.

          asmb->setEntrypoint("prog");
        });
  }

  SECTION("Bitwise ops") {
    CHECK_PROG(
        "enum E = a:0b01, b:0b10, ab:0b11 "
        "assert((E.a | E.b) == E.ab, \"test\")",
        [](novasm::Assembler* asmb) -> void {
          // --- conWite statement start.
          asmb->label("prog");
          asmb->addLoadLitInt(0b01);
          asmb->addLoadLitInt(0b10);
          asmb->addOrInt();

          asmb->addLoadLitInt(0b11);
          asmb->addCheckEqInt();

          asmb->addLoadLitString("test");
          asmb->addPCall(novasm::PCallCode::Assert);
          asmb->addRet();
          // --- conWite statement end.

          asmb->setEntrypoint("prog");
        });
  }

  SECTION("Convert from int") {
    CHECK_PROG(
        "enum E = a "
        "assert(E(0) == E.a, \"test\")",
        [](novasm::Assembler* asmb) -> void {
          // --- conWite statement start.
          asmb->label("prog");
          asmb->addLoadLitInt(0); // Conversion is a no-op at runtime.
          asmb->addLoadLitInt(0);
          asmb->addCheckEqInt();

          asmb->addLoadLitString("test");
          asmb->addPCall(novasm::PCallCode::Assert);
          asmb->addRet();
          // --- conWite statement end.

          asmb->setEntrypoint("prog");
        });
  }
}

} // namespace backend
