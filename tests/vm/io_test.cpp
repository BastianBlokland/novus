#include "catch2/catch.hpp"
#include "helpers.hpp"
#include "vm/exec_state.hpp"

namespace vm {

TEST_CASE("Execute input and output", "[vm]") {

  SECTION("SleepNano") {
    CHECK_PROG(
        [](novasm::Assembler* asmb) -> void {
          asmb->label("entry");
          asmb->setEntrypoint("entry");

          // Its hard to test sleep, but at least this tests if the application exits cleanly.
          asmb->addLoadLitLong(0); // Sleep for 0 nanoseconds.
          asmb->addPCall(novasm::PCallCode::SleepNano);

          asmb->addRet();
        },
        "input",
        "");
  }

  SECTION("Assert") {
    CHECK_PROG_RESULTCODE(
        [](novasm::Assembler* asmb) -> void {
          asmb->label("entry");
          asmb->setEntrypoint("entry");

          asmb->addLoadLitInt(0);
          asmb->addLoadLitString("Fails");
          asmb->addPCall(novasm::PCallCode::Assert);

          asmb->addRet();
        },
        "input",
        ExecState::AssertFailed);
    CHECK_PROG(
        [](novasm::Assembler* asmb) -> void {
          asmb->label("entry");
          asmb->setEntrypoint("entry");

          asmb->addLoadLitInt(1);
          asmb->addLoadLitString("Does not fail");
          asmb->addPCall(novasm::PCallCode::Assert);

          asmb->addRet();
        },
        "input",
        "");
  }

  SECTION("Read from console") {
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          // Open console stdin stream.
          asmb->addLoadLitInt(0); // Stdin.
          asmb->addPCall(novasm::PCallCode::StreamOpenConsole);

          asmb->addLoadLitInt(64); // Max string size.
          asmb->addPCall(novasm::PCallCode::StreamReadString);

          // Print string back out.
          ADD_PRINT(asmb);
        },
        "Hello world",
        "Hello world");
  }

  SECTION("Write file") {
    const auto filePath = "test.tmp";
    CHECK_PROG(
        [&](novasm::Assembler* asmb) -> void {
          asmb->label("entry");
          asmb->setEntrypoint("entry");
          asmb->addStackAlloc(1);

          asmb->addLoadLitString(filePath);
          asmb->addLoadLitInt(0U | (1U << 8U)); // Options, mode 0 (Create) and flag 1 (AutoRemove).
          asmb->addPCall(novasm::PCallCode::StreamOpenFile);
          asmb->addStackStore(0); // Store file-stream.

          // Write string to file.
          asmb->addStackLoad(0);                // Load stream.
          asmb->addLoadLitString("Hello worl"); // Content.
          asmb->addPCall(novasm::PCallCode::StreamWriteString);

          // Assert that writing succeeded.
          asmb->addLoadLitString("Write failed");
          asmb->addPCall(novasm::PCallCode::Assert);

          // Write character to file.
          asmb->addStackLoad(0);    // Load stream.
          asmb->addLoadLitInt('d'); // Content.
          asmb->addPCall(novasm::PCallCode::StreamWriteChar);

          // Assert that writing succeeded.
          asmb->addLoadLitString("Write failed");
          asmb->addPCall(novasm::PCallCode::Assert);

          // Flush file.
          asmb->addStackLoad(0); // Load stream.
          asmb->addPCall(novasm::PCallCode::StreamFlush);

          // Assert that flushing succeeded.
          asmb->addLoadLitString("Flush failed");
          asmb->addPCall(novasm::PCallCode::Assert);

          // Open the file again for reading.
          asmb->addLoadLitString(filePath); // Load stream.
          asmb->addLoadLitInt(1U);          // Options, mode 1 (Open).
          asmb->addPCall(novasm::PCallCode::StreamOpenFile);
          asmb->addStackStore(0); // Store file-stream.

          // Assert that file-stream is valid.
          asmb->addStackLoad(0); // Load stream.
          asmb->addPCall(novasm::PCallCode::StreamCheckValid);
          asmb->addLoadLitString("Stream not valid");
          asmb->addPCall(novasm::PCallCode::Assert);

          // Read file content.
          asmb->addStackLoad(0);    // Load stream.
          asmb->addLoadLitInt(64U); // Max chars to load.
          asmb->addPCall(novasm::PCallCode::StreamReadString);

          // Print file content.
          ADD_PRINT(asmb);
          asmb->addRet();
        },
        "input",
        "Hello world");
  }

  SECTION("Non-existing file is not valid") {
    CHECK_PROG(
        [](novasm::Assembler* asmb) -> void {
          asmb->label("entry");
          asmb->setEntrypoint("entry");

          // Open steam to non existing file.
          asmb->addLoadLitString("test.tmp"); // Path.
          asmb->addLoadLitInt(1U);            // Options, mode 1 (Open).
          asmb->addPCall(novasm::PCallCode::StreamOpenFile);

          // Assert that file-stream is valid.
          asmb->addPCall(novasm::PCallCode::StreamCheckValid);
          asmb->addLogicInvInt(); // Invert to check if stream is not valid.
          asmb->addLoadLitString("Stream not valid");
          asmb->addPCall(novasm::PCallCode::Assert);
          asmb->addRet();
        },
        "input",
        "");
  }

  SECTION("Read from non-existing file returns empty string") {
    CHECK_PROG(
        [](novasm::Assembler* asmb) -> void {
          asmb->label("entry");
          asmb->setEntrypoint("entry");

          // Open steam to non existing file.
          asmb->addLoadLitString("test.tmp"); // Path.
          asmb->addLoadLitInt(1U);            // Options, mode 1 (Open).
          asmb->addPCall(novasm::PCallCode::StreamOpenFile);

          // Read file content.
          asmb->addStackLoad(0);    // Load stream.
          asmb->addLoadLitInt(64U); // Max chars to load.
          asmb->addPCall(novasm::PCallCode::StreamReadString);

          // Print file content.
          ADD_PRINT(asmb);
          asmb->addRet();
        },
        "input",
        "");
  }

  SECTION("Write to from non-existing file in open-mode fails") {
    CHECK_PROG(
        [](novasm::Assembler* asmb) -> void {
          asmb->label("entry");
          asmb->setEntrypoint("entry");

          // Open steam to non existing file.
          asmb->addLoadLitString("test.tmp"); // Path.
          asmb->addLoadLitInt(1U);            // Options, mode 1 (Open).
          asmb->addPCall(novasm::PCallCode::StreamOpenFile);

          // Write to file.
          asmb->addStackLoad(0);          // Load stream.
          asmb->addLoadLitString("Test"); // Content.
          asmb->addPCall(novasm::PCallCode::StreamWriteString);

          // Assert that writing failed.
          asmb->addLogicInvInt(); // Invert to check if writing failed.
          asmb->addLoadLitString("Write failed");
          asmb->addPCall(novasm::PCallCode::Assert);
          asmb->addRet();
        },
        "input",
        "");
  }
}

} // namespace vm
