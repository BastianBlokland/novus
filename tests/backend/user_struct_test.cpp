#include "catch2/catch.hpp"
#include "helpers.hpp"

namespace backend {

TEST_CASE("Generating assembly for user-structs", "[backend]") {

  SECTION("Create user struct and check for equality") {
    CHECK_PROG(
        "struct User = string name, int age "
        "print(User(\"hello\", 42) == User(\"world\", 1337))",
        [](backend::Builder* builder) -> void {
          // Struct equality function.
          builder->label("UserEq");
          builder->addReserveConsts(2);
          builder->addStoreConst(0);
          builder->addStoreConst(1);

          builder->addLoadConst(0);
          builder->addLoadStructField(0);
          builder->addLoadConst(1);
          builder->addLoadStructField(0);
          builder->addCheckEqString();

          builder->addJumpIf("field1 equal");
          builder->addLoadLitInt(0);
          builder->addRet();

          builder->label("field1 equal");
          builder->addLoadConst(0);
          builder->addLoadStructField(1);
          builder->addLoadConst(1);
          builder->addLoadStructField(1);
          builder->addCheckEqInt();

          builder->addJumpIf("field2 equal");
          builder->addLoadLitInt(0);
          builder->addRet();

          builder->label("field2 equal");
          builder->addLoadLitInt(1);
          builder->addRet();
          builder->addFail();

          // Print statement.
          builder->label("print");
          builder->addLoadLitString("hello");
          builder->addLoadLitInt(42);
          builder->addMakeStruct(2);

          builder->addLoadLitString("world");
          builder->addLoadLitInt(1337);
          builder->addMakeStruct(2);

          builder->addCall("UserEq");
          builder->addConvBoolString();
          builder->addPrintString();
          builder->addRet();
          builder->addFail();

          builder->addEntryPoint("print");
        });
  }
}

} // namespace backend
