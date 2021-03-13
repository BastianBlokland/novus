#include "catch2/catch.hpp"
#include "helpers.hpp"

namespace backend {

TEST_CASE("[backend] Generate assembly for enums", "backend") {

  SECTION("Create and convert to int") {
    CHECK_PROG(
        "enum E = a : 42 "
        "fun test(bool b) b "
        "test(intrinsic{int_eq_int}(E.a, 1337))",
        [](novasm::Assembler* asmb) -> void {
          // --- test function start.
          asmb->label("func-test");
          asmb->addStackLoad(0);
          asmb->addRet();
          // --- test function end.

          // --- exec statement start.
          asmb->label("prog");
          asmb->addLoadLitInt(42);
          asmb->addLoadLitInt(1337);
          asmb->addCheckEqInt();

          asmb->addCall("func-test", 1, novasm::CallMode::Normal);
          asmb->addRet();
          // --- exec statement end.

          asmb->setEntrypoint("prog");
        });
  }

  SECTION("Bitwise ops") {
    CHECK_PROG(
        "enum E = a:0b01, b:0b10, ab:0b11 "
        "fun test(bool b) b "
        "test(intrinsic{int_eq_int}(E.a | E.b, E.ab))",
        [](novasm::Assembler* asmb) -> void {
          // --- test function start.
          asmb->label("func-test");
          asmb->addStackLoad(0);
          asmb->addRet();
          // --- test function end.

          // --- exec statement start.
          asmb->label("prog");
          asmb->addLoadLitInt(0b01);
          asmb->addLoadLitInt(0b10);
          asmb->addOrInt();

          asmb->addLoadLitInt(0b11);
          asmb->addCheckEqInt();

          asmb->addCall("func-test", 1, novasm::CallMode::Normal);
          asmb->addRet();
          // --- exec statement end.

          asmb->setEntrypoint("prog");
        });
  }

  SECTION("Convert from int") {
    CHECK_PROG(
        "enum E = a "
        "fun test(bool b) b "
        "test(intrinsic{int_eq_int}(E(0), E.a))",
        [](novasm::Assembler* asmb) -> void {
          // --- test function start.
          asmb->label("func-test");
          asmb->addStackLoad(0);
          asmb->addRet();
          // --- test function end.

          // --- exec statement start.
          asmb->label("prog");
          asmb->addLoadLitInt(0); // Conversion is a no-op at runtime.
          asmb->addLoadLitInt(0);
          asmb->addCheckEqInt();

          asmb->addCall("func-test", 1, novasm::CallMode::Normal);
          asmb->addRet();
          // --- exec statement end.

          asmb->setEntrypoint("prog");
        });
  }
}

} // namespace backend
