#include "catch2/catch.hpp"
#include "helpers.hpp"

namespace backend {

TEST_CASE("Generating assembly for group expressions", "[backend]") {

  CHECK_EXPR_INT("x = 42; x", [](backend::Builder* builder) -> void {
    builder->addReserveConsts(1);

    builder->addLoadLitInt(42);
    builder->addDup();
    builder->addStoreConst(0);

    builder->addPop();
    builder->addLoadConst(0);
  });

  CHECK_EXPR_INT("1; 2; 3", [](backend::Builder* builder) -> void {
    builder->addLoadLitInt(1);
    builder->addPop();

    builder->addLoadLitInt(2);
    builder->addPop();

    builder->addLoadLitInt(3);
  });
}

} // namespace backend
