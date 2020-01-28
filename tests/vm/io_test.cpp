#include "catch2/catch.hpp"
#include "helpers.hpp"
#include "vm/exceptions/assert_failed.hpp"

namespace vm {

TEST_CASE("Execute input and output", "[vm]") {

  SECTION("PrintChar") {
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitInt('a');
          builder->addPCall(vm::PCallCode::PrintChar);
          builder->addPop();

          builder->addLoadLitInt('b');
          builder->addPCall(vm::PCallCode::PrintChar);
          builder->addPop();

          builder->addLoadLitInt('c');
          builder->addPCall(vm::PCallCode::PrintChar);
          builder->addPop();
        },
        "input",
        "a",
        "b",
        "c");
  }

  SECTION("PrintString") {
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitString("hello world");
          builder->addPCall(vm::PCallCode::PrintString);
          builder->addPop();

          builder->addLoadLitInt(0);
          builder->addConvBoolString();
          builder->addPCall(vm::PCallCode::PrintString);
          builder->addPop();

          builder->addLoadLitInt(1);
          builder->addConvBoolString();
          builder->addPCall(vm::PCallCode::PrintString);
          builder->addPop();

          builder->addLoadLitInt(-42);
          builder->addConvIntString();
          builder->addPCall(vm::PCallCode::PrintString);
          builder->addPop();
        },
        "input",
        "hello world",
        "false",
        "true",
        "-42");
  }

  SECTION("PrintStringLine") {
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitString("hello");
          builder->addPCall(vm::PCallCode::PrintStringLine);
          builder->addPop();

          builder->addLoadLitString("world");
          builder->addPCall(vm::PCallCode::PrintStringLine);
          builder->addPop();
        },
        "input",
        "hello",
        "\n",
        "world",
        "\n");
  }

  SECTION("Read char") {
    CHECK_PROG(
        [](backend::Builder* builder) -> void {
          builder->label("entry");
          builder->addEntryPoint("entry");

          builder->addLoadLitString("hello: ");

          builder->addPCall(vm::PCallCode::ReadChar);
          builder->addDup();
          builder->addLogicInvInt();
          builder->addJumpIf("end");

          builder->addConvCharString();
          builder->addAddString();
          builder->addPCall(vm::PCallCode::PrintString);
          builder->addPop();
          builder->addJump("entry");

          builder->label("end");
          builder->addPop();
          builder->addPop();
          builder->addRet();
        },
        "abc",
        "hello: a",
        "hello: b",
        "hello: c");
  }

  SECTION("Read line") {
    CHECK_PROG(
        [](backend::Builder* builder) -> void {
          builder->label("entry");
          builder->addEntryPoint("entry");

          builder->addLoadLitString("hello: ");

          builder->addPCall(vm::PCallCode::ReadStringLine);
          builder->addDup();
          builder->addLengthString();
          builder->addLogicInvInt();
          builder->addJumpIf("end");

          builder->addAddString();
          builder->addPCall(vm::PCallCode::PrintString);
          builder->addPop();
          builder->addJump("entry");

          builder->label("end");
          builder->addPop();
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
          builder->addEntryPoint("entry");

          builder->addLoadLitString("var1");
          builder->addPCall(vm::PCallCode::GetEnvVar);
          builder->addPCall(vm::PCallCode::PrintString);
          builder->addPop();

          builder->addLoadLitString("var3");
          builder->addPCall(vm::PCallCode::GetEnvVar);
          builder->addPCall(vm::PCallCode::PrintString);
          builder->addPop();

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
          builder->label("printArgCount");
          builder->addPCall(vm::PCallCode::GetEnvArgCount);
          builder->addConvIntString();
          builder->addPCall(vm::PCallCode::PrintString);
          builder->addPop();
          builder->addRet();

          builder->label("printArg1");
          builder->addLoadLitInt(0);
          builder->addPCall(vm::PCallCode::GetEnvArg);
          builder->addPCall(vm::PCallCode::PrintString);
          builder->addPop();
          builder->addRet();

          builder->label("printArg3");
          builder->addLoadLitInt(2);
          builder->addPCall(vm::PCallCode::GetEnvArg);
          builder->addPCall(vm::PCallCode::PrintString);
          builder->addPop();
          builder->addRet();

          builder->label("printArg4");
          builder->addLoadLitInt(3);
          builder->addPCall(vm::PCallCode::GetEnvArg);
          builder->addPCall(vm::PCallCode::PrintString);
          builder->addPop();
          builder->addRet();

          builder->addEntryPoint("printArgCount");
          builder->addEntryPoint("printArg1");
          builder->addEntryPoint("printArg3");
          builder->addEntryPoint("printArg4");
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
          builder->addEntryPoint("entry");

          // Its hard to test sleep, but at least this tests if the application exits cleanly.
          builder->addLoadLitInt(0); // Sleep for 0 milliseconds.
          builder->addPCall(vm::PCallCode::Sleep);
          builder->addPop();

          builder->addRet();
        },
        "input");
  }

  SECTION("Assert") {
    CHECK_PROG_THROWS(
        [](backend::Builder* builder) -> void {
          builder->label("entry");
          builder->addEntryPoint("entry");

          builder->addLoadLitInt(0);
          builder->addLoadLitString("Fails");
          builder->addPCall(vm::PCallCode::Assert);
          builder->addPop();

          builder->addRet();
        },
        "input",
        vm::exceptions::AssertFailed);
    CHECK_PROG(
        [](backend::Builder* builder) -> void {
          builder->label("entry");
          builder->addEntryPoint("entry");

          builder->addLoadLitInt(1);
          builder->addLoadLitString("Does not fail");
          builder->addPCall(vm::PCallCode::Assert);
          builder->addPop();

          builder->addRet();
        },
        "input");
  }
}

} // namespace vm
