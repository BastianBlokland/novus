#include "catch2/catch.hpp"
#include "helpers.hpp"
#include "vm/exec_state.hpp"

namespace vm {

TEST_CASE("Execute input and output", "[vm]") {

  SECTION("ConWriteChar") {
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitInt('a');
          asmb->addPCall(novasm::PCallCode::ConWriteChar);

          asmb->addLoadLitInt('b');
          asmb->addPCall(novasm::PCallCode::ConWriteChar);

          asmb->addLoadLitInt('c');
          asmb->addPCall(novasm::PCallCode::ConWriteChar);
        },
        "input",
        "a",
        "b",
        "c");
  }

  SECTION("ConWriteString") {
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitString("hello world");
          asmb->addPCall(novasm::PCallCode::ConWriteString);

          asmb->addLoadLitInt(0);
          asmb->addConvBoolString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);

          asmb->addLoadLitInt(1);
          asmb->addConvBoolString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);

          asmb->addLoadLitInt(-42);
          asmb->addConvIntString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
        },
        "input",
        "hello world",
        "false",
        "true",
        "-42");
  }

  SECTION("ConWriteStringLine") {
    CHECK_EXPR(
        [](novasm::Assembler* asmb) -> void {
          asmb->addLoadLitString("hello");
          asmb->addPCall(novasm::PCallCode::ConWriteStringLine);

          asmb->addLoadLitString("world");
          asmb->addPCall(novasm::PCallCode::ConWriteStringLine);
        },
        "input",
        "hello",
        "\n",
        "world",
        "\n");
  }

  SECTION("ConReadChar") {
    CHECK_PROG(
        [](novasm::Assembler* asmb) -> void {
          asmb->label("entry");
          asmb->setEntrypoint("entry");

          asmb->addLoadLitString("hello: ");

          asmb->addPCall(novasm::PCallCode::ConReadChar);
          asmb->addDup();
          asmb->addLogicInvInt();
          asmb->addJumpIf("end");

          asmb->addConvCharString();
          asmb->addAddString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
          asmb->addPop();
          asmb->addJump("entry");

          asmb->label("end");
          asmb->addPop();
          asmb->addRet();
        },
        "abc",
        "hello: a",
        "hello: b",
        "hello: c");
  }

  SECTION("ConReadStringLine") {
    CHECK_PROG(
        [](novasm::Assembler* asmb) -> void {
          asmb->label("entry");
          asmb->setEntrypoint("entry");

          asmb->addLoadLitString("hello: ");

          asmb->addPCall(novasm::PCallCode::ConReadStringLine);
          asmb->addDup();
          asmb->addLengthString();
          asmb->addLogicInvInt();
          asmb->addJumpIf("end");

          asmb->addAddString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
          asmb->addPop();
          asmb->addJump("entry");

          asmb->label("end");
          asmb->addPop();
          asmb->addRet();
        },
        "John Doe\nJane Doe\n",
        "hello: John Doe",
        "hello: Jane Doe");
  }

  SECTION("GetEnvVar") {
    auto envVars =
        std::unordered_map<std::string, std::string>{{"var1", "hello world"}, {"var2", "nope"}};
    CHECK_PROG_WITH_ENV_VARS(
        [](novasm::Assembler* asmb) -> void {
          asmb->label("entry");
          asmb->setEntrypoint("entry");

          asmb->addLoadLitString("var1");
          asmb->addPCall(novasm::PCallCode::GetEnvVar);
          asmb->addPCall(novasm::PCallCode::ConWriteString);
          asmb->addPop();

          asmb->addLoadLitString("var3");
          asmb->addPCall(novasm::PCallCode::GetEnvVar);
          asmb->addPCall(novasm::PCallCode::ConWriteString);

          asmb->addRet();
        },
        "input",
        envVars,
        "hello world",
        "");
  }

  SECTION("GetEnvArg && GetEnvArgCount") {
    auto envArgs = std::vector<std::string>{"a", "b", "c"};
    CHECK_PROG_WITH_ENV_ARGS(
        [](novasm::Assembler* asmb) -> void {
          asmb->label("writeArgCount");
          asmb->addPCall(novasm::PCallCode::GetEnvArgCount);
          asmb->addConvIntString();
          asmb->addPCall(novasm::PCallCode::ConWriteString);
          asmb->addRet();

          asmb->label("writeArg1");
          asmb->addLoadLitInt(0);
          asmb->addPCall(novasm::PCallCode::GetEnvArg);
          asmb->addPCall(novasm::PCallCode::ConWriteString);
          asmb->addRet();

          asmb->label("writeArg3");
          asmb->addLoadLitInt(2);
          asmb->addPCall(novasm::PCallCode::GetEnvArg);
          asmb->addPCall(novasm::PCallCode::ConWriteString);
          asmb->addRet();

          asmb->label("writeArg4");
          asmb->addLoadLitInt(3);
          asmb->addPCall(novasm::PCallCode::GetEnvArg);
          asmb->addPCall(novasm::PCallCode::ConWriteString);
          asmb->addRet();

          // Entrypoint.
          asmb->label("entry");
          asmb->addCall("writeArgCount", 0, novasm::CallMode::Normal);
          asmb->addCall("writeArg1", 0, novasm::CallMode::Normal);
          asmb->addCall("writeArg3", 0, novasm::CallMode::Normal);
          asmb->addCall("writeArg4", 0, novasm::CallMode::Normal);
          asmb->addRet();

          asmb->setEntrypoint("entry");
        },
        "input",
        envArgs,
        "3",
        "a",
        "c",
        "");
  }

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
        "input");
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
        "input");
  }
}

} // namespace vm
