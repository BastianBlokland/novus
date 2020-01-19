#include "catch2/catch.hpp"
#include "helpers.hpp"

namespace backend {

TEST_CASE("Generating assembly for structs", "[backend]") {

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
          // --- Struct equality function end.

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
          builder->addCall("UserEq", false);
          builder->addConvBoolString();
          builder->addPrintString();
          builder->addPop();
          builder->addRet();
          // --- Print statement end.

          builder->addEntryPoint("print");
        });
  }

  SECTION("Create empty struct and check for equality") {
    CHECK_PROG(
        "struct Empty "
        "print(Empty() == Empty())",
        [](backend::Builder* builder) -> void {
          // --- Struct equality function start.
          builder->label("UserEq");
          builder->addPop();
          builder->addPop();
          builder->addLoadLitInt(1); // Empty structs are always equal.
          builder->addRet();
          // --- Struct equality function end.

          // --- Print statement start.
          builder->label("print");
          // Make struct 1 'Empty()'.
          builder->addLoadLitInt(0); // Empty struct is represented by '0'.

          // Make struct 2 'Empty()'.
          builder->addLoadLitInt(0); // Empty struct is represented by '0'.

          // Call the equality function and print the result.
          builder->addCall("UserEq", false);
          builder->addConvBoolString();
          builder->addPrintString();
          builder->addPop();
          builder->addRet();
          // --- Print statement end.

          builder->addEntryPoint("print");
        });

    SECTION("Create struct with one field, check for equality and load field") {
      CHECK_PROG(
          "struct Age = int years "
          "print(Age(42) == Age(1337)) "
          "print(Age(42).years)",
          [](backend::Builder* builder) -> void {
            // --- Struct equality function start.
            builder->label("UserEq");
            builder->addCheckEqInt(); // Check the field itself.
            builder->addRet();
            // --- Struct equality function end.

            // --- Print statement 1 start.
            builder->label("print1");
            // Make struct 1 'Age(42)'.
            builder->addLoadLitInt(42); // Struct with 1 field is represented by the field itself.

            // Make struct 2 'Age(1337)'.
            builder->addLoadLitInt(1337); // Struct with 1 field is represented by the field itself.

            // Call the equality function and print the result.
            builder->addCall("UserEq", false);
            builder->addConvBoolString();
            builder->addPrintString();
            builder->addPop();
            builder->addRet();
            // --- Print statement 1 end.

            // --- Print statement 2 start.
            builder->label("print2");
            // Make struct 'Age(42)'.
            builder->addLoadLitInt(42); // Struct with 1 field is represented by the field itself.

            builder->addConvIntString();
            builder->addPrintString();
            builder->addPop();
            builder->addRet();
            // --- Print statement 2 end.

            builder->addEntryPoint("print1");
            builder->addEntryPoint("print2");
          });
    }
  }
}

} // namespace backend
