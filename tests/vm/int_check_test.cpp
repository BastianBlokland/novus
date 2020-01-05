#include "catch2/catch.hpp"
#include "helpers.hpp"

namespace vm {

TEST_CASE("Execute integer checks", "[vm]") {

  SECTION("Equal") {
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitInt(1);
          builder->addLoadLitInt(-2);
          builder->addCheckEqInt();
          builder->addConvBoolString();
          builder->addPrintString();
        },
        "false");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitInt(2);
          builder->addLoadLitInt(2);
          builder->addCheckEqInt();
          builder->addConvBoolString();
          builder->addPrintString();
        },
        "true");
  }

  SECTION("Greater") {
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitInt(-1);
          builder->addLoadLitInt(2);
          builder->addCheckGtInt();
          builder->addConvBoolString();
          builder->addPrintString();
        },
        "false");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitInt(2);
          builder->addLoadLitInt(1);
          builder->addCheckGtInt();
          builder->addConvBoolString();
          builder->addPrintString();
        },
        "true");
  }

  SECTION("Less") {
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitInt(3);
          builder->addLoadLitInt(2);
          builder->addCheckLeInt();
          builder->addConvBoolString();
          builder->addPrintString();
        },
        "false");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitInt(2);
          builder->addLoadLitInt(3);
          builder->addCheckLeInt();
          builder->addConvBoolString();
          builder->addPrintString();
        },
        "true");
  }
}

} // namespace vm
