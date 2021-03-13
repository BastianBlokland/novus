#include "catch2/catch.hpp"
#include "helpers.hpp"

namespace backend {

TEST_CASE("[backend] Generate assembly for call expressions", "backend") {

  SECTION("Int operations") {
    CHECK_EXPR("-42", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitInt(42);
      asmb->addNegInt();
    });
    CHECK_EXPR("- -42", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitInt(42);
      asmb->addNegInt();
      asmb->addNegInt();
    });
    CHECK_EXPR("~42", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitInt(42);
      asmb->addInvInt();
    });
    CHECK_EXPR("1 + 3", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitInt(1);
      asmb->addLoadLitInt(3);
      asmb->addAddInt();
    });
    CHECK_EXPR("-1 + 3", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitInt(1);
      asmb->addNegInt();
      asmb->addLoadLitInt(3);
      asmb->addAddInt();
    });
    CHECK_EXPR("1 - 3", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitInt(1);
      asmb->addLoadLitInt(3);
      asmb->addSubInt();
    });
    CHECK_EXPR("1 * 3", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitInt(1);
      asmb->addLoadLitInt(3);
      asmb->addMulInt();
    });
    CHECK_EXPR("1 / 3", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitInt(1);
      asmb->addLoadLitInt(3);
      asmb->addDivInt();
    });
    CHECK_EXPR("1 % 3", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitInt(1);
      asmb->addLoadLitInt(3);
      asmb->addRemInt();
    });
    CHECK_EXPR("1 << 3", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitInt(1);
      asmb->addLoadLitInt(3);
      asmb->addShiftLeftInt();
    });
    CHECK_EXPR("1 >> 3", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitInt(1);
      asmb->addLoadLitInt(3);
      asmb->addShiftRightInt();
    });
    CHECK_EXPR("1 & 3", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitInt(1);
      asmb->addLoadLitInt(3);
      asmb->addAndInt();
    });
    CHECK_EXPR("1 | 3", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitInt(1);
      asmb->addLoadLitInt(3);
      asmb->addOrInt();
    });
    CHECK_EXPR("1 ^ 3", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitInt(1);
      asmb->addLoadLitInt(3);
      asmb->addXorInt();
    });
  }

  SECTION("Long operations") {
    CHECK_EXPR("-42L", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitLong(42);
      asmb->addNegLong();
    });
    CHECK_EXPR("- -42L", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitLong(42);
      asmb->addNegLong();
      asmb->addNegLong();
    });
    CHECK_EXPR("~42L", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitLong(42);
      asmb->addInvLong();
    });
    CHECK_EXPR("1L + 3L", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitLong(1);
      asmb->addLoadLitLong(3);
      asmb->addAddLong();
    });
    CHECK_EXPR("-1L + 3L", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitLong(1);
      asmb->addNegLong();
      asmb->addLoadLitLong(3);
      asmb->addAddLong();
    });
    CHECK_EXPR("1L - 3L", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitLong(1);
      asmb->addLoadLitLong(3);
      asmb->addSubLong();
    });
    CHECK_EXPR("1L * 3L", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitLong(1);
      asmb->addLoadLitLong(3);
      asmb->addMulLong();
    });
    CHECK_EXPR("1L / 3L", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitLong(1);
      asmb->addLoadLitLong(3);
      asmb->addDivLong();
    });
    CHECK_EXPR("1L % 3L", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitLong(1);
      asmb->addLoadLitLong(3);
      asmb->addRemLong();
    });
    CHECK_EXPR("1L << 3", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitLong(1);
      asmb->addLoadLitInt(3);
      asmb->addShiftLeftLong();
    });
    CHECK_EXPR("1L >> 3", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitLong(1);
      asmb->addLoadLitInt(3);
      asmb->addShiftRightLong();
    });
    CHECK_EXPR("1L & 3L", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitLong(1);
      asmb->addLoadLitLong(3);
      asmb->addAndLong();
    });
    CHECK_EXPR("1L | 3L", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitLong(1);
      asmb->addLoadLitLong(3);
      asmb->addOrLong();
    });
    CHECK_EXPR("1L ^ 3L", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitLong(1);
      asmb->addLoadLitLong(3);
      asmb->addXorLong();
    });
  }

  SECTION("Float operations") {
    CHECK_EXPR_FLOAT("-.1337", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitFloat(0.1337F);
      asmb->addNegFloat();
    });
    CHECK_EXPR_FLOAT("1.42 + 3.42", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitFloat(1.42F);
      asmb->addLoadLitFloat(3.42F);
      asmb->addAddFloat();
    });
    CHECK_EXPR_FLOAT("1.42 - 3.42", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitFloat(1.42F);
      asmb->addLoadLitFloat(3.42F);
      asmb->addSubFloat();
    });
    CHECK_EXPR_FLOAT("1.42 * 3.42", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitFloat(1.42F);
      asmb->addLoadLitFloat(3.42F);
      asmb->addMulFloat();
    });
    CHECK_EXPR_FLOAT("1.42 / 3.42", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitFloat(1.42F);
      asmb->addLoadLitFloat(3.42F);
      asmb->addDivFloat();
    });
    CHECK_EXPR_FLOAT("6.0 % 2.0", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitFloat(6.0F);
      asmb->addLoadLitFloat(2.0F);
      asmb->addModFloat();
    });
    CHECK_EXPR_FLOAT("intrinsic{float_sin}(2.0)", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitFloat(2.0F);
      asmb->addSinFloat();
    });
    CHECK_EXPR_FLOAT("intrinsic{float_cos}(2.0)", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitFloat(2.0F);
      asmb->addCosFloat();
    });
    CHECK_EXPR_FLOAT("intrinsic{float_tan}(2.0)", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitFloat(2.0F);
      asmb->addTanFloat();
    });
    CHECK_EXPR_FLOAT("intrinsic{float_asin}(2.0)", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitFloat(2.0F);
      asmb->addASinFloat();
    });
    CHECK_EXPR_FLOAT("intrinsic{float_acos}(2.0)", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitFloat(2.0F);
      asmb->addACosFloat();
    });
    CHECK_EXPR_FLOAT("intrinsic{float_atan}(2.0)", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitFloat(2.0F);
      asmb->addATanFloat();
    });
    CHECK_EXPR_FLOAT("intrinsic{float_atan2}(6.0, 2.0)", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitFloat(6.0F);
      asmb->addLoadLitFloat(2.0F);
      asmb->addATan2Float();
    });
  }

  SECTION("Int checks") {
    CHECK_EXPR_BOOL("intrinsic{int_eq_int}(1, 3)", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitInt(1);
      asmb->addLoadLitInt(3);
      asmb->addCheckEqInt();
    });
    CHECK_EXPR_BOOL("intrinsic{int_eq_int}(1, -3)", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitInt(1);
      asmb->addLoadLitInt(3);
      asmb->addNegInt();
      asmb->addCheckEqInt();
    });
    CHECK_EXPR("1 < 3", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitInt(1);
      asmb->addLoadLitInt(3);
      asmb->addCheckLeInt();
    });
    CHECK_EXPR("1 <= 3", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitInt(1);
      asmb->addLoadLitInt(3);
      asmb->addCheckGtInt();
      asmb->addCheckIntZero(); // Invert.
    });
    CHECK_EXPR("1 > 3", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitInt(1);
      asmb->addLoadLitInt(3);
      asmb->addCheckGtInt();
    });
    CHECK_EXPR("1 >= 3", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitInt(1);
      asmb->addLoadLitInt(3);
      asmb->addCheckLeInt();
      asmb->addCheckIntZero(); // Invert.
    });
  }

  SECTION("Long checks") {
    CHECK_EXPR_BOOL("intrinsic{long_eq_long}(1L, 3L)", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitLong(1);
      asmb->addLoadLitLong(3);
      asmb->addCheckEqLong();
    });
    CHECK_EXPR_BOOL("intrinsic{long_eq_long}(1L, -3L)", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitLong(1);
      asmb->addLoadLitLong(3);
      asmb->addNegLong();
      asmb->addCheckEqLong();
    });
    CHECK_EXPR("1L < 3L", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitLong(1);
      asmb->addLoadLitLong(3);
      asmb->addCheckLeLong();
    });
    CHECK_EXPR("1L <= 3L", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitLong(1);
      asmb->addLoadLitLong(3);
      asmb->addCheckGtLong();
      asmb->addCheckIntZero(); // Invert.
    });
    CHECK_EXPR("1L > 3L", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitLong(1);
      asmb->addLoadLitLong(3);
      asmb->addCheckGtLong();
    });
    CHECK_EXPR("1L >= 3L", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitLong(1);
      asmb->addLoadLitLong(3);
      asmb->addCheckLeLong();
      asmb->addCheckIntZero(); // Invert.
    });
  }

  SECTION("Float checks") {
    CHECK_EXPR_BOOL("intrinsic{float_eq_float}(1.42, 3.42)", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitFloat(1.42F);
      asmb->addLoadLitFloat(3.42F);
      asmb->addCheckEqFloat();
    });
    CHECK_EXPR("1.42 < 3.42", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitFloat(1.42F);
      asmb->addLoadLitFloat(3.42F);
      asmb->addCheckLeFloat();
    });
    CHECK_EXPR("1.42 <= 3.42", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitFloat(1.42F);
      asmb->addLoadLitFloat(3.42F);
      asmb->addCheckGtFloat();
      asmb->addCheckIntZero(); // Invert.
    });
    CHECK_EXPR("1.42 > 3.42", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitFloat(1.42F);
      asmb->addLoadLitFloat(3.42F);
      asmb->addCheckGtFloat();
    });
    CHECK_EXPR("1.42 >= 3.42", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitFloat(1.42F);
      asmb->addLoadLitFloat(3.42F);
      asmb->addCheckLeFloat();
      asmb->addCheckIntZero(); // Invert.
    });
  }

  SECTION("String operations") {
    CHECK_EXPR("\"hello\" + \"world\"", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitString("hello");
      asmb->addLoadLitString("world");
      asmb->addAddString();
    });

    CHECK_EXPR("\"hello worl\" + 'd'", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitString("hello worl");
      asmb->addLoadLitInt('d');
      asmb->addAppendChar();
    });

    CHECK_EXPR_INT("intrinsic{string_length}(\"hello\")", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitString("hello");
      asmb->addLengthString();
    });

    CHECK_EXPR("\"hello world\"[6]", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitString("hello world");
      asmb->addLoadLitInt(6);
      asmb->addIndexString();
    });
    CHECK_EXPR("\"hello world\"[0, 6]", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitString("hello world");
      asmb->addLoadLitInt(0);
      asmb->addLoadLitInt(6);
      asmb->addSliceString();
    });
  }

  SECTION("String checks") {
    CHECK_EXPR_BOOL(
        "intrinsic{string_eq_string}(\"hello\", \"world\")", [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitString("hello");
          asmb->addLoadLitString("world");
          asmb->addCheckEqString();
        });
  }

  SECTION("Conversions") {
    CHECK_EXPR_FLOAT("intrinsic{int_to_float}(42)", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitInt(42);
      asmb->addConvIntFloat();
    });
    CHECK_EXPR_INT("intrinsic{float_to_int}(42.1337)", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitFloat(42.1337F);
      asmb->addConvFloatInt();
    });
    CHECK_EXPR_INT("intrinsic{long_to_int}(42L)", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitLong(42);
      asmb->addConvLongInt();
    });
    CHECK_EXPR_LONG("intrinsic{int_to_long}(42)", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitInt(42);
      asmb->addConvIntLong();
    });
    CHECK_EXPR_LONG("intrinsic{char_to_long}('a')", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitInt('a');
      asmb->addConvIntLong();
    });
    CHECK_EXPR_STRING("intrinsic{int_to_string}(42)", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitInt(42);
      asmb->addConvIntString();
    });
    CHECK_EXPR_STRING("intrinsic{long_to_string}(42L)", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitLong(42);
      asmb->addConvLongString();
    });
    CHECK_EXPR_STRING("intrinsic{float_to_string}(.1337)", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitFloat(0.1337F);
      asmb->addConvFloatString();
    });
    CHECK_EXPR_STRING("intrinsic{char_to_string}('a')", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitInt('a');
      asmb->addConvCharString();
    });
    CHECK_EXPR_CHAR("intrinsic{int_to_char}(42)", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitInt(42);
      asmb->addConvIntChar();
    });
    CHECK_EXPR_CHAR("intrinsic{long_to_char}(42L)", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitLong(42);
      asmb->addConvLongChar();
    });
    CHECK_EXPR_CHAR("intrinsic{float_to_char}(42.42)", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitFloat(42.42F);
      asmb->addConvFloatChar();
    });
    CHECK_EXPR_LONG("intrinsic{float_to_long}(42.42)", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitFloat(42.42F);
      asmb->addConvFloatLong();
    });
  }

  SECTION("User functions") {
    CHECK_PROG(
        "fun funcA(int a, int b) -> bool intrinsic{int_eq_int}(a, b) "
        "fun test(bool b) b "
        "test(funcA(42, 1337))",
        [](novasm::Assembler* asmb) -> void {
          asmb->label("funcA");
          asmb->addStackLoad(0);
          asmb->addStackLoad(1);
          asmb->addCheckEqInt();
          asmb->addRet();

          // --- test function start.
          asmb->label("func-test");
          asmb->addStackLoad(0);
          asmb->addRet();
          // --- test function end.

          asmb->label("prog");
          asmb->addLoadLitInt(42);
          asmb->addLoadLitInt(1337);
          asmb->addCall("funcA", 2, novasm::CallMode::Normal);

          asmb->addCall("func-test", 1, novasm::CallMode::Normal);
          asmb->addRet();

          asmb->setEntrypoint("prog");
        });
  }
}

} // namespace backend
