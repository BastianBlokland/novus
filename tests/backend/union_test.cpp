#include "catch2/catch.hpp"
#include "helpers.hpp"

namespace backend {

TEST_CASE("Generating assembly for unions", "[backend]") {

  SECTION("Create user union and check for equality") {
    CHECK_PROG(
        "union Val = int, float "
        "conWrite(string(Val(1) == Val(1.0)))",
        [](backend::Builder* builder) -> void {
          // --- Union equality function start.
          builder->label("ValEq");
          // Check if both unions have the same type (field 0).
          builder->addStackLoad(0);
          builder->addLoadStructField(0);
          builder->addStackLoad(1);
          builder->addLoadStructField(0);
          builder->addCheckEqInt();
          builder->addJumpIf("typeEqual");

          // If not: return false.
          builder->addLoadLitInt(0);
          builder->addRet();

          builder->label("typeEqual");
          // Check if the union type is int.
          builder->addStackLoad(0);
          builder->addLoadStructField(0);
          builder->addLoadLitInt(0); // 0 because 'int' is the first type in the union.
          builder->addCheckEqInt();
          builder->addJumpIf("int");

          // Check if the union type is float.
          builder->addStackLoad(0);
          builder->addLoadStructField(0);
          builder->addLoadLitInt(1); // 1 because 'float' is the second type in the union.
          builder->addCheckEqInt();
          builder->addJumpIf("float");

          // If union type is neither int or float terminate the program.
          builder->addFail();

          // If type is int then check if the int value (field 1) is the same.
          builder->label("int");
          builder->addStackLoad(0);
          builder->addLoadStructField(1);
          builder->addStackLoad(1);
          builder->addLoadStructField(1);
          builder->addCheckEqInt();
          builder->addJumpIf("valueEqual");
          builder->addLoadLitInt(0);
          builder->addRet();

          // If type is float then check if the float value (field 1) is the same.
          builder->label("float");
          builder->addStackLoad(0);
          builder->addLoadStructField(1);
          builder->addStackLoad(1);
          builder->addLoadStructField(1);
          builder->addCheckEqFloat();
          builder->addJumpIf("valueEqual");
          builder->addLoadLitInt(0);
          builder->addRet();

          // If value is the same then return true.
          builder->label("valueEqual");
          builder->addLoadLitInt(1);
          builder->addRet();
          // --- Struct equality function end.

          // --- conWrite statement start.
          builder->label("prog");
          // Make union with int 'Val(1)'.
          builder->addLoadLitInt(0); // 0 because 'int' is the first type in the union.
          builder->addLoadLitInt(1);
          builder->addMakeStruct(2);

          // Make union with float 'Val(1.0)'.
          builder->addLoadLitInt(1); // 1 because 'float' is the second type in the union.
          builder->addLoadLitFloat(1.0F);
          builder->addMakeStruct(2);

          // Call equality function and write the result.
          builder->addCall("ValEq", 2, CallMode::Normal);
          builder->addConvBoolString();
          builder->addPCall(vm::PCallCode::ConWriteString);
          builder->addRet();
          // --- conWrite statement end.

          builder->addEntryPoint("prog");
        });
  }
}

} // namespace backend
