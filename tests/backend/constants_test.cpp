#include "catch2/catch.hpp"
#include "helpers.hpp"

namespace backend {

TEST_CASE("Generate assembly for storing and loading constants", "[backend]") {

  CHECK_EXPR_INT("x = 42; x", [](backend::Builder* builder) -> void {
    builder->addReserveConsts(1);

    builder->addLoadLitInt(42);
    builder->addDup();
    builder->addStoreConst(0);

    builder->addPop();
    builder->addLoadConst(0);
  });

  CHECK_EXPR_INT("x = y = 42; y", [](backend::Builder* builder) -> void {
    builder->addReserveConsts(2);

    builder->addLoadLitInt(42);
    builder->addDup();
    builder->addStoreConst(0);

    builder->addDup();
    builder->addStoreConst(1);

    builder->addPop();
    builder->addLoadConst(0);
  });

  CHECK_EXPR_INT("x = 42; y = 1337; x + y", [](backend::Builder* builder) -> void {
    builder->addReserveConsts(2);

    builder->addLoadLitInt(42);
    builder->addDup();
    builder->addStoreConst(0);

    builder->addPop();

    builder->addLoadLitInt(1337);
    builder->addDup();
    builder->addStoreConst(1);

    builder->addPop();

    builder->addLoadConst(0);
    builder->addLoadConst(1);
    builder->addAddInt();
  });

  CHECK_EXPR_INT("x = y = 42; y", [](backend::Builder* builder) -> void {
    builder->addReserveConsts(2);

    builder->addLoadLitInt(42);
    builder->addDup();
    builder->addStoreConst(0);

    builder->addDup();
    builder->addStoreConst(1);

    builder->addPop();
    builder->addLoadConst(0);
  });
}

} // namespace backend
