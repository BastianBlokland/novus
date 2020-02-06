#include "catch2/catch.hpp"
#include "helpers.hpp"

namespace vm {

TEST_CASE("Execute conversions", "[vm]") {

  SECTION("Int to Float") {
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitInt(42);
          builder->addConvIntFloat();

          builder->addConvFloatString();
          builder->addPCall(vm::PCallCode::ConWriteString);
        },
        "input",
        "42");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitInt(-2147483647);
          builder->addConvIntFloat();

          builder->addConvFloatString();
          builder->addPCall(vm::PCallCode::ConWriteString);
        },
        "input",
        "-2.14748e+09");
  }

  SECTION("Float to Int") {
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitFloat(42.1F); // NOLINT: Magic numbers
          builder->addConvFloatInt();

          builder->addConvIntString();
          builder->addPCall(vm::PCallCode::ConWriteString);
        },
        "input",
        "42");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitFloat(42.9F); // NOLINT: Magic numbers
          builder->addConvFloatInt();

          builder->addConvIntString();
          builder->addPCall(vm::PCallCode::ConWriteString);
        },
        "input",
        "42");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitFloat(0.001F); // NOLINT: Magic numbers
          builder->addConvFloatInt();

          builder->addConvIntString();
          builder->addPCall(vm::PCallCode::ConWriteString);
        },
        "input",
        "0");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitFloat(2147483648.0F); // NOLINT: Magic numbers
          builder->addConvFloatInt();

          builder->addConvIntString();
          builder->addPCall(vm::PCallCode::ConWriteString);
        },
        "input",
        "-2147483648"); // Unrepresentable by int, high bit 1 all others 0.
  }

  SECTION("Int to String") {
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitInt(0);
          builder->addConvIntString();
          builder->addPCall(vm::PCallCode::ConWriteString);
        },
        "input",
        "0");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitInt(42);
          builder->addConvIntString();
          builder->addPCall(vm::PCallCode::ConWriteString);
        },
        "input",
        "42");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitInt(2147483647);
          builder->addConvIntString();
          builder->addPCall(vm::PCallCode::ConWriteString);
        },
        "input",
        "2147483647");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitInt(-2147483647);
          builder->addConvIntString();
          builder->addPCall(vm::PCallCode::ConWriteString);
        },
        "input",
        "-2147483647");
  }

  SECTION("Float to String") {
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitFloat(0.0F);
          builder->addConvFloatString();
          builder->addPCall(vm::PCallCode::ConWriteString);
        },
        "input",
        "0");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitFloat(42.0F); // NOLINT: Magic numbers
          builder->addConvFloatString();
          builder->addPCall(vm::PCallCode::ConWriteString);
        },
        "input",
        "42");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitFloat(42.1337F); // NOLINT: Magic numbers
          builder->addConvFloatString();
          builder->addPCall(vm::PCallCode::ConWriteString);
        },
        "input",
        "42.1337");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitFloat(0.42F); // NOLINT: Magic numbers
          builder->addConvFloatString();
          builder->addPCall(vm::PCallCode::ConWriteString);
        },
        "input",
        "0.42");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitFloat(999999.0F); // NOLINT: Magic numbers
          builder->addConvFloatString();
          builder->addPCall(vm::PCallCode::ConWriteString);
        },
        "input",
        "999999");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitFloat(99999.1F); // NOLINT: Magic numbers
          builder->addConvFloatString();
          builder->addPCall(vm::PCallCode::ConWriteString);
        },
        "input",
        "99999.1");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitFloat(0.000001F); // NOLINT: Magic numbers
          builder->addConvFloatString();
          builder->addPCall(vm::PCallCode::ConWriteString);
        },
        "input",
        "1e-06");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitFloat(9999999999.0F); // NOLINT: Magic numbers
          builder->addConvFloatString();
          builder->addPCall(vm::PCallCode::ConWriteString);
        },
        "input",
        "1e+10"); // Rounding error.
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitFloat(-0.42F); // NOLINT: Magic numbers
          builder->addConvFloatString();
          builder->addPCall(vm::PCallCode::ConWriteString);
        },
        "input",
        "-0.42");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitFloat(-9999999999.0F); // NOLINT: Magic numbers
          builder->addConvFloatString();
          builder->addPCall(vm::PCallCode::ConWriteString);
        },
        "input",
        "-1e+10"); // Rounding error.
  }

  SECTION("Char to String") {
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitInt(48);
          builder->addConvCharString();
          builder->addPCall(vm::PCallCode::ConWriteString);
        },
        "input",
        "0");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitInt(10);
          builder->addConvCharString();
          builder->addPCall(vm::PCallCode::ConWriteString);
        },
        "input",
        "\n");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitInt(38);
          builder->addConvCharString();
          builder->addPCall(vm::PCallCode::ConWriteString);
        },
        "input",
        "&");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitInt(32);
          builder->addConvCharString();
          builder->addPCall(vm::PCallCode::ConWriteString);
        },
        "input",
        " ");
  }

  SECTION("Int to Char") {
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitInt(0);
          builder->addConvIntChar();

          builder->addConvIntString();
          builder->addPCall(vm::PCallCode::ConWriteString);
        },
        "input",
        "0");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitInt(42);
          builder->addConvIntChar();

          builder->addConvIntString();
          builder->addPCall(vm::PCallCode::ConWriteString);
        },
        "input",
        "42");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitInt(255);
          builder->addConvIntChar();

          builder->addConvIntString();
          builder->addPCall(vm::PCallCode::ConWriteString);
        },
        "input",
        "255");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitInt(256);
          builder->addConvIntChar();

          builder->addConvIntString();
          builder->addPCall(vm::PCallCode::ConWriteString);
        },
        "input",
        "0");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitInt(-1);
          builder->addConvIntChar();

          builder->addConvIntString();
          builder->addPCall(vm::PCallCode::ConWriteString);
        },
        "input",
        "255");
    CHECK_EXPR(
        [](backend::Builder* builder) -> void {
          builder->addLoadLitInt(1337);
          builder->addConvIntChar();

          builder->addConvIntString();
          builder->addPCall(vm::PCallCode::ConWriteString);
        },
        "input",
        "57");
  }
}

} // namespace vm
