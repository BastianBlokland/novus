#include "catch2/catch.hpp"
#include "helpers.hpp"
#include "vm/exec_state.hpp"

namespace vm {

TEST_CASE("Execute input and output", "[vm]") {

  SECTION("ConWriteChar") {
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitInt('a');
          builder->addPCall(vm::PCallCode::ConWriteChar);

          builder->addLoadLitInt('b');
          builder->addPCall(vm::PCallCode::ConWriteChar);

          builder->addLoadLitInt('c');
          builder->addPCall(vm::PCallCode::ConWriteChar);
        },
        "input",
        "a",
        "b",
        "c");
  }

  SECTION("ConWriteString") {
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitString("hello world");
          builder->addPCall(vm::PCallCode::ConWriteString);

          builder->addLoadLitInt(0);
          builder->addConvBoolString();
          builder->addPCall(vm::PCallCode::ConWriteString);

          builder->addLoadLitInt(1);
          builder->addConvBoolString();
          builder->addPCall(vm::PCallCode::ConWriteString);

          builder->addLoadLitInt(-42);
          builder->addConvIntString();
          builder->addPCall(vm::PCallCode::ConWriteString);
        },
        "input",
        "hello world",
        "false",
        "true",
        "-42");
  }

  SECTION("ConWriteStringLine") {
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitString("hello");
          builder->addPCall(vm::PCallCode::ConWriteStringLine);

          builder->addLoadLitString("world");
          builder->addPCall(vm::PCallCode::ConWriteStringLine);
        },
        "input",
        "hello",
        "\n",
        "world",
        "\n");
  }

  SECTION("ConReadChar") {
    CHECK_PROG(
        [](backend::Builder* builder) -> void {
          builder->label("entry");
          builder->setEntrypoint("entry");

          builder->addLoadLitString("hello: ");

          builder->addPCall(vm::PCallCode::ConReadChar);
          builder->addDup();
          builder->addLogicInvInt();
          builder->addJumpIf("end");

          builder->addConvCharString();
          builder->addAddString();
          builder->addPCall(vm::PCallCode::ConWriteString);
          builder->addPop();
          builder->addJump("entry");

          builder->label("end");
          builder->addPop();
          builder->addRet();
        },
        "abc",
        "hello: a",
        "hello: b",
        "hello: c");
  }

  SECTION("ConReadStringLine") {
    CHECK_PROG(
        [](backend::Builder* builder) -> void {
          builder->label("entry");
          builder->setEntrypoint("entry");

          builder->addLoadLitString("hello: ");

          builder->addPCall(vm::PCallCode::ConReadStringLine);
          builder->addDup();
          builder->addLengthString();
          builder->addLogicInvInt();
          builder->addJumpIf("end");

          builder->addAddString();
          builder->addPCall(vm::PCallCode::ConWriteString);
          builder->addPop();
          builder->addJump("entry");

          builder->label("end");
          builder->addPop();
          builder->addRet();
        },
        "John Doe\nJane Doe\n",
        "hello: John Doe",
        "hello: Jane Doe");
  }

  SECTION("GetEnvVar") {
    auto envVars =
        std::unordered_map<std::string, std::string>{{"var1", "hello world"}, {"var2", "nope"}};
    CHECK_PROG_WITH_ENV_VARS(
        [](backend::Builder* builder) -> void {
          builder->label("entry");
          builder->setEntrypoint("entry");

          builder->addLoadLitString("var1");
          builder->addPCall(vm::PCallCode::GetEnvVar);
          builder->addPCall(vm::PCallCode::ConWriteString);
          builder->addPop();

          builder->addLoadLitString("var3");
          builder->addPCall(vm::PCallCode::GetEnvVar);
          builder->addPCall(vm::PCallCode::ConWriteString);

          builder->addRet();
        },
        "input",
        envVars,
        "hello world",
        "");
  }

  SECTION("GetEnvArg && GetEnvArgCount") {
    auto envArgs = std::vector<std::string>{"a", "b", "c"};
    CHECK_PROG_WITH_ENV_ARGS(
        [](backend::Builder* builder) -> void {
          builder->label("writeArgCount");
          builder->addPCall(vm::PCallCode::GetEnvArgCount);
          builder->addConvIntString();
          builder->addPCall(vm::PCallCode::ConWriteString);
          builder->addRet();

          builder->label("writeArg1");
          builder->addLoadLitInt(0);
          builder->addPCall(vm::PCallCode::GetEnvArg);
          builder->addPCall(vm::PCallCode::ConWriteString);
          builder->addRet();

          builder->label("writeArg3");
          builder->addLoadLitInt(2);
          builder->addPCall(vm::PCallCode::GetEnvArg);
          builder->addPCall(vm::PCallCode::ConWriteString);
          builder->addRet();

          builder->label("writeArg4");
          builder->addLoadLitInt(3);
          builder->addPCall(vm::PCallCode::GetEnvArg);
          builder->addPCall(vm::PCallCode::ConWriteString);
          builder->addRet();

          // Entrypoint.
          builder->label("entry");
          builder->addCall("writeArgCount", 0, backend::CallMode::Normal);
          builder->addCall("writeArg1", 0, backend::CallMode::Normal);
          builder->addCall("writeArg3", 0, backend::CallMode::Normal);
          builder->addCall("writeArg4", 0, backend::CallMode::Normal);
          builder->addRet();

          builder->setEntrypoint("entry");
        },
        "input",
        envArgs,
        "3",
        "a",
        "c",
        "");
  }

  SECTION("Sleep") {
    CHECK_PROG(
        [](backend::Builder* builder) -> void {
          builder->label("entry");
          builder->setEntrypoint("entry");

          // Its hard to test sleep, but at least this tests if the application exits cleanly.
          builder->addLoadLitInt(0); // Sleep for 0 milliseconds.
          builder->addPCall(vm::PCallCode::Sleep);

          builder->addRet();
        },
        "input");
  }

  SECTION("Assert") {
    CHECK_PROG_RESULTCODE(
        [](backend::Builder* builder) -> void {
          builder->label("entry");
          builder->setEntrypoint("entry");

          builder->addLoadLitInt(0);
          builder->addLoadLitString("Fails");
          builder->addPCall(vm::PCallCode::Assert);

          builder->addRet();
        },
        "input",
        ExecState::AssertFailed);
    CHECK_PROG(
        [](backend::Builder* builder) -> void {
          builder->label("entry");
          builder->setEntrypoint("entry");

          builder->addLoadLitInt(1);
          builder->addLoadLitString("Does not fail");
          builder->addPCall(vm::PCallCode::Assert);

          builder->addRet();
        },
        "input");
  }
}

} // namespace vm
