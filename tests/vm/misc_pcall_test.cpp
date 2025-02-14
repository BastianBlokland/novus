#include "catch2/catch.hpp"
#include "config.hpp"
#include "helpers.hpp"
#include "input/search_paths.hpp"
#include "novasm/pcall_code.hpp"

namespace vm {

TEST_CASE("[vm] Execute miscellaneous pcalls", "vm") {

  SECTION("Endianness") {
    /* This test is left for the first person to try to run this on a big-endian machine :)
     * In most places endianness is somewhat taken into account but has definitively never been run
     * on a big-endian machine so will probably blow up in epic ways.
     */
    auto expectedEndianness = "0"; // LittleEndian
    CHECK_PROG(
        [](novasm::Assembler* asmb) -> void {
          asmb->label("start");
          asmb->addPCall(novasm::PCallCode::EndiannessNative);
          asmb->addConvIntString();
          ADD_PRINT(asmb);
          asmb->addRet();

          asmb->setEntrypoint("start");
        },
        "input",
        expectedEndianness);
  }

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
#if defined(linux) || defined(__linux__)
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

          ADD_BOOL_TO_STRING(asmb);
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
