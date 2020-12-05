#include "catch2/catch.hpp"
#include "config.hpp"
#include "helpers.hpp"
#include "novasm/pcall_code.hpp"

namespace vm {

TEST_CASE("Execute miscellaneous pcalls", "[vm]") {

  SECTION("RtVersion") {
    CHECK_PROG(
        [](novasm::Assembler* asmb) -> void {
          asmb->label("start");
          asmb->addPCall(novasm::PCallCode::VersionRt);
          ADD_PRINT(asmb);
          asmb->addRet();

          asmb->setEntrypoint("start");
        },
        "input",
        PROJECT_VER);
  }

  SECTION("CompilerVersion") {
    CHECK_PROG(
        [](novasm::Assembler* asmb) -> void {
          asmb->label("start");
          asmb->addPCall(novasm::PCallCode::VersionCompiler);
          ADD_PRINT(asmb);
          asmb->addRet();

          asmb->setEntrypoint("start");
        },
        "input",
        "0.42.1337");
  }
}

} // namespace vm
