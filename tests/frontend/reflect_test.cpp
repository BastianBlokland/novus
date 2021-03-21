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
