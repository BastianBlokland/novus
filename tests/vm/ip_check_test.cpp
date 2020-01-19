#include "catch2/catch.hpp"
#include "helpers.hpp"

namespace vm {

TEST_CASE("Execute instruction pointer checks", "[vm]") {

  SECTION("Equal") {
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitIp("label");
          builder->addLoadLitIp("label");

          builder->label("label");
          builder->addCheckEqIp();
          builder->addConvBoolString();
          builder->addPCall(vm::PCallCode::PrintString);
          builder->addPop();
        },
        "true");

    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitIp("label1");
          builder->addLoadLitIp("label2");

          builder->label("label1");
          builder->addCheckEqIp();

          builder->label("label2");
          builder->addConvBoolString();
          builder->addPCall(vm::PCallCode::PrintString);
          builder->addPop();
        },
        "false");

    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitIp("label");
          builder->addLoadLitIp("label");

          builder->label("label");
          builder->addCheckEqCallDynTgt();
          builder->addConvBoolString();
          builder->addPCall(vm::PCallCode::PrintString);
          builder->addPop();
        },
        "true");

    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitIp("label1");
          builder->addLoadLitIp("label2");

          builder->label("label1");
          builder->addCheckEqCallDynTgt();

          builder->label("label2");
          builder->addConvBoolString();
          builder->addPCall(vm::PCallCode::PrintString);
          builder->addPop();
        },
        "false");

    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          // Make a closure struct.
          builder->addLoadLitInt(42);
          builder->addLoadLitIp("label");
          builder->addMakeStruct(2);

          // Make a closure struct.
          builder->addLoadLitInt(1337);
          builder->addLoadLitInt(42);
          builder->addLoadLitIp("label");
          builder->addMakeStruct(3);

          builder->label("label");
          builder->addCheckEqCallDynTgt();

          builder->addConvBoolString();
          builder->addPCall(vm::PCallCode::PrintString);
          builder->addPop();
        },
        "true");

    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          // Make a closure struct.
          builder->addLoadLitInt(42);
          builder->addLoadLitIp("label1");
          builder->addMakeStruct(2);

          // Make a closure struct.
          builder->addLoadLitInt(1337);
          builder->addLoadLitInt(42);
          builder->addLoadLitIp("label2");
          builder->addMakeStruct(3);

          builder->label("label1");
          builder->addCheckEqCallDynTgt();

          builder->label("label2");
          builder->addConvBoolString();
          builder->addPCall(vm::PCallCode::PrintString);
          builder->addPop();
        },
        "false");

    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          // Make a closure struct.
          builder->addLoadLitInt(42);
          builder->addLoadLitIp("label");
          builder->addMakeStruct(2);

          // Make a raw instruction pointer.
          builder->addLoadLitIp("label");

          builder->label("label");
          builder->addCheckEqCallDynTgt();

          builder->addConvBoolString();
          builder->addPCall(vm::PCallCode::PrintString);
          builder->addPop();
        },
        "true");

    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          // Make a closure struct.
          builder->addLoadLitInt(42);
          builder->addLoadLitIp("label1");
          builder->addMakeStruct(2);

          // Make a raw instruction pointer.
          builder->addLoadLitIp("label2");

          builder->label("label1");
          builder->addCheckEqCallDynTgt();

          builder->label("label2");
          builder->addConvBoolString();
          builder->addPCall(vm::PCallCode::PrintString);
          builder->addPop();
        },
        "false");
  }
}

} // namespace vm
