#include "catch2/catch.hpp"
#include "helpers.hpp"

namespace backend {

TEST_CASE("Call Expression", "[backend]") {

  SECTION("Int operations") {
    CHECK_EXPR_INT("-42", [](backend::Builder* builder) -> void {
      builder->addLoadLitInt(42);
      builder->addNegInt();
    });
    CHECK_EXPR_INT("--42", [](backend::Builder* builder) -> void {
      builder->addLoadLitInt(42);
      builder->addNegInt();
      builder->addNegInt();
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

  SECTION("User functions") {
    CHECK_PROG(
        "fun test(int a, int b) -> int a + b "
        "print(test(42, 1337))",
        [](backend::Builder* builder) -> void {
          builder->label("test");
          builder->addReserveConsts(2);
          builder->addStoreConst(1);
          builder->addStoreConst(0);
          builder->addLoadConst(0);
          builder->addLoadConst(1);
          builder->addAddInt();
          builder->addRet();
          builder->addFail();

          builder->label("print");
          builder->addLoadLitInt(42);
          builder->addLoadLitInt(1337);
          builder->addCall("test");
          builder->addPrintInt();
          builder->addRet();
          builder->addFail();

          builder->addEntryPoint("print");
        });
  }
}

} // namespace backend