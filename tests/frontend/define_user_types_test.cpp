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
    const auto& structDef    = std::get<prog::sym::StructDef>(typeDef);
    const auto& structFields = structDef.getFields();

    REQUIRE(structFields.getCount() == 2);
    CHECK(structFields[*structFields.lookup("a")].getType() == GET_TYPE_ID(output, "int"));
    CHECK(structFields[*structFields.lookup("b")].getType() == GET_TYPE_ID(output, "bool"));
  }

  SECTION("Define templated struct") {
    const auto& output = ANALYZE("struct s1{T, Y} = T a, Y b "
                                 "struct s2 = s1{float, int} s");
    REQUIRE(output.isSuccess());
    const auto& typeDef = GET_TYPE_DEF(output, "s1__float_int");
    REQUIRE(std::holds_alternative<prog::sym::StructDef>(typeDef));
    const auto& structDef    = std::get<prog::sym::StructDef>(typeDef);
    const auto& structFields = structDef.getFields();

    REQUIRE(structFields.getCount() == 2);
    CHECK(structFields[*structFields.lookup("a")].getType() == GET_TYPE_ID(output, "float"));
    CHECK(structFields[*structFields.lookup("b")].getType() == GET_TYPE_ID(output, "int"));
  }

  SECTION("Define union") {
    const auto& output = ANALYZE("union u = int, bool");
    REQUIRE(output.isSuccess());
    const auto& typeDef = GET_TYPE_DEF(output, "u");
    REQUIRE(std::holds_alternative<prog::sym::UnionDef>(typeDef));
    const auto& unionDef = std::get<prog::sym::UnionDef>(typeDef);

    CHECK(
        unionDef.getTypes() ==
        std::vector<prog::sym::TypeId>{GET_TYPE_ID(output, "int"), GET_TYPE_ID(output, "bool")});
    CHECK(unionDef.hasType(GET_TYPE_ID(output, "int")));
    CHECK(unionDef.hasType(GET_TYPE_ID(output, "bool")));
  }

  SECTION("Define templated union") {
    const auto& output = ANALYZE("union u{T} = int, T "
                                 "struct s = u{float} a");
    REQUIRE(output.isSuccess());
    const auto& typeDef = GET_TYPE_DEF(output, "u__float");
    REQUIRE(std::holds_alternative<prog::sym::UnionDef>(typeDef));
    const auto& unionDef = std::get<prog::sym::UnionDef>(typeDef);

    CHECK(
        unionDef.getTypes() ==
        std::vector<prog::sym::TypeId>{GET_TYPE_ID(output, "int"), GET_TYPE_ID(output, "float")});
    CHECK(unionDef.hasType(GET_TYPE_ID(output, "int")));
    CHECK(unionDef.hasType(GET_TYPE_ID(output, "float")));
  }

  SECTION("Struct diagnostics") {
    CHECK_DIAG("struct s = hello a, bool b", errUndeclaredType(src, "hello", input::Span{11, 15}));
    CHECK_DIAG(
        "struct s = int a, bool a", errDuplicateFieldNameInStruct(src, "a", input::Span{23, 23}));
    CHECK_DIAG(
        "struct s = int int", errFieldNameConflictsWithType(src, "int", input::Span{15, 17}));
    CHECK_DIAG("struct s = s i", errCyclicStruct(src, "i", "s", input::Span{0, 13}));
    CHECK_DIAG(
        "struct s1 = s2 a "
        "struct s2 = s1 b",
        errCyclicStruct(src, "a", "s1", input::Span{0, 15}),
        errCyclicStruct(src, "b", "s2", input::Span{17, 32}));
    CHECK_DIAG(
        "struct s1 = s2 a "
        "struct s2 = int a, s1 b "
        "struct s3 = s1 a, s2 b",
        errCyclicStruct(src, "a", "s1", input::Span{0, 15}),
        errCyclicStruct(src, "b", "s2", input::Span{17, 39}),
        errCyclicStruct(src, "a", "s3", input::Span{41, 62}));
    CHECK_DIAG(
        "struct s{T} = T T "
        "struct s2 = s{int} s",
        errFieldNameConflictsWithTypeSubstitution(src, "T", input::Span{16, 16}),
        errInvalidTypeInstantiation(src, input::Span{30, 35}));
  }

  SECTION("Union diagnostics") {
    CHECK_DIAG("union u = hello, bool", errUndeclaredType(src, "hello", input::Span{10, 14}));
    CHECK_DIAG("union u = int, int", errDuplicateTypeInUnion(src, "int", input::Span{15, 17}));
  }
}

} // namespace frontend
