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

  SECTION("Define enum") {
    const auto& output = ANALYZE("enum e = a, b, c");
    REQUIRE(output.isSuccess());
    const auto& typeDef = GET_TYPE_DEF(output, "e");
    REQUIRE(std::holds_alternative<prog::sym::EnumDef>(typeDef));
    const auto& enumDef = std::get<prog::sym::EnumDef>(typeDef);

    CHECK(*enumDef.getValue("a") == 0);
    CHECK(*enumDef.getValue("b") == 1);
    CHECK(*enumDef.getValue("c") == 2);
  }

  SECTION("Define enum with values") {
    const auto& output = ANALYZE("enum e = a : 42, b : -1337, c");
    REQUIRE(output.isSuccess());
    const auto& typeDef = GET_TYPE_DEF(output, "e");
    REQUIRE(std::holds_alternative<prog::sym::EnumDef>(typeDef));
    const auto& enumDef = std::get<prog::sym::EnumDef>(typeDef);

    CHECK(*enumDef.getValue("a") == 42);
    CHECK(*enumDef.getValue("b") == -1337);
    CHECK(*enumDef.getValue("c") == -1336);
  }

  SECTION("Define enum with names of types") {
    const auto& output = ANALYZE("enum e = int, float, bool");
    REQUIRE(output.isSuccess());
    const auto& typeDef = GET_TYPE_DEF(output, "e");
    REQUIRE(std::holds_alternative<prog::sym::EnumDef>(typeDef));
    const auto& enumDef = std::get<prog::sym::EnumDef>(typeDef);

    CHECK(*enumDef.getValue("int") == 0);
    CHECK(*enumDef.getValue("float") == 1);
    CHECK(*enumDef.getValue("bool") == 2);
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
    CHECK_DIAG(
        "struct s = hello a, bool b", errUndeclaredType(src, "hello", 0, input::Span{11, 15}));
    CHECK_DIAG(
        "struct s = int a, bool a", errDuplicateFieldNameInStruct(src, "a", input::Span{23, 23}));
    CHECK_DIAG(
        "struct s = int int", errFieldNameConflictsWithType(src, "int", input::Span{15, 17}));
    CHECK_DIAG(
        "struct s = int function",
        errFieldNameConflictsWithType(src, "function", input::Span{15, 22}));
    CHECK_DIAG(
        "struct s = int action", errFieldNameConflictsWithType(src, "action", input::Span{15, 20}));
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
        errInvalidTypeInstantiation(src, input::Span{30, 30}),
        errUndeclaredType(src, "s", 1, input::Span{30, 35}));
  }

  SECTION("Union diagnostics") {
    CHECK_DIAG("union u = hello, bool", errUndeclaredType(src, "hello", 0, input::Span{10, 14}));
    CHECK_DIAG(
        "union u = int, int", errDuplicateTypeInUnion(src, "int", "int", input::Span{15, 17}));
    CHECK_DIAG(
        "union u{T} = int, T "
        "fun f(u{int} in) -> int 1",
        errDuplicateTypeInUnion(src, "T", "int", input::Span{18, 18}),
        errInvalidTypeInstantiation(src, input::Span{26, 26}),
        errUndeclaredType(src, "u", 1, input::Span{26, 31}));
  }

  SECTION("Enum diagnostics") {
    CHECK_DIAG("enum e = a, b, a", errDuplicateEntryNameInEnum(src, "a", input::Span{15, 15}));
    CHECK_DIAG(
        "enum e = a : 1, b, c : 1", errDuplicateEntryValueInEnum(src, 1, input::Span{19, 23}));
  }
}

} // namespace frontend
