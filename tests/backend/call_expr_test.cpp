#include "catch2/catch.hpp"
#include "helpers.hpp"

namespace backend {

TEST_CASE("Generate assembly for call expressions", "[backend]") {

  SECTION("Int operations") {
    CHECK_EXPR_INT("-42", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitInt(42);
      asmb->addNegInt();
    });
    CHECK_EXPR_INT("+42", [](novasm::Assembler* asmb) -> void { asmb->addLoadLitInt(42); });
    CHECK_EXPR_INT("- -42", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitInt(42);
      asmb->addNegInt();
      asmb->addNegInt();
    });
    CHECK_EXPR_INT("--42", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitInt(42);
      asmb->addLoadLitInt(1);
      asmb->addSubInt();
    });
    CHECK_EXPR_INT("++42", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitInt(42);
      asmb->addLoadLitInt(1);
      asmb->addAddInt();
    });
    CHECK_EXPR_INT("1 + 3", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitInt(1);
      asmb->addLoadLitInt(3);
      asmb->addAddInt();
    });
    CHECK_EXPR_INT("-1 + 3", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitInt(1);
      asmb->addNegInt();
      asmb->addLoadLitInt(3);
      asmb->addAddInt();
    });
    CHECK_EXPR_INT("1 - 3", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitInt(1);
      asmb->addLoadLitInt(3);
      asmb->addSubInt();
    });
    CHECK_EXPR_INT("1 * 3", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitInt(1);
      asmb->addLoadLitInt(3);
      asmb->addMulInt();
    });
    CHECK_EXPR_INT("1 / 3", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitInt(1);
      asmb->addLoadLitInt(3);
      asmb->addDivInt();
    });
    CHECK_EXPR_INT("1 % 3", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitInt(1);
      asmb->addLoadLitInt(3);
      asmb->addRemInt();
    });
    CHECK_EXPR_INT("1 << 3", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitInt(1);
      asmb->addLoadLitInt(3);
      asmb->addShiftLeftInt();
    });
    CHECK_EXPR_INT("1 >> 3", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitInt(1);
      asmb->addLoadLitInt(3);
      asmb->addShiftRightInt();
    });
    CHECK_EXPR_INT("1 & 3", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitInt(1);
      asmb->addLoadLitInt(3);
      asmb->addAndInt();
    });
    CHECK_EXPR_INT("1 | 3", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitInt(1);
      asmb->addLoadLitInt(3);
      asmb->addOrInt();
    });
    CHECK_EXPR_INT("1 ^ 3", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitInt(1);
      asmb->addLoadLitInt(3);
      asmb->addXorInt();
    });
  }

  SECTION("Long operations") {
    CHECK_EXPR_LONG("-42L", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitLong(42);
      asmb->addNegLong();
    });
    CHECK_EXPR_LONG("+42L", [](novasm::Assembler* asmb) -> void { asmb->addLoadLitLong(42); });
    CHECK_EXPR_LONG("- -42L", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitLong(42);
      asmb->addNegLong();
      asmb->addNegLong();
    });
    CHECK_EXPR_LONG("--42L", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitLong(42);
      asmb->addLoadLitLong(1);
      asmb->addSubLong();
    });
    CHECK_EXPR_LONG("++42L", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitLong(42);
      asmb->addLoadLitLong(1);
      asmb->addAddLong();
    });
    CHECK_EXPR_LONG("1L + 3L", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitLong(1);
      asmb->addLoadLitLong(3);
      asmb->addAddLong();
    });
    CHECK_EXPR_LONG("-1L + 3L", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitLong(1);
      asmb->addNegLong();
      asmb->addLoadLitLong(3);
      asmb->addAddLong();
    });
    CHECK_EXPR_LONG("1L - 3L", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitLong(1);
      asmb->addLoadLitLong(3);
      asmb->addSubLong();
    });
    CHECK_EXPR_LONG("1L * 3L", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitLong(1);
      asmb->addLoadLitLong(3);
      asmb->addMulLong();
    });
    CHECK_EXPR_LONG("1L / 3L", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitLong(1);
      asmb->addLoadLitLong(3);
      asmb->addDivLong();
    });
    CHECK_EXPR_LONG("1L % 3L", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitLong(1);
      asmb->addLoadLitLong(3);
      asmb->addRemLong();
    });
  }

  SECTION("Float operations") {
    CHECK_EXPR_FLOAT("-.1337", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitFloat(0.1337F); // NOLINT: Magic numbers
      asmb->addNegFloat();
    });
    CHECK_EXPR_FLOAT("+.1337", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitFloat(0.1337F); // NOLINT: Magic numbers
    });
    CHECK_EXPR_FLOAT("--.1337", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitFloat(0.1337F); // NOLINT: Magic numbers
      asmb->addLoadLitFloat(1.0F);
      asmb->addSubFloat();
    });
    CHECK_EXPR_FLOAT("++.1337", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitFloat(0.1337F); // NOLINT: Magic numbers
      asmb->addLoadLitFloat(1.0F);
      asmb->addAddFloat();
    });
    CHECK_EXPR_FLOAT("1.42 + 3.42", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitFloat(1.42F); // NOLINT: Magic numbers
      asmb->addLoadLitFloat(3.42F); // NOLINT: Magic numbers
      asmb->addAddFloat();
    });
    CHECK_EXPR_FLOAT("1.42 - 3.42", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitFloat(1.42F); // NOLINT: Magic numbers
      asmb->addLoadLitFloat(3.42F); // NOLINT: Magic numbers
      asmb->addSubFloat();
    });
    CHECK_EXPR_FLOAT("1.42 * 3.42", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitFloat(1.42F); // NOLINT: Magic numbers
      asmb->addLoadLitFloat(3.42F); // NOLINT: Magic numbers
      asmb->addMulFloat();
    });
    CHECK_EXPR_FLOAT("1.42 / 3.42", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitFloat(1.42F); // NOLINT: Magic numbers
      asmb->addLoadLitFloat(3.42F); // NOLINT: Magic numbers
      asmb->addDivFloat();
    });
    CHECK_EXPR_FLOAT("1.0 / 2", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitFloat(1.0F);
      asmb->addLoadLitInt(2);
      asmb->addConvIntFloat();
      asmb->addDivFloat();
    });
    CHECK_EXPR_FLOAT("1 / 2.0", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitInt(1);
      asmb->addConvIntFloat();
      asmb->addLoadLitFloat(2.0F); // NOLINT: Magic numbers
      asmb->addDivFloat();
    });
  }

  SECTION("Int checks") {
    CHECK_EXPR_BOOL("1 == 3", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitInt(1);
      asmb->addLoadLitInt(3);
      asmb->addCheckEqInt();
    });
    CHECK_EXPR_BOOL("1 == -3", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitInt(1);
      asmb->addLoadLitInt(3);
      asmb->addNegInt();
      asmb->addCheckEqInt();
    });
    CHECK_EXPR_BOOL("1 != 3", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitInt(1);
      asmb->addLoadLitInt(3);
      asmb->addCheckEqInt();
      asmb->addLogicInvInt();
    });
    CHECK_EXPR_BOOL("1 < 3", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitInt(1);
      asmb->addLoadLitInt(3);
      asmb->addCheckLeInt();
    });
    CHECK_EXPR_BOOL("1 <= 3", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitInt(1);
      asmb->addLoadLitInt(3);
      asmb->addCheckGtInt();
      asmb->addLogicInvInt();
    });
    CHECK_EXPR_BOOL("1 > 3", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitInt(1);
      asmb->addLoadLitInt(3);
      asmb->addCheckGtInt();
    });
    CHECK_EXPR_BOOL("1 >= 3", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitInt(1);
      asmb->addLoadLitInt(3);
      asmb->addCheckLeInt();
      asmb->addLogicInvInt();
    });
  }

  SECTION("Long checks") {
    CHECK_EXPR_BOOL("1L == 3L", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitLong(1);
      asmb->addLoadLitLong(3);
      asmb->addCheckEqLong();
    });
    CHECK_EXPR_BOOL("1L == -3L", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitLong(1);
      asmb->addLoadLitLong(3);
      asmb->addNegLong();
      asmb->addCheckEqLong();
    });
    CHECK_EXPR_BOOL("1L != 3L", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitLong(1);
      asmb->addLoadLitLong(3);
      asmb->addCheckEqLong();
      asmb->addLogicInvInt();
    });
    CHECK_EXPR_BOOL("1L < 3L", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitLong(1);
      asmb->addLoadLitLong(3);
      asmb->addCheckLeLong();
    });
    CHECK_EXPR_BOOL("1L <= 3L", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitLong(1);
      asmb->addLoadLitLong(3);
      asmb->addCheckGtLong();
      asmb->addLogicInvInt();
    });
    CHECK_EXPR_BOOL("1L > 3L", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitLong(1);
      asmb->addLoadLitLong(3);
      asmb->addCheckGtLong();
    });
    CHECK_EXPR_BOOL("1L >= 3L", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitLong(1);
      asmb->addLoadLitLong(3);
      asmb->addCheckLeLong();
      asmb->addLogicInvInt();
    });
  }

  SECTION("Float checks") {
    CHECK_EXPR_BOOL("1.42 == 3.42", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitFloat(1.42F); // NOLINT: Magic numbers
      asmb->addLoadLitFloat(3.42F); // NOLINT: Magic numbers
      asmb->addCheckEqFloat();
    });
    CHECK_EXPR_BOOL("1.42 != 3.42", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitFloat(1.42F); // NOLINT: Magic numbers
      asmb->addLoadLitFloat(3.42F); // NOLINT: Magic numbers
      asmb->addCheckEqFloat();
      asmb->addLogicInvInt();
    });
    CHECK_EXPR_BOOL("1.42 < 3.42", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitFloat(1.42F); // NOLINT: Magic numbers
      asmb->addLoadLitFloat(3.42F); // NOLINT: Magic numbers
      asmb->addCheckLeFloat();
    });
    CHECK_EXPR_BOOL("1.42 <= 3.42", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitFloat(1.42F); // NOLINT: Magic numbers
      asmb->addLoadLitFloat(3.42F); // NOLINT: Magic numbers
      asmb->addCheckGtFloat();
      asmb->addLogicInvInt();
    });
    CHECK_EXPR_BOOL("1.42 > 3.42", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitFloat(1.42F); // NOLINT: Magic numbers
      asmb->addLoadLitFloat(3.42F); // NOLINT: Magic numbers
      asmb->addCheckGtFloat();
    });
    CHECK_EXPR_BOOL("1.42 >= 3.42", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitFloat(1.42F); // NOLINT: Magic numbers
      asmb->addLoadLitFloat(3.42F); // NOLINT: Magic numbers
      asmb->addCheckLeFloat();
      asmb->addLogicInvInt();
    });
  }

  SECTION("Char operations") {
    CHECK_EXPR_STRING("'a' + 'b'", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitInt('a');
      asmb->addLoadLitInt('b');
      asmb->addAddChar();
    });
    CHECK_EXPR_CHAR("--'a'", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitInt('a');
      asmb->addLoadLitInt(1);
      asmb->addSubInt();
      asmb->addConvIntChar();
    });
    CHECK_EXPR_CHAR("++'a'", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitInt('a');
      asmb->addLoadLitInt(1);
      asmb->addAddInt();
      asmb->addConvIntChar();
    });
  }

  SECTION("Char checks") {
    CHECK_EXPR_BOOL("'1' == '3'", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitInt('1');
      asmb->addLoadLitInt('3');
      asmb->addCheckEqInt();
    });
    CHECK_EXPR_BOOL("'1' != '3'", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitInt('1');
      asmb->addLoadLitInt('3');
      asmb->addCheckEqInt();
      asmb->addLogicInvInt();
    });
    CHECK_EXPR_BOOL("'1' < '3'", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitInt('1');
      asmb->addLoadLitInt('3');
      asmb->addCheckLeInt();
    });
    CHECK_EXPR_BOOL("'1' <= '3'", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitInt('1');
      asmb->addLoadLitInt('3');
      asmb->addCheckGtInt();
      asmb->addLogicInvInt();
    });
    CHECK_EXPR_BOOL("'1' > '3'", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitInt('1');
      asmb->addLoadLitInt('3');
      asmb->addCheckGtInt();
    });
    CHECK_EXPR_BOOL("'1' >= '3'", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitInt('1');
      asmb->addLoadLitInt('3');
      asmb->addCheckLeInt();
      asmb->addLogicInvInt();
    });
  }

  SECTION("Bool operations") {
    CHECK_EXPR_BOOL("!false", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitInt(0);
      asmb->addLogicInvInt();
    });
    CHECK_EXPR_BOOL("!true", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitInt(1);
      asmb->addLogicInvInt();
    });
  }

  SECTION("Bool checks") {
    CHECK_EXPR_BOOL("false == true", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitInt(0);
      asmb->addLoadLitInt(1);
      asmb->addCheckEqInt();
    });
    CHECK_EXPR_BOOL("false != true", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitInt(0);
      asmb->addLoadLitInt(1);
      asmb->addCheckEqInt();
      asmb->addLogicInvInt();
    });
  }

  SECTION("Bool operations") {
    CHECK_EXPR_BOOL("!false", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitInt(0);
      asmb->addLogicInvInt();
    });
    CHECK_EXPR_BOOL("!true", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitInt(1);
      asmb->addLogicInvInt();
    });
  }

  SECTION("String operations") {
    CHECK_EXPR_STRING("\"hello\" + \"world\"", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitString("hello");
      asmb->addLoadLitString("world");
      asmb->addAddString();
    });

    CHECK_EXPR_INT("length(\"hello world\")", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitString("hello world");
      asmb->addLengthString();
    });

    CHECK_EXPR_CHAR("\"hello world\"[6]", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitString("hello world");
      asmb->addLoadLitInt(6);
      asmb->addIndexString();
    });
    CHECK_EXPR_STRING("\"hello world\"[0, 6]", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitString("hello world");
      asmb->addLoadLitInt(0);
      asmb->addLoadLitInt(6);
      asmb->addSliceString();
    });
  }

  SECTION("String checks") {
    CHECK_EXPR_BOOL("\"hello\" == \"world\"", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitString("hello");
      asmb->addLoadLitString("world");
      asmb->addCheckEqString();
    });
    CHECK_EXPR_BOOL("\"hello\" != \"world\"", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitString("hello");
      asmb->addLoadLitString("world");
      asmb->addCheckEqString();
      asmb->addLogicInvInt();
    });
  }

  SECTION("Conversions") {
    CHECK_EXPR_FLOAT("float(42)", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitInt(42);
      asmb->addConvIntFloat();
    });
    CHECK_EXPR_INT("int(42.1337)", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitFloat(42.1337F); // NOLINT: Magic numbers
      asmb->addConvFloatInt();
    });
    CHECK_EXPR_INT("int(42L)", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitLong(42); // NOLINT: Magic numbers
      asmb->addConvLongInt();
    });
    CHECK_EXPR_LONG("long(42)", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitInt(42); // NOLINT: Magic numbers
      asmb->addConvIntLong();
    });
    CHECK_EXPR_STRING("string(42)", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitInt(42);
      asmb->addConvIntString();
    });
    CHECK_EXPR_STRING("string(42L)", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitLong(42);
      asmb->addConvLongString();
    });
    CHECK_EXPR_STRING("string(.1337)", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitFloat(0.1337F); // NOLINT: Magic numbers
      asmb->addConvFloatString();
    });
    CHECK_EXPR_STRING("string(true)", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitInt(1);
      asmb->addConvBoolString();
    });
    CHECK_EXPR_STRING("string('a')", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitInt('a');
      asmb->addConvCharString();
    });
  }

  SECTION("Default constructors") {
    CHECK_EXPR_INT("int()", [](novasm::Assembler* asmb) -> void { asmb->addLoadLitInt(0); });
    CHECK_EXPR_LONG("long()", [](novasm::Assembler* asmb) -> void { asmb->addLoadLitLong(0); });
    CHECK_EXPR_FLOAT("float()", [](novasm::Assembler* asmb) -> void { asmb->addLoadLitFloat(.0); });
    CHECK_EXPR_BOOL("bool()", [](novasm::Assembler* asmb) -> void { asmb->addLoadLitInt(0); });
    CHECK_EXPR_STRING(
        "string()", [](novasm::Assembler* asmb) -> void { asmb->addLoadLitString(""); });
  }

  SECTION("User functions") {
    CHECK_PROG(
        "fun test(int a, int b) -> int a + b "
        "conWrite(string(test(42, 1337)))",
        [](novasm::Assembler* asmb) -> void {
          asmb->label("test");
          asmb->addStackLoad(0);
          asmb->addStackLoad(1);
          asmb->addAddInt();
          asmb->addRet();

          asmb->label("prog");
          asmb->addLoadLitInt(42);
          asmb->addLoadLitInt(1337);
          asmb->addCall("test", 2, novasm::CallMode::Normal);
          asmb->addConvIntString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
          asmb->addRet();

          asmb->setEntrypoint("prog");
        });
  }

  SECTION("Identity conversions") {
    CHECK_EXPR_INT("int(42)", [](novasm::Assembler* asmb) -> void { asmb->addLoadLitInt(42); });
    CHECK_EXPR_LONG("long(42L)", [](novasm::Assembler* asmb) -> void { asmb->addLoadLitLong(42); });
    CHECK_EXPR_FLOAT("float(42.1337)", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitFloat(42.1337F); // NOLINT: Magic numbers
    });
    CHECK_EXPR_BOOL("bool(false)", [](novasm::Assembler* asmb) -> void { asmb->addLoadLitInt(0); });
    CHECK_EXPR_STRING("string(\"hello world\")", [](novasm::Assembler* asmb) -> void {
      asmb->addLoadLitString("hello world");
    });
    CHECK_EXPR_CHAR("char('a')", [](novasm::Assembler* asmb) -> void { asmb->addLoadLitInt('a'); });
  }
}

} // namespace backend
