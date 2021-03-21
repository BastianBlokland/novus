#include "catch2/catch.hpp"
#include "helpers.hpp"
#include "prog/expr/node_lit_int.hpp"
#include "prog/expr/node_lit_string.hpp"

namespace frontend {

TEST_CASE("[frontend] Analyze reflection", "frontend") {

  SECTION("Type") {

    SECTION("Type Kind") {
      const auto& output = ANALYZE("struct Type{T} "
                                   "fun f() Type{reflect_type_kind{sys_stream}}()");
      REQUIRE(output.isSuccess());
      CHECK(GET_FUNC_DECL(output, "f").getOutput() == GET_TYPE_ID(output, "Type__#6"));
    }
  }

  SECTION("Struct") {

    SECTION("Field count") {
      const auto& output = ANALYZE("struct Type{T} "
                                   "struct S = int A, float B, long C "
                                   "fun f() Type{reflect_struct_field_count{S}}()");
      REQUIRE(output.isSuccess());
      CHECK(GET_FUNC_DECL(output, "f").getOutput() == GET_TYPE_ID(output, "Type__#3"));
    }

    SECTION("Field name at index") {
      const auto& output =
          ANALYZE("struct S = int A, float B, long C "
                  "fun f() -> string intrinsic{reflect_struct_field_name}{S, #1}()");
      REQUIRE(output.isSuccess());
      CHECK(
          GET_FUNC_DEF(output, "f").getBody() == *prog::expr::litStringNode(output.getProg(), "B"));
    }

    SECTION("Field type at index") {
      const auto& output = ANALYZE("struct Type{T} "
                                   "struct S = int A, float B, long C "
                                   "fun f() Type{reflect_struct_field_type{S, #1}}()");
      REQUIRE(output.isSuccess());
      CHECK(GET_FUNC_DECL(output, "f").getOutput() == GET_TYPE_ID(output, "Type__float"));
    }
  }

  SECTION("Union") {

    SECTION("Union count") {
      const auto& output = ANALYZE("struct Type{T} "
                                   "union U = int, float, long "
                                   "fun f() Type{reflect_union_count{U}}()");
      REQUIRE(output.isSuccess());
      CHECK(GET_FUNC_DECL(output, "f").getOutput() == GET_TYPE_ID(output, "Type__#3"));
    }

    SECTION("Union type at index") {
      const auto& output = ANALYZE("struct Type{T} "
                                   "union U = int, float, long "
                                   "fun f() Type{reflect_union_type{U, #1}}()");
      REQUIRE(output.isSuccess());
      CHECK(GET_FUNC_DECL(output, "f").getOutput() == GET_TYPE_ID(output, "Type__float"));
    }
  }

  SECTION("Enum") {

    SECTION("Enum count") {
      const auto& output = ANALYZE("struct Type{T} "
                                   "enum E = A, B, C "
                                   "fun f() Type{reflect_enum_count{E}}()");
      REQUIRE(output.isSuccess());
      CHECK(GET_FUNC_DECL(output, "f").getOutput() == GET_TYPE_ID(output, "Type__#3"));
    }

    SECTION("Enum get key at index") {
      const auto& output = ANALYZE("enum E = A, B, C "
                                   "fun f() -> string intrinsic{reflect_enum_key}{E, #1}()");
      REQUIRE(output.isSuccess());
      CHECK(
          GET_FUNC_DEF(output, "f").getBody() == *prog::expr::litStringNode(output.getProg(), "B"));
    }

    SECTION("Enum get val at index") {
      const auto& output = ANALYZE("enum E = A, B : 42, C "
                                   "fun f() -> int intrinsic{reflect_enum_value}{E, #1}()");
      REQUIRE(output.isSuccess());
      CHECK(GET_FUNC_DEF(output, "f").getBody() == *prog::expr::litIntNode(output.getProg(), 42));
    }
  }
}

} // namespace frontend
