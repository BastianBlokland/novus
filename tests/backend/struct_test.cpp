#include "catch2/catch.hpp"
#include "helpers.hpp"

namespace backend {

TEST_CASE("Generating assembly for structs", "[backend]") {

  SECTION("Normal struct") {
    CHECK_PROG(
        "struct User = string name, int age "
        "conWrite(string(User(\"hello\", 42) == User(\"world\", 1337)))",
        [](novasm::Assembler* asmb) -> void {
          // --- Struct equality function start.
          asmb->label("UserEq");
          // Check if field 0 is the same.
          asmb->addStackLoad(0);
          asmb->addLoadStructField(0);
          asmb->addStackLoad(1);
          asmb->addLoadStructField(0);
          asmb->addCheckEqString();

          asmb->addJumpIf("field0 equal");
          // If not: return false.
          asmb->addLoadLitInt(0);
          asmb->addRet();

          // Check if field 1 is the same.
          asmb->label("field0 equal");
          asmb->addStackLoad(0);
          asmb->addLoadStructField(1);
          asmb->addStackLoad(1);
          asmb->addLoadStructField(1);
          asmb->addCheckEqInt();

          asmb->addJumpIf("field1 equal");
          // If not: return false.
          asmb->addLoadLitInt(0);
          asmb->addRet();

          asmb->label("field1 equal");
          // If both are equal then return true.
          asmb->addLoadLitInt(1);
          asmb->addRet();
          // --- Struct equality function end.

          // --- conWrite statement start.
          asmb->label("prog");
          // Make struct 1 'User("hello", 42)'.
          asmb->addLoadLitString("hello");
          asmb->addLoadLitInt(42);
          asmb->addMakeStruct(2);

          // Make struct 2 'User("world", 1337)'.
          asmb->addLoadLitString("world");
          asmb->addLoadLitInt(1337);
          asmb->addMakeStruct(2);

          // Call the equality function and write the result.
          asmb->addCall("UserEq", 2, novasm::CallMode::Normal);
          asmb->addConvBoolString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
          asmb->addRet();
          // --- conWrite statement end.

          asmb->setEntrypoint("prog");
        });
  }

  SECTION("Empty struct (tag type)") {
    CHECK_PROG(
        "struct Empty "
        "conWrite(string(Empty() == Empty()))",
        [](novasm::Assembler* asmb) -> void {
          // --- Struct equality function start.
          asmb->label("UserEq");

          asmb->addLoadLitInt(1); // Empty structs are always equal.
          asmb->addRet();
          // --- Struct equality function end.

          // --- conWrite statement start.
          asmb->label("prog");
          // Make struct 1 'Empty()'.
          asmb->addMakeNullStruct(); // Empty struct is represented by a null-struct.

          // Make struct 2 'Empty()'.
          asmb->addMakeNullStruct(); // Empty struct is represented by a null-struct.

          // Call the equality function and write the result.
          asmb->addCall("UserEq", 2, novasm::CallMode::Normal);
          asmb->addConvBoolString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
          asmb->addRet();
          // --- conWrite statement end.

          asmb->setEntrypoint("prog");
        });

    SECTION("One field struct") {
      CHECK_PROG(
          "struct Age = int years "
          "conWrite(string(Age(42) == Age(1337))) "
          "conWrite(string(Age(42).years))",
          [](novasm::Assembler* asmb) -> void {
            // --- Struct equality function start.
            asmb->label("UserEq");
            asmb->addStackLoad(0);
            asmb->addStackLoad(1);
            asmb->addCheckEqInt(); // Check the field itself.
            asmb->addRet();
            // --- Struct equality function end.

            // --- conWrite statement 1 start.
            asmb->label("write1");
            // Make struct 1 'Age(42)'.
            asmb->addLoadLitInt(42); // Struct with 1 field is represented by the field itself.

            // Make struct 2 'Age(1337)'.
            asmb->addLoadLitInt(1337); // Struct with 1 field is represented by the field itself.

            // Call the equality function and write the result.
            asmb->addCall("UserEq", 2, novasm::CallMode::Normal);
            asmb->addConvBoolString();
            asmb->addPCall(novasm::PCallCode::ConWriteString);
            asmb->addRet();
            // --- conWrite statement 1 end.

            // --- conWrite statement 2 start.
            asmb->label("write2");
            // Make struct 'Age(42)'.
            asmb->addLoadLitInt(42); // Struct with 1 field is represented by the field itself.

            asmb->addConvIntString();
            asmb->addPCall(novasm::PCallCode::ConWriteString);
            asmb->addRet();
            // --- conWrite statement 2 end.

            // Entry point that calls both writes.
            asmb->label("entry");
            asmb->addCall("write1", 0, novasm::CallMode::Normal);
            asmb->addCall("write2", 0, novasm::CallMode::Normal);
            asmb->addRet();

            asmb->setEntrypoint("entry");
          });
    }
  }
}

} // namespace backend
