#include "catch2/catch.hpp"
#include "helpers.hpp"

namespace vm {

TEST_CASE("Execute calls", "[vm]") {

  CHECK_PROG(
      [](backend::Builder* builder) -> void {
        builder->label("section1");
        builder->addLoadLitInt(0);
        builder->addCall("section2", false);
        builder->addConvIntString();
        builder->addPCall(vm::PCallCode::Print);
        builder->addPop();
        builder->addRet();

        builder->label("section2");
        builder->addJumpIf("section2-true");
        builder->addCall("section3", false);
        builder->addRet();

        builder->label("section2-true");
        builder->addLoadLitInt(1337);
        builder->addRet();

        builder->label("section3");
        builder->addLoadLitInt(1);
        builder->addCall("section2", false);
        builder->addRet();

        builder->addEntryPoint("section1");
      },
      "1337");

  CHECK_PROG(
      [](backend::Builder* builder) -> void {
        builder->label("section1");
        builder->addLoadLitInt(0);
        builder->addCall("section2", false);
        builder->addConvIntString();
        builder->addPCall(vm::PCallCode::Print);
        builder->addPop();
        builder->addRet();

        builder->label("section2");
        builder->addJumpIf("section2-true");
        builder->addCall("section3", true);

        builder->label("section2-true");
        builder->addLoadLitInt(1337);
        builder->addRet();

        builder->label("section3");
        builder->addLoadLitInt(1);
        builder->addCall("section2", true);

        builder->addEntryPoint("section1");
      },
      "1337");

  CHECK_PROG(
      [](backend::Builder* builder) -> void {
        builder->label("section1");

        // Call raw instruction pointer.
        builder->addLoadLitIp("section2");
        builder->addCallDyn(false);

        builder->addConvIntString();
        builder->addPCall(vm::PCallCode::Print);
        builder->addPop();
        builder->addRet();

        builder->label("section2");
        builder->addLoadLitInt(1337);
        builder->addRet();

        builder->addEntryPoint("section1");
      },
      "1337");

  CHECK_PROG(
      [](backend::Builder* builder) -> void {
        builder->label("section1");

        // Call raw instruction pointer.
        builder->addLoadLitIp("section2");
        builder->addCallDyn(false);

        builder->addConvIntString();
        builder->addPCall(vm::PCallCode::Print);
        builder->addPop();
        builder->addRet();

        builder->label("section2");
        builder->addLoadLitInt(1337);
        builder->addLoadLitIp("section3");
        builder->addCallDyn(true);

        builder->label("section3");
        builder->addLoadLitInt(1337);
        builder->addAddInt();
        builder->addRet();

        builder->addEntryPoint("section1");
      },
      "2674");

  CHECK_PROG(
      [](backend::Builder* builder) -> void {
        builder->label("section1");
        builder->addCall("section2", false);
        builder->addConvIntString();
        builder->addPCall(vm::PCallCode::Print);
        builder->addPop();
        builder->addRet();

        builder->label("section2");

        // Make a closure struct and call it.
        builder->addLoadLitInt(42);
        builder->addLoadLitIp("section3");
        builder->addMakeStruct(2);
        builder->addCallDyn(true);

        builder->label("section3");
        builder->addLoadLitInt(1337);
        builder->addAddInt();
        builder->addRet();

        builder->addEntryPoint("section1");
      },
      "1379");

  CHECK_PROG(
      [](backend::Builder* builder) -> void {
        builder->label("section1");

        // Make a closure struct.
        builder->addLoadLitInt(42);
        builder->addLoadLitIp("section2");
        builder->addMakeStruct(2);

        // Call closure struct.
        builder->addCallDyn(false);

        builder->addConvIntString();
        builder->addPCall(vm::PCallCode::Print);
        builder->addPop();
        builder->addRet();

        builder->label("section2");
        builder->addLoadLitInt(1337);
        builder->addAddInt();
        builder->addRet();

        builder->addEntryPoint("section1");
      },
      "1379");
}

} // namespace vm
