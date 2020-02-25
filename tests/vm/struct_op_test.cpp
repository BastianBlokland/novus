#include "catch2/catch.hpp"
#include "helpers.hpp"

namespace vm {

TEST_CASE("Execute struct operations", "[vm]") {

  SECTION("Load field") {
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addStackAlloc(1);

          asmb->addLoadLitString("hello ");
          asmb->addLoadLitString("world");
          asmb->addMakeStruct(2);
          asmb->addStackStore(0);

          asmb->addStackLoad(0);
          asmb->addLoadStructField(0);

          asmb->addStackLoad(0);
          asmb->addLoadStructField(1);

          asmb->addAddString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
          asmb->addPop();
        },
        "input",
        "hello world");
  }
}

} // namespace vm
