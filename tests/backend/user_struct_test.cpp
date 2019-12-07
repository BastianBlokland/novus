#include "catch2/catch.hpp"
#include "helpers.hpp"

namespace backend {

TEST_CASE("Generating assembly for user-structs", "[backend]") {

  SECTION("Create user struct and check for equality") {
    CHECK_PROG(
        "struct User = string name, int age "
        "print(User(\"hello\", 42) == User(\"world\", 1337))",
        [](backend::Builder* builder) -> void {
          // --- Struct equality function start.
          builder->label("UserEq");
          // Store both structs as consts.
          builder->addReserveConsts(2);
          builder->addStoreConst(0);
          builder->addStoreConst(1);

          // Check if field 0 is the same.
          builder->addLoadConst(0);
          builder->addLoadStructField(0);
          builder->addLoadConst(1);
          builder->addLoadStructField(0);
          builder->addCheckEqString();

          builder->addJumpIf("field0 equal");
          // If not: return false.
          builder->addLoadLitInt(0);
          builder->addRet();

          // Check if field 1 is the same.
          builder->label("field0 equal");
          builder->addLoadConst(0);
          builder->addLoadStructField(1);
          builder->addLoadConst(1);
          builder->addLoadStructField(1);
          builder->addCheckEqInt();

          builder->addJumpIf("field1 equal");
          // If not: return false.
          builder->addLoadLitInt(0);
          builder->addRet();

          builder->label("field1 equal");
          // If both are equal then return true.
          builder->addLoadLitInt(1);
          builder->addRet();
          builder->addFail();
          // --- Struct equality function start.

          // --- Print statement start.
          builder->label("print");
          // Make struct 1 'User("hello", 42)'.
          builder->addLoadLitString("hello");
          builder->addLoadLitInt(42);
          builder->addMakeStruct(2);

          // Make struct 2 'User("world", 1337)'.
          builder->addLoadLitString("world");
          builder->addLoadLitInt(1337);
          builder->addMakeStruct(2);

          // Call the equality function and print the result.
          builder->addCall("UserEq");
          builder->addConvBoolString();
          builder->addPrintString();
          builder->addRet();
          builder->addFail();
          // --- Print statement end.

          builder->addEntryPoint("print");
        });
  }
}

} // namespace backend
