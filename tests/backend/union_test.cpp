#include "catch2/catch.hpp"
#include "helpers.hpp"

namespace backend {

TEST_CASE("Generating assembly for unions", "[backend]") {

  SECTION("Normal union") {
    CHECK_PROG(
        "union Val = int, float "
        "conWrite(string(Val(1) == Val(1.0)))"
        "conWrite(string(Val(1) is int))"
        "conWrite(Val(1) as int i ? string(i) : \"not int\")",
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

          // --- first conWrite statement start.
          asmb->label("conWrite1");

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
          // --- first conWrite statement end.

          // --- second conWrite statement start.
          asmb->label("conWrite2");

          // Make union with int 'Val(1)'.
          asmb->addLoadLitInt(0); // 0 because 'int' is the first type in the union.
          asmb->addLoadLitInt(1);
          asmb->addMakeStruct(2);

          // Check if union as discriminant for type int.
          asmb->addLoadStructField(0); // Load discriminant.
          asmb->addLoadLitInt(0);      // int is first type in union so discriminant is 0.
          asmb->addCheckEqInt();

          // Write the result.
          asmb->addConvBoolString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
          asmb->addRet();
          // --- second conWrite statement end.

          // --- third conWrite statement start.
          asmb->label("conWrite3");
          asmb->addStackAlloc(1); // Allocate space for constant 'i'.

          // Make union with int 'Val(1)'.
          asmb->addLoadLitInt(0); // 0 because 'int' is the first type in the union.
          asmb->addLoadLitInt(1);
          asmb->addMakeStruct(2);

          // Check if the discriminant is int.
          asmb->addDup();
          asmb->addLoadStructField(0); // Load discriminant.
          asmb->addLoadLitInt(0);      // int is first type in union so discriminant is 0.
          asmb->addCheckEqInt();
          asmb->addJumpIf("as-check-type-is-int");

          // Not int, set result of 'as' to false.
          asmb->addPop();
          asmb->addLoadLitInt(0);
          asmb->addJump("as-check-end");

          asmb->label("as-check-type-is-int");

          // Store value in constant 'i' and set result to true.
          asmb->addLoadStructField(1); // Value of union.
          asmb->addStackStore(0);      // Assign to constant 'i'.
          asmb->addLoadLitInt(1);      // Load true.

          asmb->label("as-check-end");
          asmb->addJumpIf("as-check-true");

          // Not equal.
          asmb->addLoadLitString("not int");
          asmb->addJump("as-check-write-result");

          asmb->label("as-check-true");
          asmb->addStackLoad(0); // Load constant 'i'.
          asmb->addConvIntString();

          // Write the result.
          asmb->label("as-check-write-result");
          asmb->addPCall(novasm::PCallCode::ConWriteString);
          asmb->addRet();
          // --- third conWrite statement end.

          asmb->label("prog");
          asmb->addCall("conWrite1", 0, novasm::CallMode::Normal);
          asmb->addCall("conWrite2", 0, novasm::CallMode::Normal);
          asmb->addCall("conWrite3", 0, novasm::CallMode::Normal);
          asmb->addRet();

          asmb->setEntrypoint("prog");
        });
  }

