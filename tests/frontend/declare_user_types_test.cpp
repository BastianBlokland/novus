#include "catch2/catch.hpp"
#include "frontend/diag_defs.hpp"
#include "helpers.hpp"

namespace frontend {

TEST_CASE("[frontend] Analyzing user-type declarations", "frontend") {

  SECTION("Declare basic struct") {
    const auto& output = ANALYZE("struct s = int i");
    REQUIRE(output.isSuccess());
    CHECK(TYPE_EXISTS(output, "s"));
  }

  SECTION("Declare templated struct") {
    const auto& output = ANALYZE("struct tuple{T1, T2} = T1 a, T2 b "
                                 "struct s = tuple{int, string} a, tuple{float, bool} b");
    REQUIRE(output.isSuccess());
    CHECK(TYPE_EXISTS(output, "tuple__int_string"));
    CHECK(TYPE_EXISTS(output, "tuple__float_bool"));
  }

  SECTION("Declare basic union") {
    const auto& output = ANALYZE("union u = int, float");
    REQUIRE(output.isSuccess());
    CHECK(TYPE_EXISTS(output, "u"));
  }

  SECTION("Declare templated union") {
    const auto& output = ANALYZE("union u{T1, T2} = T1, T2 "
                                 "struct s = u{int, string} a, u{float, bool} b");
    REQUIRE(output.isSuccess());
    CHECK(TYPE_EXISTS(output, "u__int_string"));
    CHECK(TYPE_EXISTS(output, "u__float_bool"));
  }

  SECTION("Declare templated struct and union") {
    const auto& output = ANALYZE("struct null "
                                 "union option{T} = T, null "
                                 "struct optTuple{T1, T2} = option{T1} a, option{T2} b "
                                 "union u{T1, T2} = optTuple{T1, T2}, float "
                                 "struct s = u{int, string} a, u{float, bool} b");
    REQUIRE(output.isSuccess());
    CHECK(TYPE_EXISTS(output, "option__int"));
    CHECK(TYPE_EXISTS(output, "option__float"));
    CHECK(TYPE_EXISTS(output, "option__string"));
    CHECK(TYPE_EXISTS(output, "option__bool"));
    CHECK(TYPE_EXISTS(output, "optTuple__int_string"));
    CHECK(TYPE_EXISTS(output, "optTuple__float_bool"));
    CHECK(TYPE_EXISTS(output, "u__int_string"));
    CHECK(TYPE_EXISTS(output, "u__float_bool"));
  }

  SECTION("Declare nested struct") {
    const auto& output = ANALYZE("struct s1 = int a "
                                 "struct s2 = s1 a, s1 b");
    REQUIRE(output.isSuccess());
    CHECK(TYPE_EXISTS(output, "s1"));
    CHECK(TYPE_EXISTS(output, "s2"));
  }

  SECTION("Declare nested templated struct") {
    const auto& output = ANALYZE("struct pair{T} = T a, T b "
                                 "struct s1 = int a "
                                 "struct s2 = pair{s1} a");
    REQUIRE(output.isSuccess());
    CHECK(TYPE_EXISTS(output, "pair__s1"));
    CHECK(TYPE_EXISTS(output, "s1"));
    CHECK(TYPE_EXISTS(output, "s2"));
  }

  SECTION("Declare basic enum") {
    const auto& output = ANALYZE("enum e = a, b, c");
    REQUIRE(output.isSuccess());
    CHECK(TYPE_EXISTS(output, "e"));
  }

  SECTION("Diagnostics") {
    CHECK_DIAG("struct int = bool i", errTypeAlreadyDeclared(NO_SRC, "int"));
    CHECK_DIAG("union int = int, bool", errTypeAlreadyDeclared(NO_SRC, "int"));
    CHECK_DIAG("struct function = bool i", errTypeNameIsReserved(NO_SRC, "function"));
    CHECK_DIAG("struct action = bool i", errTypeNameIsReserved(NO_SRC, "action"));
    CHECK_DIAG(
        "struct s = int i "
        "struct s = bool b",
        errTypeAlreadyDeclared(NO_SRC, "s"));
    CHECK_DIAG(
        "union u = int, bool "
        "union u = bool, int",
        errTypeAlreadyDeclared(NO_SRC, "u"));
    CHECK_DIAG(
        "union s = int, bool "
        "struct s = bool b",
        errTypeAlreadyDeclared(NO_SRC, "s"));
    CHECK_DIAG(
        "union s{T} = T, bool "
        "struct s = bool b",
        errTypeTemplateAlreadyDeclared(NO_SRC, "s"));
    CHECK_DIAG(
        "union s{T} = T, bool "
        "struct s{T} = T t",
        errTypeTemplateAlreadyDeclared(NO_SRC, "s"));
    CHECK_DIAG("enum int = a", errTypeAlreadyDeclared(NO_SRC, "int"));
    CHECK_DIAG("enum function = a", errTypeNameIsReserved(NO_SRC, "function"));
    CHECK_DIAG("enum action = a", errTypeNameIsReserved(NO_SRC, "action"));
  }
}

} // namespace frontend
