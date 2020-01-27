#include "catch2/catch.hpp"
#include "helpers.hpp"

namespace backend {

TEST_CASE("Generate assembly for call expressions", "[backend]") {

  SECTION("Int operations") {
    CHECK_EXPR_INT("-42", [](backend::Builder* builder) -> void {
      builder->addLoadLitInt(42);
      builder->addNegInt();
    });
    CHECK_EXPR_INT("+42", [](backend::Builder* builder) -> void { builder->addLoadLitInt(42); });
    CHECK_EXPR_INT("- -42", [](backend::Builder* builder) -> void {
      builder->addLoadLitInt(42);
      builder->addNegInt();
      builder->addNegInt();
    });
    CHECK_EXPR_INT("--42", [](backend::Builder* builder) -> void {
      builder->addLoadLitInt(42);
      builder->addLoadLitInt(1);
      builder->addSubInt();
    });
    CHECK_EXPR_INT("++42", [](backend::Builder* builder) -> void {
      builder->addLoadLitInt(42);
      builder->addLoadLitInt(1);
      builder->addAddInt();
    });
    CHECK_EXPR_INT("1 + 3", [](backend::Builder* builder) -> void {
      builder->addLoadLitInt(1);
      builder->addLoadLitInt(3);
      builder->addAddInt();
    });
    CHECK_EXPR_INT("-1 + 3", [](backend::Builder* builder) -> void {
      builder->addLoadLitInt(1);
      builder->addNegInt();
      builder->addLoadLitInt(3);
      builder->addAddInt();
    });
    CHECK_EXPR_INT("1 - 3", [](backend::Builder* builder) -> void {
      builder->addLoadLitInt(1);
      builder->addLoadLitInt(3);
      builder->addSubInt();
    });
    CHECK_EXPR_INT("1 * 3", [](backend::Builder* builder) -> void {
      builder->addLoadLitInt(1);
      builder->addLoadLitInt(3);
      builder->addMulInt();
    });
    CHECK_EXPR_INT("1 / 3", [](backend::Builder* builder) -> void {
      builder->addLoadLitInt(1);
      builder->addLoadLitInt(3);
      builder->addDivInt();
    });
    CHECK_EXPR_INT("1 % 3", [](backend::Builder* builder) -> void {
      builder->addLoadLitInt(1);
      builder->addLoadLitInt(3);
      builder->addRemInt();
    });
    CHECK_EXPR_INT("1 << 3", [](backend::Builder* builder) -> void {
      builder->addLoadLitInt(1);
      builder->addLoadLitInt(3);
      builder->addShiftLeftInt();
    });
    CHECK_EXPR_INT("1 >> 3", [](backend::Builder* builder) -> void {
      builder->addLoadLitInt(1);
      builder->addLoadLitInt(3);
      builder->addShiftRightInt();
    });
    CHECK_EXPR_INT("1 & 3", [](backend::Builder* builder) -> void {
      builder->addLoadLitInt(1);
      builder->addLoadLitInt(3);
      builder->addAndInt();
    });
    CHECK_EXPR_INT("1 | 3", [](backend::Builder* builder) -> void {
      builder->addLoadLitInt(1);
      builder->addLoadLitInt(3);
      builder->addOrInt();
    });
    CHECK_EXPR_INT("1 ^ 3", [](backend::Builder* builder) -> void {
      builder->addLoadLitInt(1);
      builder->addLoadLitInt(3);
      builder->addXorInt();
    });
  }

  SECTION("Float operations") {
    CHECK_EXPR_FLOAT("-.1337", [](backend::Builder* builder) -> void {
      builder->addLoadLitFloat(0.1337F); // NOLINT: Magic numbers
      builder->addNegFloat();
    });
    CHECK_EXPR_FLOAT("+.1337", [](backend::Builder* builder) -> void {
      builder->addLoadLitFloat(0.1337F); // NOLINT: Magic numbers
    });
    CHECK_EXPR_FLOAT("--.1337", [](backend::Builder* builder) -> void {
      builder->addLoadLitFloat(0.1337F); // NOLINT: Magic numbers
      builder->addLoadLitFloat(1.0F);
      builder->addSubFloat();
    });
    CHECK_EXPR_FLOAT("++.1337", [](backend::Builder* builder) -> void {
      builder->addLoadLitFloat(0.1337F); // NOLINT: Magic numbers
      builder->addLoadLitFloat(1.0F);
      builder->addAddFloat();
    });
    CHECK_EXPR_FLOAT("1.42 + 3.42", [](backend::Builder* builder) -> void {
      builder->addLoadLitFloat(1.42F); // NOLINT: Magic numbers
      builder->addLoadLitFloat(3.42F); // NOLINT: Magic numbers
      builder->addAddFloat();
    });
    CHECK_EXPR_FLOAT("1.42 - 3.42", [](backend::Builder* builder) -> void {
      builder->addLoadLitFloat(1.42F); // NOLINT: Magic numbers
      builder->addLoadLitFloat(3.42F); // NOLINT: Magic numbers
      builder->addSubFloat();
    });
    CHECK_EXPR_FLOAT("1.42 * 3.42", [](backend::Builder* builder) -> void {
      builder->addLoadLitFloat(1.42F); // NOLINT: Magic numbers
      builder->addLoadLitFloat(3.42F); // NOLINT: Magic numbers
      builder->addMulFloat();
    });
    CHECK_EXPR_FLOAT("1.42 / 3.42", [](backend::Builder* builder) -> void {
      builder->addLoadLitFloat(1.42F); // NOLINT: Magic numbers
      builder->addLoadLitFloat(3.42F); // NOLINT: Magic numbers
      builder->addDivFloat();
    });
    CHECK_EXPR_FLOAT("1.0 / 2", [](backend::Builder* builder) -> void {
      builder->addLoadLitFloat(1.0F);
      builder->addLoadLitInt(2);
      builder->addConvIntFloat();
      builder->addDivFloat();
    });
    CHECK_EXPR_FLOAT("1 / 2.0", [](backend::Builder* builder) -> void {
      builder->addLoadLitInt(1);
      builder->addConvIntFloat();
      builder->addLoadLitFloat(2.0F); // NOLINT: Magic numbers
      builder->addDivFloat();
    });
  }

  SECTION("Int checks") {
    CHECK_EXPR_BOOL("1 == 3", [](backend::Builder* builder) -> void {
      builder->addLoadLitInt(1);
      builder->addLoadLitInt(3);
      builder->addCheckEqInt();
    });
    CHECK_EXPR_BOOL("1 == -3", [](backend::Builder* builder) -> void {
      builder->addLoadLitInt(1);
      builder->addLoadLitInt(3);
      builder->addNegInt();
      builder->addCheckEqInt();
    });
    CHECK_EXPR_BOOL("1 != 3", [](backend::Builder* builder) -> void {
      builder->addLoadLitInt(1);
      builder->addLoadLitInt(3);
      builder->addCheckEqInt();
      builder->addLogicInvInt();
    });
    CHECK_EXPR_BOOL("1 < 3", [](backend::Builder* builder) -> void {
      builder->addLoadLitInt(1);
      builder->addLoadLitInt(3);
      builder->addCheckLeInt();
    });
    CHECK_EXPR_BOOL("1 <= 3", [](backend::Builder* builder) -> void {
      builder->addLoadLitInt(1);
      builder->addLoadLitInt(3);
      builder->addCheckGtInt();
      builder->addLogicInvInt();
    });
    CHECK_EXPR_BOOL("1 > 3", [](backend::Builder* builder) -> void {
      builder->addLoadLitInt(1);
      builder->addLoadLitInt(3);
      builder->addCheckGtInt();
    });
    CHECK_EXPR_BOOL("1 >= 3", [](backend::Builder* builder) -> void {
      builder->addLoadLitInt(1);
      builder->addLoadLitInt(3);
      builder->addCheckLeInt();
      builder->addLogicInvInt();
    });
  }

  SECTION("Float checks") {
    CHECK_EXPR_BOOL("1.42 == 3.42", [](backend::Builder* builder) -> void {
      builder->addLoadLitFloat(1.42F); // NOLINT: Magic numbers
      builder->addLoadLitFloat(3.42F); // NOLINT: Magic numbers
      builder->addCheckEqFloat();
    });
    CHECK_EXPR_BOOL("1.42 != 3.42", [](backend::Builder* builder) -> void {
      builder->addLoadLitFloat(1.42F); // NOLINT: Magic numbers
      builder->addLoadLitFloat(3.42F); // NOLINT: Magic numbers
      builder->addCheckEqFloat();
      builder->addLogicInvInt();
    });
    CHECK_EXPR_BOOL("1.42 < 3.42", [](backend::Builder* builder) -> void {
      builder->addLoadLitFloat(1.42F); // NOLINT: Magic numbers
      builder->addLoadLitFloat(3.42F); // NOLINT: Magic numbers
      builder->addCheckLeFloat();
    });
    CHECK_EXPR_BOOL("1.42 <= 3.42", [](backend::Builder* builder) -> void {
      builder->addLoadLitFloat(1.42F); // NOLINT: Magic numbers
      builder->addLoadLitFloat(3.42F); // NOLINT: Magic numbers
      builder->addCheckGtFloat();
      builder->addLogicInvInt();
    });
    CHECK_EXPR_BOOL("1.42 > 3.42", [](backend::Builder* builder) -> void {
      builder->addLoadLitFloat(1.42F); // NOLINT: Magic numbers
      builder->addLoadLitFloat(3.42F); // NOLINT: Magic numbers
      builder->addCheckGtFloat();
    });
    CHECK_EXPR_BOOL("1.42 >= 3.42", [](backend::Builder* builder) -> void {
      builder->addLoadLitFloat(1.42F); // NOLINT: Magic numbers
      builder->addLoadLitFloat(3.42F); // NOLINT: Magic numbers
      builder->addCheckLeFloat();
      builder->addLogicInvInt();
    });
  }

  SECTION("Char operations") {
    CHECK_EXPR_CHAR("--'a'", [](backend::Builder* builder) -> void {
      builder->addLoadLitInt('a');
      builder->addLoadLitInt(1);
      builder->addSubInt();
      builder->addConvIntChar();
    });
    CHECK_EXPR_CHAR("++'a'", [](backend::Builder* builder) -> void {
      builder->addLoadLitInt('a');
      builder->addLoadLitInt(1);
      builder->addAddInt();
      builder->addConvIntChar();
    });
  }

  SECTION("Char checks") {
    CHECK_EXPR_BOOL("'1' == '3'", [](backend::Builder* builder) -> void {
      builder->addLoadLitInt('1');
      builder->addLoadLitInt('3');
      builder->addCheckEqInt();
    });
    CHECK_EXPR_BOOL("'1' != '3'", [](backend::Builder* builder) -> void {
      builder->addLoadLitInt('1');
      builder->addLoadLitInt('3');
      builder->addCheckEqInt();
      builder->addLogicInvInt();
    });
    CHECK_EXPR_BOOL("'1' < '3'", [](backend::Builder* builder) -> void {
      builder->addLoadLitInt('1');
      builder->addLoadLitInt('3');
      builder->addCheckLeInt();
    });
    CHECK_EXPR_BOOL("'1' <= '3'", [](backend::Builder* builder) -> void {
      builder->addLoadLitInt('1');
      builder->addLoadLitInt('3');
      builder->addCheckGtInt();
      builder->addLogicInvInt();
    });
    CHECK_EXPR_BOOL("'1' > '3'", [](backend::Builder* builder) -> void {
      builder->addLoadLitInt('1');
      builder->addLoadLitInt('3');
      builder->addCheckGtInt();
    });
    CHECK_EXPR_BOOL("'1' >= '3'", [](backend::Builder* builder) -> void {
      builder->addLoadLitInt('1');
      builder->addLoadLitInt('3');
      builder->addCheckLeInt();
      builder->addLogicInvInt();
    });
  }

  SECTION("Bool operations") {
    CHECK_EXPR_BOOL("!false", [](backend::Builder* builder) -> void {
      builder->addLoadLitInt(0);
      builder->addLogicInvInt();
    });
    CHECK_EXPR_BOOL("!true", [](backend::Builder* builder) -> void {
      builder->addLoadLitInt(1);
      builder->addLogicInvInt();
    });
  }

  SECTION("Bool checks") {
    CHECK_EXPR_BOOL("false == true", [](backend::Builder* builder) -> void {
      builder->addLoadLitInt(0);
      builder->addLoadLitInt(1);
      builder->addCheckEqInt();
    });
    CHECK_EXPR_BOOL("false != true", [](backend::Builder* builder) -> void {
      builder->addLoadLitInt(0);
      builder->addLoadLitInt(1);
      builder->addCheckEqInt();
      builder->addLogicInvInt();
    });
  }

  SECTION("Bool operations") {
    CHECK_EXPR_BOOL("!false", [](backend::Builder* builder) -> void {
      builder->addLoadLitInt(0);
      builder->addLogicInvInt();
    });
    CHECK_EXPR_BOOL("!true", [](backend::Builder* builder) -> void {
      builder->addLoadLitInt(1);
      builder->addLogicInvInt();
    });
  }

  SECTION("String operations") {
    CHECK_EXPR_STRING("\"hello\" + \"world\"", [](backend::Builder* builder) -> void {
      builder->addLoadLitString("hello");
      builder->addLoadLitString("world");
      builder->addAddString();
    });

    CHECK_EXPR_INT("length(\"hello world\")", [](backend::Builder* builder) -> void {
      builder->addLoadLitString("hello world");
      builder->addLengthString();
    });

    CHECK_EXPR_CHAR("\"hello world\"[6]", [](backend::Builder* builder) -> void {
      builder->addLoadLitString("hello world");
      builder->addLoadLitInt(6);
      builder->addIndexString();
    });
    CHECK_EXPR_STRING("\"hello world\"[0, 6]", [](backend::Builder* builder) -> void {
      builder->addLoadLitString("hello world");
      builder->addLoadLitInt(0);
      builder->addLoadLitInt(6);
      builder->addSliceString();
    });
  }

  SECTION("String checks") {
    CHECK_EXPR_BOOL("\"hello\" == \"world\"", [](backend::Builder* builder) -> void {
      builder->addLoadLitString("hello");
      builder->addLoadLitString("world");
      builder->addCheckEqString();
    });
    CHECK_EXPR_BOOL("\"hello\" != \"world\"", [](backend::Builder* builder) -> void {
      builder->addLoadLitString("hello");
      builder->addLoadLitString("world");
      builder->addCheckEqString();
      builder->addLogicInvInt();
    });
  }

  SECTION("Conversions") {
    CHECK_EXPR_FLOAT("float(42)", [](backend::Builder* builder) -> void {
      builder->addLoadLitInt(42);
      builder->addConvIntFloat();
    });
    CHECK_EXPR_INT("int(42.1337)", [](backend::Builder* builder) -> void {
      builder->addLoadLitFloat(42.1337F); // NOLINT: Magic numbers
      builder->addConvFloatInt();
    });
    CHECK_EXPR_STRING("string(42)", [](backend::Builder* builder) -> void {
      builder->addLoadLitInt(42);
      builder->addConvIntString();
    });
    CHECK_EXPR_STRING("string(.1337)", [](backend::Builder* builder) -> void {
      builder->addLoadLitFloat(0.1337F); // NOLINT: Magic numbers
      builder->addConvFloatString();
    });
    CHECK_EXPR_STRING("string(true)", [](backend::Builder* builder) -> void {
      builder->addLoadLitInt(1);
      builder->addConvBoolString();
    });
    CHECK_EXPR_STRING("string('a')", [](backend::Builder* builder) -> void {
      builder->addLoadLitInt('a');
      builder->addConvCharString();
    });
  }

  SECTION("Default constructors") {
    CHECK_EXPR_INT("int()", [](backend::Builder* builder) -> void { builder->addLoadLitInt(0); });
    CHECK_EXPR_FLOAT(
        "float()", [](backend::Builder* builder) -> void { builder->addLoadLitFloat(.0); });
    CHECK_EXPR_BOOL("bool()", [](backend::Builder* builder) -> void { builder->addLoadLitInt(0); });
    CHECK_EXPR_STRING(
        "string()", [](backend::Builder* builder) -> void { builder->addLoadLitString(""); });
  }

  SECTION("User functions") {
    CHECK_PROG(
        "fun test(int a, int b) -> int a + b "
        "print(string(test(42, 1337)))",
        [](backend::Builder* builder) -> void {
          builder->label("test");
          builder->addReserveConsts(2);
          builder->addStoreConst(1);
          builder->addStoreConst(0);
          builder->addLoadConst(0);
          builder->addLoadConst(1);
          builder->addAddInt();
          builder->addRet();

          builder->label("print");
          builder->addLoadLitInt(42);
          builder->addLoadLitInt(1337);
          builder->addCall("test", false);
          builder->addConvIntString();
          builder->addPCall(vm::PCallCode::PrintString);
          builder->addPop();
          builder->addRet();

          builder->addEntryPoint("print");
        });
  }

  SECTION("Identity conversions") {
    CHECK_EXPR_INT(
        "int(42)", [](backend::Builder* builder) -> void { builder->addLoadLitInt(42); });
    CHECK_EXPR_FLOAT("float(42.1337)", [](backend::Builder* builder) -> void {
      builder->addLoadLitFloat(42.1337F); // NOLINT: Magic numbers
    });
    CHECK_EXPR_BOOL(
        "bool(false)", [](backend::Builder* builder) -> void { builder->addLoadLitInt(0); });
    CHECK_EXPR_STRING("string(\"hello world\")", [](backend::Builder* builder) -> void {
      builder->addLoadLitString("hello world");
    });
    CHECK_EXPR_CHAR(
        "char('a')", [](backend::Builder* builder) -> void { builder->addLoadLitInt('a'); });
  }
}

} // namespace backend