  SECTION("Nullable-struct union") {
    /* Nullable-struct unions are an optimization where unions contains a struct with 2 or more
    fields and an empty struct (tag type) are represented by single struct. */
    CHECK_PROG(
        "struct User = string name, int age "
        "struct Null "
        "union NullableUser = User, Null "
        "conWrite(string(NullableUser(User(\"John\", 42)) == Null()))"
        "conWrite(string(NullableUser(User(\"John\", 42)) is Null))"
        "conWrite(string(NullableUser(User(\"John\", 42)) is User))"
        "conWrite(NullableUser(User(\"John\", 42))  as User u ? u.name : \"null\")",
        [](novasm::Assembler* asmb) -> void {
          // -- struct 'Null' equality function start.
          asmb->label("null-eq");
          asmb->addLoadLitInt(1); // Always returns true.
          asmb->addRet();
          // -- struct 'Null' equality function end.

          // -- union 'NullableUser' equality function start.
          asmb->label("nullableuser-eq");

          // Check if union a is null.
          asmb->addStackLoad(0);
          asmb->addCheckStructNull();
          asmb->addJumpIf("nullableuser-eq-a-null");

          // Check if union a is null.
          asmb->addStackLoad(1);
          asmb->addCheckStructNull();
          asmb->addJumpIf("nullableuser-eq-b-null");

          // If neither a nor b is null then call the equality function of the user struct.
          asmb->addStackLoad(0);
          asmb->addStackLoad(1);
          asmb->addCall("user-eq", 2, novasm::CallMode::Tail);
          // Note, tailcall so no ret instruction needed here.

          asmb->label("nullableuser-eq-a-null");

          // If a is null then return true if b is also null, otherwise return false.
          asmb->addStackLoad(1);
          asmb->addCheckStructNull();
          asmb->addRet();

          asmb->label("nullableuser-eq-b-null");

          // If b is null (and a was not null) then return false.
          asmb->addLoadLitInt(0);
          asmb->addRet();
          // -- union 'NullableUser' equality function end.

          // -- struct 'User' equality function start.
          asmb->label("user-eq");

          // Check if field 1 is equal.
          asmb->addStackLoad(0);
          asmb->addLoadStructField(0);
          asmb->addStackLoad(1);
          asmb->addLoadStructField(0);
          asmb->addCheckEqString();
          asmb->addJumpIf("user-eq-field1equal");

          // If field 1 is not equal then return false.
          asmb->addLoadLitInt(0);
          asmb->addRet();

          asmb->label("user-eq-field1equal");

          // Check if field 2 is equal.
          asmb->addStackLoad(0);
          asmb->addLoadStructField(1);
          asmb->addStackLoad(1);
          asmb->addLoadStructField(1);
          asmb->addCheckEqInt();
          asmb->addJumpIf("user-eq-field2equal");

          // If field 2 is not equal then return false.
          asmb->addLoadLitInt(0);
          asmb->addRet();

          asmb->label("user-eq-field2equal");

          // Both are equal, return true.
          asmb->addLoadLitInt(1);
          asmb->addRet();
          // -- struct 'User' equality function end.

          // --- first conWrite statement start.
          asmb->label("conWrite1");

          // Create user.
          asmb->addLoadLitString("John");
          asmb->addLoadLitInt(42);
          asmb->addMakeStruct(2);

          // Create 'null'.
          asmb->addMakeNullStruct();

          // Call equality function of Nullable-user.
          asmb->addCall("nullableuser-eq", 2, novasm::CallMode::Normal);

          // Write the result.
          asmb->addConvBoolString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
          asmb->addRet();
          // --- first conWrite statement end.

          // --- second conWrite statement start.
          asmb->label("conWrite2");

          // Create user.
          asmb->addLoadLitString("John");
          asmb->addLoadLitInt(42);
          asmb->addMakeStruct(2);

          // Check if user is null.
          asmb->addCheckStructNull();

          // Write the result.
          asmb->addConvBoolString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
          asmb->addRet();
          // --- second conWrite statement end.

          // --- third conWrite statement start.
          asmb->label("conWrite3");

          // Create user.
          asmb->addLoadLitString("John");
          asmb->addLoadLitInt(42);
          asmb->addMakeStruct(2);

          // Check if user is not null.
          asmb->addCheckStructNull();
          asmb->addLogicInvInt();

          // Write the result.
          asmb->addConvBoolString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
          asmb->addRet();
          // --- third conWrite statement end.

          // --- fourth conWrite statement start.
          asmb->label("conWrite4");
          asmb->addStackAlloc(1); // Allocate space for the 'u' constant.

          // Create user.
          asmb->addLoadLitString("John");
          asmb->addLoadLitInt(42);
          asmb->addMakeStruct(2);

          // Check if user is not null and set constant 'u'.
          asmb->addDup();
          asmb->addStackStore(0);
          asmb->addCheckStructNull();
          asmb->addLogicInvInt();
          asmb->addJumpIf("user-is-not-null");

          // Is null.
          asmb->addLoadLitString("null");
          asmb->addJump("fourth-conwrite-end");

          asmb->label("user-is-not-null");
          // Load name of user.
          asmb->addStackLoad(0);
          asmb->addLoadStructField(0);

          asmb->label("fourth-conwrite-end");
          // Print the result.
          asmb->addPCall(novasm::PCallCode::ConWriteString);
          asmb->addRet();
          // --- fourth conWrite statement end.

          asmb->label("prog");
          asmb->addCall("conWrite1", 0, novasm::CallMode::Normal);
          asmb->addCall("conWrite2", 0, novasm::CallMode::Normal);
          asmb->addCall("conWrite3", 0, novasm::CallMode::Normal);
          asmb->addCall("conWrite4", 0, novasm::CallMode::Normal);
          asmb->addRet();

          asmb->setEntrypoint("prog");
        });
  }
}

} // namespace backend
