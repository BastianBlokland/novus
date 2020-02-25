#include "catch2/catch.hpp"
#include "helpers.hpp"

namespace vm {

TEST_CASE("Execute calls", "[vm]") {

  CHECK_PROG(
      [](novasm::Assembler* asmb) -> void {
        asmb->label("section1");
        asmb->addLoadLitInt(0);
        asmb->addCall("section2", 1, novasm::CallMode::Normal);
        asmb->addConvIntString();
        asmb->addPCall(novasm::PCallCode::ConWriteString);
        asmb->addRet();

        asmb->label("section2");
        asmb->addJumpIf("section2-true");
        asmb->addCall("section3", 0, novasm::CallMode::Normal);
        asmb->addRet();

        asmb->label("section2-true");
        asmb->addLoadLitInt(1337);
        asmb->addRet();

        asmb->label("section3");
        asmb->addLoadLitInt(1);
        asmb->addCall("section2", 1, novasm::CallMode::Normal);
        asmb->addRet();

        asmb->setEntrypoint("section1");
      },
      "input",
      "1337");

  CHECK_PROG(
      [](novasm::Assembler* asmb) -> void {
        asmb->label("section1");
        asmb->addLoadLitInt(0);
        asmb->addCall("section2", 1, novasm::CallMode::Normal);
        asmb->addConvIntString();
        asmb->addPCall(novasm::PCallCode::ConWriteString);
        asmb->addRet();

        asmb->label("section2");
        asmb->addJumpIf("section2-true");
        asmb->addCall("section3", 0, novasm::CallMode::Tail);

        asmb->label("section2-true");
        asmb->addLoadLitInt(1337);
        asmb->addRet();

        asmb->label("section3");
        asmb->addLoadLitInt(1);
        asmb->addCall("section2", 1, novasm::CallMode::Tail);

        asmb->setEntrypoint("section1");
      },
      "input",
      "1337");

  CHECK_PROG(
      [](novasm::Assembler* asmb) -> void {
        asmb->label("section1");

        // Call raw instruction pointer.
        asmb->addLoadLitIp("section2");
        asmb->addCallDyn(0, novasm::CallMode::Normal);

        asmb->addConvIntString();
        asmb->addPCall(novasm::PCallCode::ConWriteString);
        asmb->addRet();

        asmb->label("section2");
        asmb->addLoadLitInt(1337);
        asmb->addRet();

        asmb->setEntrypoint("section1");
      },
      "input",
      "1337");

  CHECK_PROG(
      [](novasm::Assembler* asmb) -> void {
        asmb->label("section1");

        // Call raw instruction pointer.
        asmb->addLoadLitIp("section2");
        asmb->addCallDyn(0, novasm::CallMode::Normal);

        asmb->addConvIntString();
        asmb->addPCall(novasm::PCallCode::ConWriteString);
        asmb->addRet();

        asmb->label("section2");
        asmb->addLoadLitInt(1337);
        asmb->addLoadLitIp("section3");
        asmb->addCallDyn(1, novasm::CallMode::Tail);

        asmb->label("section3");
        asmb->addLoadLitInt(1337);
        asmb->addAddInt();
        asmb->addRet();

        asmb->setEntrypoint("section1");
      },
      "input",
      "2674");

  CHECK_PROG(
      [](novasm::Assembler* asmb) -> void {
        asmb->label("section1");
        asmb->addCall("section2", 0, novasm::CallMode::Normal);
        asmb->addConvIntString();
        asmb->addPCall(novasm::PCallCode::ConWriteString);
        asmb->addRet();

        asmb->label("section2");

        // Make a closure struct and call it.
        asmb->addLoadLitInt(42);
        asmb->addLoadLitIp("section3");
        asmb->addMakeStruct(2);
        asmb->addCallDyn(0, novasm::CallMode::Tail);

        asmb->label("section3");
        asmb->addLoadLitInt(1337);
        asmb->addAddInt();
        asmb->addRet();

        asmb->setEntrypoint("section1");
      },
      "input",
      "1379");

  CHECK_PROG(
      [](novasm::Assembler* asmb) -> void {
        asmb->label("section1");

        // Make a closure struct.
        asmb->addLoadLitInt(42);
        asmb->addLoadLitIp("section2");
        asmb->addMakeStruct(2);

        // Call closure struct.
        asmb->addCallDyn(0, novasm::CallMode::Normal);

        asmb->addConvIntString();
        asmb->addPCall(novasm::PCallCode::ConWriteString);
        asmb->addRet();

        asmb->label("section2");
        asmb->addLoadLitInt(1337);
        asmb->addAddInt();
        asmb->addRet();

        asmb->setEntrypoint("section1");
      },
      "input",
      "1379");
}

} // namespace vm
