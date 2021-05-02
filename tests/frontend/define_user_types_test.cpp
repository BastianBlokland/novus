#include "catch2/catch.hpp"
#include "frontend/diag_defs.hpp"
#include "helpers.hpp"

namespace frontend {

TEST_CASE("[frontend] Analyzing user-type definitions", "frontend") {

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
    CHECK_DIAG("struct s = hello a, bool b", errUndeclaredType(NO_SRC, "hello", 0));
    CHECK_DIAG("struct s = int a, bool a", errDuplicateFieldNameInStruct(NO_SRC, "a"));
    CHECK_DIAG("struct s = int int", errFieldNameConflictsWithType(NO_SRC, "int"));
    CHECK_DIAG("struct s = int function", errFieldNameConflictsWithType(NO_SRC, "function"));
    CHECK_DIAG("struct s = int action", errFieldNameConflictsWithType(NO_SRC, "action"));
    CHECK_DIAG("struct s = s i", errCyclicStruct(NO_SRC, "i", "s"));
    CHECK_DIAG(
        "struct s1 = s2 a "
        "struct s2 = s1 b",
        errCyclicStruct(NO_SRC, "a", "s1"),
        errCyclicStruct(NO_SRC, "b", "s2"));
    CHECK_DIAG(
        "struct s1 = s2 a "
        "struct s2 = int a, s1 b "
        "struct s3 = s1 a, s2 b",
        errCyclicStruct(NO_SRC, "a", "s1"),
        errCyclicStruct(NO_SRC, "b", "s2"),
        errCyclicStruct(NO_SRC, "a", "s3"));
    CHECK_DIAG(
        "struct s{T} = T T "
        "struct s2 = s{int} s",
        errFieldNameConflictsWithTypeSubstitution(NO_SRC, "T"),
        errInvalidTypeInstantiation(NO_SRC),
        errUndeclaredType(NO_SRC, "s", 1));
  }

  SECTION("Union diagnostics") {
    CHECK_DIAG("union u = hello, bool", errUndeclaredType(NO_SRC, "hello", 0));
    CHECK_DIAG("union u = int, int", errDuplicateTypeInUnion(NO_SRC, "u", "int", "int"));
    CHECK_DIAG(
        "union u{T} = int, T "
        "fun f(u{int} in) -> int 1",
        errDuplicateTypeInUnion(NO_SRC, "u{int}", "T", "int"),
        errInvalidTypeInstantiation(NO_SRC),
        errUndeclaredType(NO_SRC, "u", 1));
  }

  SECTION("Enum diagnostics") {
    CHECK_DIAG("enum e = a, b, a", errDuplicateEntryNameInEnum(NO_SRC, "a"));
  }
}

} // namespace frontend
