#include "catch2/catch.hpp"
#include "helpers.hpp"

namespace backend {

TEST_CASE("Generate assembly for literals", "[backend]") {

  SECTION("Int literals") {
    CHECK_EXPR("42", [](novasm::Assembler* asmb) -> void { asmb->addLoadLitInt(42); });
  }

  SECTION("Long literals") {
    CHECK_EXPR("4200000000000L", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitLong(4'200'000'000'000L);
    });
  }

  SECTION("Float literals") {
    CHECK_EXPR(".1337", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitFloat(0.1337F); // NOLINT: Magic numbers
    });
  }

  SECTION("Bool literals") {
    CHECK_EXPR("false", [](novasm::Assembler* asmb) -> void { asmb->addLoadLitInt(0); });
    CHECK_EXPR("true", [](novasm::Assembler* asmb) -> void { asmb->addLoadLitInt(1); });
  }

  SECTION("String literals") {
    CHECK_EXPR("\"hello world\"", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitString("hello world");
    });
    CHECK_EXPR("\"\"", [](novasm::Assembler* asmb) -> void { asmb->addLoadLitString(""); });
  }

  SECTION("Char literals") {
    CHECK_EXPR("'a'", [](novasm::Assembler* asmb) -> void { asmb->addLoadLitInt('a'); });
  }
}

} // namespace backend
