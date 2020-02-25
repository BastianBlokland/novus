#include "catch2/catch.hpp"
#include "helpers.hpp"

namespace backend {

TEST_CASE("Generating assembly for unions", "[backend]") {

  SECTION("Create user union and check for equality") {
    CHECK_PROG(
        "union Val = int, float "
        "conWrite(string(Val(1) == Val(1.0)))",
        [](novasm::Assembler* asmb) -> void {
          // --- Union equality function start.
          asmb->label("ValEq");
          // Check if both unions have the same type (field 0).
          asmb->addStackLoad(0);
          asmb->addLoadStructField(0);
          asmb->addStackLoad(1);
          asmb->addLoadStructField(0);
          asmb->addCheckEqInt();
          asmb->addJumpIf("typeEqual");

          // If not: return false.
          asmb->addLoadLitInt(0);
          asmb->addRet();

          asmb->label("typeEqual");
          // Check if the union type is int.
          asmb->addStackLoad(0);
          asmb->addLoadStructField(0);
          asmb->addLoadLitInt(0); // 0 because 'int' is the first type in the union.
          asmb->addCheckEqInt();
          asmb->addJumpIf("int");

          // Check if the union type is float.
          asmb->addStackLoad(0);
          asmb->addLoadStructField(0);
          asmb->addLoadLitInt(1); // 1 because 'float' is the second type in the union.
          asmb->addCheckEqInt();
          asmb->addJumpIf("float");

          // If union type is neither int or float terminate the program.
          asmb->addFail();

          // If type is int then check if the int value (field 1) is the same.
          asmb->label("int");
          asmb->addStackLoad(0);
          asmb->addLoadStructField(1);
          asmb->addStackLoad(1);
          asmb->addLoadStructField(1);
          asmb->addCheckEqInt();
          asmb->addJumpIf("valueEqual");
          asmb->addLoadLitInt(0);
          asmb->addRet();

          // If type is float then check if the float value (field 1) is the same.
          asmb->label("float");
          asmb->addStackLoad(0);
          asmb->addLoadStructField(1);
          asmb->addStackLoad(1);
          asmb->addLoadStructField(1);
          asmb->addCheckEqFloat();
          asmb->addJumpIf("valueEqual");
          asmb->addLoadLitInt(0);
          asmb->addRet();

          // If value is the same then return true.
          asmb->label("valueEqual");
          asmb->addLoadLitInt(1);
          asmb->addRet();
          // --- Struct equality function end.

          // --- conWrite statement start.
          asmb->label("prog");
          // Make union with int 'Val(1)'.
          asmb->addLoadLitInt(0); // 0 because 'int' is the first type in the union.
          asmb->addLoadLitInt(1);
          asmb->addMakeStruct(2);

          // Make union with float 'Val(1.0)'.
          asmb->addLoadLitInt(1); // 1 because 'float' is the second type in the union.
          asmb->addLoadLitFloat(1.0F);
          asmb->addMakeStruct(2);

          // Call equality function and write the result.
          asmb->addCall("ValEq", 2, novasm::CallMode::Normal);
          asmb->addConvBoolString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
          asmb->addRet();
          // --- conWrite statement end.

          asmb->setEntrypoint("prog");
        });
  }
}

} // namespace backend
