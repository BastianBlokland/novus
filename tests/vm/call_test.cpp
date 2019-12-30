#include "catch2/catch.hpp"
#include "helpers.hpp"

namespace vm {

TEST_CASE("Execute calls", "[vm]") {

  CHECK_PROG(
      [](backend::Builder* builder) -> void {
        builder->label("section1");
        builder->addLoadLitInt(0);
        builder->addCall("section2");
        builder->addConvIntString();
        builder->addPrintString();
        builder->addRet();

        builder->label("section2");
        builder->addJumpIf("section2-true");
        builder->addCall("section3");
        builder->addRet();

        builder->label("section2-true");
        builder->addLoadLitInt(1337);
        builder->addRet();

        builder->label("section3");
        builder->addLoadLitInt(1);
        builder->addCall("section2");
        builder->addRet();

        builder->addEntryPoint("section1");
      },
      "1337");

  CHECK_PROG(
      [](backend::Builder* builder) -> void {
        builder->label("section1");
        builder->addLoadLitIp("section2");
        builder->addCallDyn();
        builder->addConvIntString();
        builder->addPrintString();
        builder->addRet();

        builder->label("section2");
        builder->addLoadLitInt(1337);
        builder->addRet();

        builder->addEntryPoint("section1");
      },
      "1337");
}

} // namespace vm
