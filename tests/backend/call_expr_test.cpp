#include "catch2/catch.hpp"
#include "helpers.hpp"

namespace backend {

TEST_CASE("[backend] Generate assembly for call expressions", "backend") {

  SECTION("Int operations") {
    CHECK_EXPR("-42", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitInt(42);
      asmb->addNegInt();
    });
    CHECK_EXPR("+42", [](novasm::Assembler* asmb) -> void { asmb->addLoadLitInt(42); });
    CHECK_EXPR("- -42", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitInt(42);
      asmb->addNegInt();
      asmb->addNegInt();
    });
    CHECK_EXPR("~42", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitInt(42);
      asmb->addInvInt();
    });
    CHECK_EXPR("--42", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitInt(42);
      asmb->addLoadLitInt(1);
      asmb->addSubInt();
    });
    CHECK_EXPR("++42", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitInt(42);
      asmb->addLoadLitInt(1);
      asmb->addAddInt();
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
    CHECK_EXPR("+42L", [](novasm::Assembler* asmb) -> void { asmb->addLoadLitLong(42); });
    CHECK_EXPR("- -42L", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitLong(42);
      asmb->addNegLong();
      asmb->addNegLong();
    });
    CHECK_EXPR("~42L", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitLong(42);
      asmb->addInvLong();
    });
    CHECK_EXPR("--42L", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitLong(42);
      asmb->addLoadLitLong(1);
      asmb->addSubLong();
    });
    CHECK_EXPR("++42L", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitLong(42);
      asmb->addLoadLitLong(1);
      asmb->addAddLong();
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
    CHECK_EXPR("-.1337", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitFloat(0.1337F); // NOLINT: Magic numbers
      asmb->addNegFloat();
    });
    CHECK_EXPR("+.1337", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitFloat(0.1337F); // NOLINT: Magic numbers
    });
    CHECK_EXPR("--.1337", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitFloat(0.1337F); // NOLINT: Magic numbers
      asmb->addLoadLitFloat(1.0F);
      asmb->addSubFloat();
    });
    CHECK_EXPR("++.1337", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitFloat(0.1337F); // NOLINT: Magic numbers
      asmb->addLoadLitFloat(1.0F);
      asmb->addAddFloat();
    });
    CHECK_EXPR("1.42 + 3.42", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitFloat(1.42F); // NOLINT: Magic numbers
      asmb->addLoadLitFloat(3.42F); // NOLINT: Magic numbers
      asmb->addAddFloat();
    });
    CHECK_EXPR("1.42 - 3.42", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitFloat(1.42F); // NOLINT: Magic numbers
      asmb->addLoadLitFloat(3.42F); // NOLINT: Magic numbers
      asmb->addSubFloat();
    });
    CHECK_EXPR("1.42 * 3.42", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitFloat(1.42F); // NOLINT: Magic numbers
      asmb->addLoadLitFloat(3.42F); // NOLINT: Magic numbers
      asmb->addMulFloat();
    });
    CHECK_EXPR("1.42 / 3.42", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitFloat(1.42F); // NOLINT: Magic numbers
      asmb->addLoadLitFloat(3.42F); // NOLINT: Magic numbers
      asmb->addDivFloat();
    });
    CHECK_EXPR("1.0 / 2", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitFloat(1.0F);
      asmb->addLoadLitInt(2);
      asmb->addConvIntFloat();
      asmb->addDivFloat();
    });
    CHECK_EXPR("1 / 2.0", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitInt(1);
      asmb->addConvIntFloat();
      asmb->addLoadLitFloat(2.0F); // NOLINT: Magic numbers
      asmb->addDivFloat();
    });
    CHECK_EXPR("6.0 % 2.0", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitFloat(6.0F); // NOLINT: Magic numbers
      asmb->addLoadLitFloat(2.0F); // NOLINT: Magic numbers
      asmb->addModFloat();
    });
    CHECK_EXPR("float(intrinsic{float_sin}(2.0))", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitFloat(2.0F); // NOLINT: Magic numbers
      asmb->addSinFloat();
    });
    CHECK_EXPR("float(intrinsic{float_cos}(2.0))", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitFloat(2.0F); // NOLINT: Magic numbers
      asmb->addCosFloat();
    });
    CHECK_EXPR("float(intrinsic{float_tan}(2.0))", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitFloat(2.0F); // NOLINT: Magic numbers
      asmb->addTanFloat();
    });
    CHECK_EXPR("float(intrinsic{float_asin}(2.0))", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitFloat(2.0F); // NOLINT: Magic numbers
      asmb->addASinFloat();
    });
    CHECK_EXPR("float(intrinsic{float_acos}(2.0))", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitFloat(2.0F); // NOLINT: Magic numbers
      asmb->addACosFloat();
    });
    CHECK_EXPR("float(intrinsic{float_atan}(2.0))", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitFloat(2.0F); // NOLINT: Magic numbers
      asmb->addATanFloat();
    });
    CHECK_EXPR("float(intrinsic{float_atan2}(6.0, 2.0))", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitFloat(6.0F); // NOLINT: Magic numbers
      asmb->addLoadLitFloat(2.0F); // NOLINT: Magic numbers
      asmb->addATan2Float();
    });
  }

  SECTION("Int checks") {
    CHECK_EXPR("1 == 3", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitInt(1);
      asmb->addLoadLitInt(3);
      asmb->addCheckEqInt();
    });
    CHECK_EXPR("1 == -3", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitInt(1);
      asmb->addLoadLitInt(3);
      asmb->addNegInt();
      asmb->addCheckEqInt();
    });
    CHECK_EXPR("1 != 3", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitInt(1);
      asmb->addLoadLitInt(3);
      asmb->addCheckEqInt();
      asmb->addLogicInvInt();
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
      asmb->addLogicInvInt();
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
      asmb->addLogicInvInt();
    });
  }

  SECTION("Long checks") {
    CHECK_EXPR("1L == 3L", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitLong(1);
      asmb->addLoadLitLong(3);
      asmb->addCheckEqLong();
    });
    CHECK_EXPR("1L == -3L", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitLong(1);
      asmb->addLoadLitLong(3);
      asmb->addNegLong();
      asmb->addCheckEqLong();
    });
    CHECK_EXPR("1L != 3L", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitLong(1);
      asmb->addLoadLitLong(3);
      asmb->addCheckEqLong();
      asmb->addLogicInvInt();
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
      asmb->addLogicInvInt();
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
      asmb->addLogicInvInt();
    });
  }

  SECTION("Float checks") {
    CHECK_EXPR("1.42 == 3.42", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitFloat(1.42F); // NOLINT: Magic numbers
      asmb->addLoadLitFloat(3.42F); // NOLINT: Magic numbers
      asmb->addCheckEqFloat();
    });
    CHECK_EXPR("1.42 != 3.42", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitFloat(1.42F); // NOLINT: Magic numbers
      asmb->addLoadLitFloat(3.42F); // NOLINT: Magic numbers
      asmb->addCheckEqFloat();
      asmb->addLogicInvInt();
    });
    CHECK_EXPR("1.42 < 3.42", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitFloat(1.42F); // NOLINT: Magic numbers
      asmb->addLoadLitFloat(3.42F); // NOLINT: Magic numbers
      asmb->addCheckLeFloat();
    });
    CHECK_EXPR("1.42 <= 3.42", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitFloat(1.42F); // NOLINT: Magic numbers
      asmb->addLoadLitFloat(3.42F); // NOLINT: Magic numbers
      asmb->addCheckGtFloat();
      asmb->addLogicInvInt();
    });
    CHECK_EXPR("1.42 > 3.42", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitFloat(1.42F); // NOLINT: Magic numbers
      asmb->addLoadLitFloat(3.42F); // NOLINT: Magic numbers
      asmb->addCheckGtFloat();
    });
    CHECK_EXPR("1.42 >= 3.42", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitFloat(1.42F); // NOLINT: Magic numbers
      asmb->addLoadLitFloat(3.42F); // NOLINT: Magic numbers
      asmb->addCheckLeFloat();
      asmb->addLogicInvInt();
    });
  }

  SECTION("Char operations") {
    CHECK_EXPR("'a' + 'b'", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitInt('a');
      asmb->addLoadLitInt('b');
      asmb->addCombineChar();
    });
    CHECK_EXPR("--'a'", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitInt('a');
      asmb->addLoadLitInt(1);
      asmb->addSubInt();
      asmb->addConvIntChar();
    });
    CHECK_EXPR("++'a'", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitInt('a');
      asmb->addLoadLitInt(1);
      asmb->addAddInt();
      asmb->addConvIntChar();
    });
  }

  SECTION("Char checks") {
    CHECK_EXPR("'1' == '3'", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitInt('1');
      asmb->addLoadLitInt('3');
      asmb->addCheckEqInt();
    });
    CHECK_EXPR("'1' != '3'", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitInt('1');
      asmb->addLoadLitInt('3');
      asmb->addCheckEqInt();
      asmb->addLogicInvInt();
    });
    CHECK_EXPR("'1' < '3'", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitInt('1');
      asmb->addLoadLitInt('3');
      asmb->addCheckLeInt();
    });
    CHECK_EXPR("'1' <= '3'", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitInt('1');
      asmb->addLoadLitInt('3');
      asmb->addCheckGtInt();
      asmb->addLogicInvInt();
    });
    CHECK_EXPR("'1' > '3'", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitInt('1');
      asmb->addLoadLitInt('3');
      asmb->addCheckGtInt();
    });
    CHECK_EXPR("'1' >= '3'", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitInt('1');
      asmb->addLoadLitInt('3');
      asmb->addCheckLeInt();
      asmb->addLogicInvInt();
    });
  }

  SECTION("Bool operations") {
    CHECK_EXPR("!false", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitInt(0);
      asmb->addLogicInvInt();
    });
    CHECK_EXPR("!true", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitInt(1);
      asmb->addLogicInvInt();
    });
  }

  SECTION("Bool checks") {
    CHECK_EXPR("false == true", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitInt(0);
      asmb->addLoadLitInt(1);
      asmb->addCheckEqInt();
    });
    CHECK_EXPR("false != true", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitInt(0);
      asmb->addLoadLitInt(1);
      asmb->addCheckEqInt();
      asmb->addLogicInvInt();
    });
  }

  SECTION("Bool operations") {
    CHECK_EXPR("!false", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitInt(0);
      asmb->addLogicInvInt();
    });
    CHECK_EXPR("!true", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitInt(1);
      asmb->addLogicInvInt();
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

    CHECK_EXPR(
        "int(intrinsic{string_length}(\"hello world\"))", [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitString("hello world");
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
    CHECK_EXPR("\"hello\" == \"world\"", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitString("hello");
      asmb->addLoadLitString("world");
      asmb->addCheckEqString();
    });
    CHECK_EXPR("\"hello\" != \"world\"", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitString("hello");
      asmb->addLoadLitString("world");
      asmb->addCheckEqString();
      asmb->addLogicInvInt();
    });
  }

  SECTION("Conversions") {
    CHECK_EXPR("float(42)", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitInt(42);
      asmb->addConvIntFloat();
    });
    CHECK_EXPR("int(42.1337)", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitFloat(42.1337F); // NOLINT: Magic numbers
      asmb->addConvFloatInt();
    });
    CHECK_EXPR("int(42L)", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitLong(42); // NOLINT: Magic numbers
      asmb->addConvLongInt();
    });
    CHECK_EXPR("long(42)", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitInt(42); // NOLINT: Magic numbers
      asmb->addConvIntLong();
    });
    CHECK_EXPR("long('a')", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitInt('a');
      asmb->addConvIntLong();
    });
    CHECK_EXPR("string(42)", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitInt(42);
      asmb->addConvIntString();
    });
    CHECK_EXPR("string(42L)", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitLong(42);
      asmb->addConvLongString();
    });
    CHECK_EXPR("string(.1337)", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitFloat(0.1337F); // NOLINT: Magic numbers
      asmb->addConvFloatString();
    });
    CHECK_EXPR("string(true)", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitInt(1);
      asmb->addConvBoolString();
    });
    CHECK_EXPR("string('a')", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitInt('a');
      asmb->addConvCharString();
    });
    CHECK_EXPR("char(42)", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitInt(42); // NOLINT: Magic numbers
      asmb->addConvIntChar();
    });
    CHECK_EXPR("char(42L)", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitLong(42); // NOLINT: Magic numbers
      asmb->addConvLongChar();
    });
    CHECK_EXPR("char(42.42)", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitFloat(42.42F); // NOLINT: Magic numbers
      asmb->addConvFloatChar();
    });
    CHECK_EXPR("long(42.42)", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitFloat(42.42F); // NOLINT: Magic numbers
      asmb->addConvFloatLong();
    });
  }

  SECTION("Default constructors") {
    CHECK_EXPR("int()", [](novasm::Assembler* asmb) -> void { asmb->addLoadLitInt(0); });
    CHECK_EXPR("long()", [](novasm::Assembler* asmb) -> void { asmb->addLoadLitLong(0); });
    CHECK_EXPR("float()", [](novasm::Assembler* asmb) -> void { asmb->addLoadLitFloat(.0); });
    CHECK_EXPR("bool()", [](novasm::Assembler* asmb) -> void { asmb->addLoadLitInt(0); });
    CHECK_EXPR("string()", [](novasm::Assembler* asmb) -> void { asmb->addLoadLitString(""); });
  }

  SECTION("User functions") {
    CHECK_PROG(
        "fun test(int a, int b) -> bool a == b "
        "assert(test(42, 1337), \"test\")",
        [](novasm::Assembler* asmb) -> void {
          asmb->label("test");
          asmb->addStackLoad(0);
          asmb->addStackLoad(1);
          asmb->addCheckEqInt();
          asmb->addRet();

          asmb->label("prog");
          asmb->addLoadLitInt(42);
          asmb->addLoadLitInt(1337);
          asmb->addCall("test", 2, novasm::CallMode::Normal);

          asmb->addLoadLitString("test");
          asmb->addPCall(novasm::PCallCode::Assert);
          asmb->addRet();

          asmb->setEntrypoint("prog");
        });
  }

  SECTION("Identity conversions") {
    CHECK_EXPR("int(42)", [](novasm::Assembler* asmb) -> void { asmb->addLoadLitInt(42); });
    CHECK_EXPR("long(42L)", [](novasm::Assembler* asmb) -> void { asmb->addLoadLitLong(42); });
    CHECK_EXPR("float(42.1337)", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitFloat(42.1337F); // NOLINT: Magic numbers
    });
    CHECK_EXPR("bool(false)", [](novasm::Assembler* asmb) -> void { asmb->addLoadLitInt(0); });
    CHECK_EXPR("string(\"hello world\")", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitString("hello world");
    });
    CHECK_EXPR("char('a')", [](novasm::Assembler* asmb) -> void { asmb->addLoadLitInt('a'); });
  }
}

} // namespace backend
