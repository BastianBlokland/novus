#include "catch2/catch.hpp"
#include "helpers.hpp"
#include "vm/exec_state.hpp"

namespace vm {

TEST_CASE("[vm] Execute input and output", "vm") {

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

  SECTION("Read string from console") {
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          // Open console stdin stream.
          asmb->addLoadLitInt(0); // Stdin.
          asmb->addPCall(novasm::PCallCode::ConsoleOpenStream);

          asmb->addLoadLitInt(64); // Max string size.
          asmb->addPCall(novasm::PCallCode::StreamReadString);

          // Print string back out.
          ADD_PRINT(asmb);
        },
        "Hello world",
        "Hello world");
  }

  SECTION("Read characters from console") {
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          // Open console stdin stream.
          asmb->addLoadLitInt(0); // Stdin.
          asmb->addPCall(novasm::PCallCode::ConsoleOpenStream);
          asmb->addDup(); // Duplicate the stream on the stack.

          // Read and print the first character.
          asmb->addLoadLitInt(1); // Read a single character.
          asmb->addPCall(novasm::PCallCode::StreamReadString);
          ADD_PRINT(asmb);
          asmb->addPop(); // Ignore the result of printing.

          // Read and print the second character.
          asmb->addLoadLitInt(1); // Read a single character.
          asmb->addPCall(novasm::PCallCode::StreamReadString);
          ADD_PRINT(asmb);
        },
        "Hello world",
        "He");
  }

  SECTION("Write and read file") {
    const auto filePath = "test.tmp";
    CHECK_PROG(
        [&](novasm::Assembler* asmb) -> void {
          asmb->label("entry");
          asmb->setEntrypoint("entry");
          asmb->addStackAlloc(1);

          asmb->addLoadLitString(filePath);
          asmb->addLoadLitInt(0U | (1U << 8U)); // Options, mode 0 (Create) and flag 1 (AutoRemove).
          asmb->addPCall(novasm::PCallCode::FileOpenStream);
          asmb->addStackStore(0); // Store file-stream.

          // Write string to file.
          asmb->addStackLoad(0);                // Load stream.
          asmb->addLoadLitString("Hello worl"); // Content.
          asmb->addPCall(novasm::PCallCode::StreamWriteString);

          // Assert that writing succeeded.
          ADD_ASSERT(asmb);

          // Write character to file.
          asmb->addStackLoad(0);       // Load stream.
          asmb->addLoadLitString("d"); // Content.
          asmb->addPCall(novasm::PCallCode::StreamWriteString);

          // Assert that writing succeeded.
          ADD_ASSERT(asmb);

          // Open the file again for reading.
          asmb->addLoadLitString(filePath); // Load stream.
          asmb->addLoadLitInt(1U);          // Options, mode 1 (Open).
          asmb->addPCall(novasm::PCallCode::FileOpenStream);
          asmb->addStackStore(0); // Store file-stream.

          // Assert that file-stream is valid.
          asmb->addStackLoad(0); // Load stream.
          asmb->addPCall(novasm::PCallCode::StreamCheckValid);
          ADD_ASSERT(asmb);

          // Read and print the first character.
          asmb->addStackLoad(0);  // Load stream.
          asmb->addLoadLitInt(1); // Read a single character.
          asmb->addPCall(novasm::PCallCode::StreamReadString);
          ADD_PRINT(asmb);
          asmb->addPop(); // Ignore the result of printing.

          // Read and print the remaining characters.
          asmb->addStackLoad(0);    // Load stream.
          asmb->addLoadLitInt(64U); // Max chars to load.
          asmb->addPCall(novasm::PCallCode::StreamReadString);
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
          asmb->addLoadLitString("does-not-exist.tmp"); // Path.
          asmb->addLoadLitInt(1U);                      // Options, mode 1 (Open).
          asmb->addPCall(novasm::PCallCode::FileOpenStream);

          // Assert that file-stream is not valid.
          asmb->addPCall(novasm::PCallCode::StreamCheckValid);
          asmb->addCheckIntZero(); // Invert. // Invert to check if stream is not valid.
          ADD_ASSERT(asmb);
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
          asmb->addLoadLitString("does-not-exist.tmp"); // Path.
          asmb->addLoadLitInt(1U);                      // Options, mode 1 (Open).
          asmb->addPCall(novasm::PCallCode::FileOpenStream);

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
          asmb->addPCall(novasm::PCallCode::FileOpenStream);

          // Write to file.
          asmb->addStackLoad(0);          // Load stream.
          asmb->addLoadLitString("Test"); // Content.
          asmb->addPCall(novasm::PCallCode::StreamWriteString);

          // Assert that writing failed.
          asmb->addCheckIntZero(); // Invert. // Invert to check if writing failed.
          ADD_ASSERT(asmb);
          asmb->addRet();
        },
        "input",
        "");
  }

  SECTION("Environment arguments can be retrieved") {
    CHECK_PROG(
        [](novasm::Assembler* asmb) -> void {
          asmb->label("entry");
          asmb->setEntrypoint("entry");

          // Print number of environment arguments.
          asmb->addPCall(novasm::PCallCode::EnvGetArgCount);
          asmb->addConvIntString();
          ADD_PRINT(asmb);
          asmb->addPop(); // Ignore the result of printing.

          // Print environment argument 0.
          asmb->addLoadLitInt(0);
          asmb->addPCall(novasm::PCallCode::EnvGetArg);
          ADD_PRINT(asmb);
          asmb->addPop(); // Ignore the result of printing.

          // Print environment argument 1.
          asmb->addLoadLitInt(1);
          asmb->addPCall(novasm::PCallCode::EnvGetArg);
          ADD_PRINT(asmb);

          asmb->addRet();
        },
        "input",
        "2"
        "Test argument 1"
        "Test argument 2");
  }

  SECTION("non-existing environment argument returns empty string") {
    CHECK_PROG(
        [](novasm::Assembler* asmb) -> void {
          asmb->label("entry");
          asmb->setEntrypoint("entry");

          // Print a non-existing environment argument.
          asmb->addLoadLitInt(99);
          asmb->addPCall(novasm::PCallCode::EnvGetArg);
          ADD_PRINT(asmb);
          asmb->addPop(); // Ignore the result of printing.

          // Print a non-existing environment argument.
          asmb->addLoadLitInt(-1);
          asmb->addPCall(novasm::PCallCode::EnvGetArg);
          ADD_PRINT(asmb);

          asmb->addRet();
        },
        "input",
        "");
  }

  SECTION("Environment variable 'PATH' can be retrieved") {
    CHECK_PROG(
        [](novasm::Assembler* asmb) -> void {
          asmb->label("entry");
          asmb->setEntrypoint("entry");

          // Print a non existing environment variable.
          asmb->addLoadLitString("PATH"); // Var name.
          asmb->addPCall(novasm::PCallCode::EnvGetVar);

          // Check that the resulting string is not empty.
          asmb->addLengthString();
          asmb->addLoadLitInt(0);
          asmb->addCheckGtInt();

          ADD_ASSERT(asmb);

          asmb->addRet();
        },
        "input",
        "");
  }

  SECTION("Non-existing environment variable returns empty string") {
    CHECK_PROG(
        [](novasm::Assembler* asmb) -> void {
          asmb->label("entry");
          asmb->setEntrypoint("entry");

          // Print a non existing environment variable.
          asmb->addLoadLitString("ENVIRONMENT_VARIABLE_THAT_SHOULD_REALLY_NOT_EXIST"); // Var name.
          asmb->addPCall(novasm::PCallCode::EnvGetVar);
          ADD_PRINT(asmb);
          asmb->addPop(); // Ignore the result of printing.

          // Print a non existing environment variable.
          asmb->addLoadLitString(""); // Var name.
          asmb->addPCall(novasm::PCallCode::EnvGetVar);
          ADD_PRINT(asmb);

          asmb->addRet();
        },
        "input",
        "");
  }
}

} // namespace vm
