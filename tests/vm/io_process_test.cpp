#include "catch2/catch.hpp"
#include "helpers.hpp"
#include "input/search_paths.hpp"
#include "novasm/pcall_code.hpp"
#include "vm/exec_state.hpp"

namespace vm {

#define WRITE_STD_IN(ASMB)                                                                         \
  {                                                                                                \
    (ASMB)->addSwap(); /* Swap because the process needs to be on the stack before the string. */  \
    (ASMB)->addLoadLitInt(0); /* StdIn. */                                                         \
    (ASMB)->addPCall(novasm::PCallCode::ProcessOpenStream);                                        \
    (ASMB)->addSwap(); /* Swap because the stream needs to be on the stack before the string. */   \
    (ASMB)->addPCall(novasm::PCallCode::StreamWriteString);                                        \
  }

#define READ_STD_OUT(ASMB)                                                                         \
  {                                                                                                \
    (ASMB)->addLoadLitInt(1); /* StdOut. */                                                        \
    (ASMB)->addPCall(novasm::PCallCode::ProcessOpenStream);                                        \
    (ASMB)->addLoadLitInt(1024); /* Read up to 1024 bytes. */                                      \
    (ASMB)->addPCall(novasm::PCallCode::StreamReadString);                                         \
  }

#define READ_STD_ERR(ASMB)                                                                         \
  {                                                                                                \
    (ASMB)->addLoadLitInt(2); /* StdErr. */                                                        \
    (ASMB)->addPCall(novasm::PCallCode::ProcessOpenStream);                                        \
    (ASMB)->addLoadLitInt(1024); /* Read up to 1024 bytes. */                                      \
    (ASMB)->addPCall(novasm::PCallCode::StreamReadString);                                         \
  }

TEST_CASE("[vm] Execute process platform-calls", "vm") {

  auto novePath = (input::getExecutablePath().parent_path() / "nove").string();

  SECTION("Block waits until process is finished and produces the exitcode") {
    CHECK_PROG(
        [&](novasm::Assembler* asmb) -> void {
          asmb->label("entry");
          asmb->setEntrypoint("entry");

          // Run a program that exits with code 0.
          asmb->addLoadLitString(novePath + " 'assert(true)'");
          asmb->addPCall(novasm::PCallCode::ProcessStart);
          asmb->addPCall(novasm::PCallCode::ProcessBlock);

          // Print the exit-code.
          asmb->addConvIntString();
          ADD_PRINT(asmb);

          // Run a program that exits with code 1.
          asmb->addLoadLitString(novePath + " 'fail()'");
          asmb->addPCall(novasm::PCallCode::ProcessStart);
          asmb->addPCall(novasm::PCallCode::ProcessBlock);

          // Print the exit-code.
          asmb->addConvIntString();
          ADD_PRINT(asmb);

          // Run a program that exits with code 14.
          asmb->addLoadLitString(novePath + " 'assert(1 / 0 == 1)'");
          asmb->addPCall(novasm::PCallCode::ProcessStart);
          asmb->addPCall(novasm::PCallCode::ProcessBlock);

          // Print the exit-code.
          asmb->addConvIntString();
          ADD_PRINT(asmb);

          asmb->addRet();
        },
        "input",
        "0114");
  }

  SECTION("StdOut can be read from a child-process") {
    CHECK_PROG(
        [&](novasm::Assembler* asmb) -> void {
          asmb->label("entry");
          asmb->setEntrypoint("entry");

          // Run a program that prints to stdOut.
          asmb->addLoadLitString(novePath + " 'print(\"Hello world\")'");
          asmb->addPCall(novasm::PCallCode::ProcessStart);

          asmb->addDup(); // Duplicate the process.
          asmb->addPCall(novasm::PCallCode::ProcessBlock);
          asmb->addPop(); // Ignore the exit code.

          asmb->addDup(); // Duplicate the process.
          READ_STD_OUT(asmb);
          ADD_PRINT(asmb);
          asmb->addPop(); // Ignore the success / failure of print.

          // Also read std-err to assert that its empty.
          READ_STD_ERR(asmb);
          ADD_PRINT(asmb);
          asmb->addPop(); // Ignore the success / failure of print.

          asmb->addRet();
        },
        "input",
        "Hello world\n");
  }

  SECTION("StdErr can be read from a child-process") {
    CHECK_PROG(
        [&](novasm::Assembler* asmb) -> void {
          asmb->label("entry");
          asmb->setEntrypoint("entry");

          // Run a program that prints to stdErr.
          asmb->addLoadLitString(novePath + " 'printErr(\"Hello world\")'");
          asmb->addPCall(novasm::PCallCode::ProcessStart);

          asmb->addDup(); // Duplicate the process.
          asmb->addPCall(novasm::PCallCode::ProcessBlock);
          asmb->addPop(); // Ignore the exit code.

          asmb->addDup(); // Duplicate the process.
          READ_STD_ERR(asmb);
          ADD_PRINT(asmb);
          asmb->addPop(); // Ignore the success / failure of print.

          // Also read std-out to assert that its empty.
          READ_STD_OUT(asmb);
          ADD_PRINT(asmb);
          asmb->addPop(); // Ignore the success / failure of print.

          asmb->addRet();
        },
        "input",
        "Hello world\n");
  }

  SECTION("StdIn of a child-process can be written to") {
    CHECK_PROG(
        [&](novasm::Assembler* asmb) -> void {
          asmb->label("entry");
          asmb->setEntrypoint("entry");

          // Run a program that reads a line from stdIn and write to stdOut.
          asmb->addLoadLitString(novePath + " 'print(readLine())'");
          asmb->addPCall(novasm::PCallCode::ProcessStart);
          asmb->addDup(); // Duplicate the process.
          asmb->addDup(); // Duplicate the process.

          // Write to the stdIn stream of the child process.
          asmb->addLoadLitString("Hello from your parent\n");
          WRITE_STD_IN(asmb);
          asmb->addPop(); // Ignore the success / failure of the write.

          asmb->addDup(); // Duplicate the process.
          asmb->addPCall(novasm::PCallCode::ProcessBlock);
          asmb->addPop(); // Ignore the exit code.

          READ_STD_OUT(asmb);
          ADD_PRINT(asmb);

          asmb->addRet();
        },
        "input",
        "Hello from your parent\n");
  }

  SECTION("Arguments are passed to child process") {
    CHECK_PROG(
        [&](novasm::Assembler* asmb) -> void {
          asmb->label("entry");
          asmb->setEntrypoint("entry");

          // Run a program that prints the number of environment arguments.
          asmb->addLoadLitString(
              novePath + " 'print(intrinsic{env_argument_count}())' a --b -ce hello 'hello world'");
          asmb->addPCall(novasm::PCallCode::ProcessStart);

          asmb->addDup(); // Duplicate the process.
          asmb->addPCall(novasm::PCallCode::ProcessBlock);
          asmb->addPop(); // Ignore the exit code.

          READ_STD_OUT(asmb);
          ADD_PRINT(asmb);

          // Run a program that the argument after --test.
          asmb->addLoadLitString(
              novePath + " 'print(getEnvOpt(\"test\"))' --something --test 'hello world' else");
          asmb->addPCall(novasm::PCallCode::ProcessStart);

          asmb->addDup(); // Duplicate the process.
          asmb->addPCall(novasm::PCallCode::ProcessBlock);
          asmb->addPop(); // Ignore the exit code.

          READ_STD_OUT(asmb);
          ADD_PRINT(asmb);

          asmb->addRet();
        },
        "input",
        "5\ntest[hello world,else]\n");
  }

  SECTION("Process id can be retrieved") {
    CHECK_PROG(
        [&](novasm::Assembler* asmb) -> void {
          asmb->label("entry");
          asmb->setEntrypoint("entry");

          asmb->addLoadLitString(novePath + " 'print(\"Hello world\")'");
          asmb->addPCall(novasm::PCallCode::ProcessStart);
          asmb->addPCall(novasm::PCallCode::ProcessGetId);

          asmb->addLoadLitLong(0);
          asmb->addCheckGtLong();

          ADD_ASSERT(asmb);

          asmb->addRet();
        },
        "input",
        "");
  }

  SECTION("Zombie processes are killed when the vm exists") {
    CHECK_PROG(
        [&](novasm::Assembler* asmb) -> void {
          asmb->label("entry");
          asmb->setEntrypoint("entry");

          asmb->addLoadLitString(novePath + " 'print(\"Hello world\")'");
          asmb->addPCall(novasm::PCallCode::ProcessStart);

          asmb->addPop();
          asmb->addRet();
        },
        "input",
        "");
  }

  SECTION("Process can finish on its own without blocking on it") {
    CHECK_PROG(
        [&](novasm::Assembler* asmb) -> void {
          asmb->label("entry");
          asmb->setEntrypoint("entry");

          asmb->addLoadLitString(novePath + " 'print(\"Hello world\")'");
          asmb->addPCall(novasm::PCallCode::ProcessStart);

          // Sleep for 1 second to give the child-process time to finish.
          asmb->addLoadLitLong(1'000'000'000);
          asmb->addPCall(novasm::PCallCode::SleepNano);

          asmb->addRet();
        },
        "input",
        "");
  }

  SECTION("Process can be waited on concurrently") {
    CHECK_PROG(
        [&](novasm::Assembler* asmb) -> void {
          constexpr uint8_t numForks = 15u;

          asmb->setEntrypoint("entry");
          // --- Main function start.
          asmb->label("entry");
          asmb->addStackAlloc(2 + numForks); // Reserve stack space:
          // Stack var 0:                     process.
          // Stack var 1:                     loop counter integer.
          // Stack var 2 - ( numForks + 2 ):  future handles to the forked workers.

          // Start a process and save it in a variable.
          asmb->addLoadLitString(novePath + " 'print(\"Hello world\")'");
          asmb->addPCall(novasm::PCallCode::ProcessStart);
          asmb->addStackStore(0);

          // Start multiple workers functions as forks that block on the process completion.
          for (auto i = 0u; i != numForks; ++i) {
            asmb->addStackLoad(0); // Pass the process to the worker.
            asmb->addCall("worker", 1, novasm::CallMode::Forked);
            asmb->addStackStore(2u + i); // Store the future to the fork on the stack.
          }

          // Wait for all workers to complete and assert the result.
          for (auto i = 0u; i != numForks; ++i) {
            asmb->addStackLoad(2u + i); // Load the future from the stack.
            asmb->addFutureBlock();

            // Assert that the exitcode is zero.
            asmb->addLoadLitInt(0);
            asmb->addCheckEqInt();
            ADD_ASSERT(asmb);
          }
          asmb->addRet();
          // --- Main function end.

          // --- Worker function start (takes one process arg and returns the exitcode).
          asmb->label("worker");
          asmb->addStackLoad(0); // Load arg 0.
          asmb->addPCall(novasm::PCallCode::ProcessBlock);
          asmb->addRet();
          // --- Worker function end.
        },
        "input",
        "");
  }

  SECTION("Starting a non-existing program results in exitcode -1") {
    CHECK_PROG(
        [&](novasm::Assembler* asmb) -> void {
          asmb->label("entry");
          asmb->setEntrypoint("entry");

          asmb->addLoadLitString("non-existent arg1 arg2");
          asmb->addPCall(novasm::PCallCode::ProcessStart);
          asmb->addPCall(novasm::PCallCode::ProcessBlock);

          // Print the exit-code.
          asmb->addConvIntString();
          ADD_PRINT(asmb);
          asmb->addRet();
        },
        "input",
        "-1");
  }

  SECTION("Empty cmd-line string results in exitcode -1") {
    CHECK_PROG(
        [&](novasm::Assembler* asmb) -> void {
          asmb->label("entry");
          asmb->setEntrypoint("entry");

          asmb->addLoadLitString("");
          asmb->addPCall(novasm::PCallCode::ProcessStart);
          asmb->addPCall(novasm::PCallCode::ProcessBlock);

          // Print the exit-code.
          asmb->addConvIntString();
          ADD_PRINT(asmb);
          asmb->addRet();
        },
        "input",
        "-1");
  }

  SECTION("Whitespace only cmd-line string results in exitcode -1") {
    CHECK_PROG(
        [&](novasm::Assembler* asmb) -> void {
          asmb->label("entry");
          asmb->setEntrypoint("entry");

          asmb->addLoadLitString(" ");
          asmb->addPCall(novasm::PCallCode::ProcessStart);
          asmb->addPCall(novasm::PCallCode::ProcessBlock);

          // Print the exit-code.
          asmb->addConvIntString();
          ADD_PRINT(asmb);
          asmb->addRet();
        },
        "input",
        "-1");
  }

  SECTION("Starting whitespace in the command string is ignored") {
    CHECK_PROG(
        [&](novasm::Assembler* asmb) -> void {
          asmb->label("entry");
          asmb->setEntrypoint("entry");

          asmb->addLoadLitString(" \t \n \r " + novePath + " 'print(\"Hello world\")'");
          asmb->addPCall(novasm::PCallCode::ProcessStart);
          asmb->addPCall(novasm::PCallCode::ProcessBlock);

          asmb->addConvIntString();
          ADD_PRINT(asmb);
          asmb->addRet();
        },
        "input",
        "0");
  }

  SECTION("Single quoted string can be passed to process") {
    CHECK_PROG(
        [&](novasm::Assembler* asmb) -> void {
          asmb->label("entry");
          asmb->setEntrypoint("entry");

          asmb->addLoadLitString(novePath + " 'print(\\'a\\')'");
          asmb->addPCall(novasm::PCallCode::ProcessStart);
          asmb->addPCall(novasm::PCallCode::ProcessBlock);

          asmb->addConvIntString();
          ADD_PRINT(asmb);
          asmb->addRet();
        },
        "input",
        "0");
  }

  SECTION("Newline can be passed to process") {
    CHECK_PROG(
        [&](novasm::Assembler* asmb) -> void {
          asmb->label("entry");
          asmb->setEntrypoint("entry");

          // Run a program that prints to stdOut.
          asmb->addLoadLitString(novePath + " 'print(\"Hello\\nworld\")'");
          asmb->addPCall(novasm::PCallCode::ProcessStart);

          asmb->addDup(); // Duplicate the process.
          asmb->addPCall(novasm::PCallCode::ProcessBlock);
          asmb->addPop(); // Ignore the exit code.

          READ_STD_OUT(asmb);
          ADD_PRINT(asmb);
          asmb->addPop(); // Ignore the success / failure of print.

          asmb->addRet();
        },
        "input",
        "Hello\nworld\n");
  }

  SECTION("Interupt signal can be send to process") {
    CHECK_PROG(
        [&](novasm::Assembler* asmb) -> void {
          asmb->label("entry");
          asmb->setEntrypoint("entry");

          // Run a program that waits until its interupted.
          asmb->addLoadLitString(
              novePath +
              " 'print(invoke( "
              "   impure lambda() "
              "     if interuptIsRequested() -> \"Received interupt\" "
              "     else -> sleep(millisecond()); self() ))'");
          asmb->addPCall(novasm::PCallCode::ProcessStart);
          asmb->addDup(); // Duplicate the process.
          asmb->addDup(); // Duplicate the process.

          // Wait a few seconds to give the process time to start.
          asmb->addLoadLitLong(3'000'000'000);
          asmb->addPCall(novasm::PCallCode::SleepNano);
          asmb->addPop(); // Ignore sleep return value.

          // Send an interupt signal to the process.
          asmb->addLoadLitInt(0); // Signal 0: interupt.
          asmb->addPCall(novasm::PCallCode::ProcessSendSignal);
          ADD_ASSERT(asmb);

          // Wait for the child process to finnish.
          asmb->addPCall(novasm::PCallCode::ProcessBlock);
          asmb->addPop(); // Ignore the exit code.

          READ_STD_OUT(asmb);
          ADD_PRINT(asmb);
          asmb->addPop(); // Ignore the success / failure of print.

          asmb->addRet();
        },
        "",
        "Received interupt\n");
  }

  SECTION("Kill signal can be send to process") {
    CHECK_PROG(
        [&](novasm::Assembler* asmb) -> void {
          asmb->label("entry");
          asmb->setEntrypoint("entry");

          // Run a program that runs an infinite loop.
          asmb->addLoadLitString(
              novePath + " 'invoke(impure lambda() -> bool sleep(millisecond()); self())'");
          asmb->addPCall(novasm::PCallCode::ProcessStart);
          asmb->addDup(); // Duplicate the process.
          asmb->addDup(); // Duplicate the process.

          // Wait a few seconds to give the process time to start.
          asmb->addLoadLitLong(3'000'000'000);
          asmb->addPCall(novasm::PCallCode::SleepNano);
          asmb->addPop(); // Ignore sleep return value.

          // Send a kill signal to the process.
          asmb->addLoadLitInt(1); // Signal 1: kill.
          asmb->addPCall(novasm::PCallCode::ProcessSendSignal);
          ADD_ASSERT(asmb);

          // Wait for the child process to finnish.
          asmb->addPCall(novasm::PCallCode::ProcessBlock);
          asmb->addPop(); // Ignore the exit code.

          asmb->addRet();
        },
        "",
        "");
  }
}

} // namespace vm
