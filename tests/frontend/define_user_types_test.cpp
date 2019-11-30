#include "catch2/catch.hpp"
#include "frontend/diag_defs.hpp"
#include "helpers.hpp"

namespace frontend {

TEST_CASE("Analyzing user-type definitions", "[frontend]") {

  SECTION("Define struct") {
    const auto& output = ANALYZE("struct s = int a, bool b");
    REQUIRE(output.isSuccess());
    const auto& typeDef = GET_TYPE_DEF(output, "s");
    REQUIRE(std::holds_alternative<prog::sym::StructDef>(typeDef));
    const auto& structDef = std::get<prog::sym::StructDef>(typeDef);

    REQUIRE(structDef.getCount() == 2);
    auto expectedFields = std::vector<prog::sym::StructDef::Field>{};
    expectedFields.emplace_back(GET_TYPE_ID(output, "int"), "a");
    expectedFields.emplace_back(GET_TYPE_ID(output, "bool"), "b");
    CHECK(std::equal(structDef.begin(), structDef.end(), expectedFields.begin()));
  }

  SECTION("Diagnostics") {
    CHECK_DIAG("struct s = hello a, bool b", errUndeclaredType(src, "hello", input::Span{11, 15}));
    CHECK_DIAG(
        "struct s = int a, bool a", errDuplicateFieldNameInStruct(src, "a", input::Span{23, 23}));
    CHECK_DIAG(
        "struct s = int int", errFieldNameConflictsWithType(src, "int", input::Span{15, 17}));
  }
}

} // namespace frontend
