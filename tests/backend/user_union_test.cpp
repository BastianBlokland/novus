#include "catch2/catch.hpp"
#include "helpers.hpp"

namespace backend {

TEST_CASE("Generating assembly for user-unions", "[backend]") {

  SECTION("Create user union and check for equality") {
    CHECK_PROG(
        "union Val = int, float "
        "print(Val(1) == Val(1.0))",
        [](backend::Builder* builder) -> void {
          const auto intTypeId   = 1;
          const auto floatTypeId = 2;

          // --- Union equality function start.
          builder->label("ValEq");
          // Store both unions as consts.
          builder->addReserveConsts(2);
          builder->addStoreConst(0);
          builder->addStoreConst(1);

          // Check if both unions have the same type (field 0).
          builder->addLoadConst(0);
          builder->addLoadStructField(0);
          builder->addLoadConst(1);
          builder->addLoadStructField(0);
          builder->addCheckEqInt();
          builder->addJumpIf("typeEqual");

          // If not: return false.
          builder->addLoadLitInt(0);
          builder->addRet();

          builder->label("typeEqual");
          // Check if the union type is int.
          builder->addLoadConst(0);
          builder->addLoadStructField(0);
          builder->addLoadLitInt(intTypeId);
          builder->addCheckEqInt();
          builder->addJumpIf("int");

          // Check if the union type is float.
          builder->addLoadConst(0);
          builder->addLoadStructField(0);
          builder->addLoadLitInt(floatTypeId);
          builder->addCheckEqInt();
          builder->addJumpIf("float");

          // If union type is neither int or float terminate the program.
          builder->addFail();

          // If type is int then check if the int value (field 1) is the same.
          builder->label("int");
          builder->addLoadConst(0);
          builder->addLoadStructField(1);
          builder->addLoadConst(1);
          builder->addLoadStructField(1);
          builder->addCheckEqInt();
          builder->addJumpIf("valueEqual");
          builder->addLoadLitInt(0);
          builder->addRet();

          // If type is float then check if the float value (field 1) is the same.
          builder->label("float");
          builder->addLoadConst(0);
          builder->addLoadStructField(1);
          builder->addLoadConst(1);
          builder->addLoadStructField(1);
          builder->addCheckEqFloat();
          builder->addJumpIf("valueEqual");
          builder->addLoadLitInt(0);
          builder->addRet();

          // If value is the same then return true.
          builder->label("valueEqual");
          builder->addLoadLitInt(1);
          builder->addRet();
          builder->addFail();
          // --- Struct equality function end.

          // --- Print statement start.
          builder->label("print");
          // Make union with int 'Val(1)'.
          builder->addLoadLitInt(intTypeId);
          builder->addLoadLitInt(1);
          builder->addMakeStruct(2);

          // Make union with float 'Val(1.0)'.
          builder->addLoadLitInt(floatTypeId);
          builder->addLoadLitFloat(1.0F);
          builder->addMakeStruct(2);

          // Call equality function and print the result.
          builder->addCall("ValEq");
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
