#include "catch2/catch.hpp"
#include "config.hpp"
#include "helpers.hpp"
#include "input/search_paths.hpp"
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

  SECTION("PlatformCode") {
#if defined(linux)
    const auto expectedResult = "1";
#elif defined(__APPLE__) // !linux
    const auto expectedResult = "2";
#elif defined(_WIN32)    // !linux && !__APPLE__
    const auto expectedResult = "3";
#endif
    CHECK_PROG(
        [](novasm::Assembler* asmb) -> void {
          asmb->label("start");
          asmb->addPCall(novasm::PCallCode::PlatformCode);
          asmb->addConvIntString();
          ADD_PRINT(asmb);
          asmb->addRet();

          asmb->setEntrypoint("start");
        },
        "input",
        expectedResult);
  }

  SECTION("WorkingDirPath") {
    CHECK_PROG(
        [](novasm::Assembler* asmb) -> void {
          asmb->label("start");
          asmb->addPCall(novasm::PCallCode::WorkingDirPath);

          // Check if the result is not an empty string.
          asmb->addLengthString();
          asmb->addLoadLitInt(0);
          asmb->addCheckEqInt();

          asmb->addConvBoolString();
          ADD_PRINT(asmb);
          asmb->addRet();

          asmb->setEntrypoint("start");
        },
        "input",
        "false");
  }

  SECTION("RtPath") {
    CHECK_PROG(
        [](novasm::Assembler* asmb) -> void {
          asmb->label("start");
          asmb->addPCall(novasm::PCallCode::RtPath);
          ADD_PRINT(asmb);
          asmb->addRet();

          asmb->setEntrypoint("start");
        },
        "input",
        input::getExecutablePath().string());
  }

  SECTION("ProgramPath") {
    // Because we execute the tests with inline programs, ProgramPath should return an empty string.
    CHECK_PROG(
        [](novasm::Assembler* asmb) -> void {
          asmb->label("start");
          asmb->addPCall(novasm::PCallCode::ProgramPath);
          ADD_PRINT(asmb);
          asmb->addRet();

          asmb->setEntrypoint("start");
        },
        "input",
        "");
  }
}

} // namespace vm
