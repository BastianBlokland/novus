#include "catch2/catch.hpp"
#include "helpers.hpp"
#include "prog/expr/node_const.hpp"
#include "prog/expr/node_field.hpp"
#include "prog/expr/node_lit_bool.hpp"
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

    SECTION("Field at index") {
      const auto& output = ANALYZE("struct Type{T} "
                                   "struct S = int A, float B, long C "
                                   "fun f(S s) -> reflect_struct_field_type{S, #1} "
                                   "  intrinsic{reflect_struct_field}{S, #1}(s)");
      REQUIRE(output.isSuccess());
      const auto& funcDef   = GET_FUNC_DEF(output, "f", GET_TYPE_ID(output, "S"));
      const auto& structDef = std::get<prog::sym::StructDef>(GET_TYPE_DEF(output, "S"));
      CHECK(
          funcDef.getBody() ==
          *prog::expr::fieldExprNode(
              output.getProg(),
              prog::expr::constExprNode(funcDef.getConsts(), *funcDef.getConsts().lookup("s")),
              structDef.getFields()[1].getId()));
    }

    SECTION("Equivalent structs") {
      const auto& output = ANALYZE("struct Type{T} "
                                   "struct S1 = int A, float B, long C "
                                   "struct S2 = int A, float B, long C "
                                   "fun f()"
                                   "  Type{reflect_usertypes_are_equivalent{S1, S2}}()");
      REQUIRE(output.isSuccess());
      CHECK(GET_FUNC_DECL(output, "f").getOutput() == GET_TYPE_ID(output, "Type__#1"));
    }

    SECTION("Non equivalent structs") {
      const auto& output = ANALYZE("struct Type{T} "
                                   "struct S1 = int A, float B, long C "
                                   "struct S2 = int A, float B "
                                   "fun f()"
                                   "  Type{reflect_usertypes_are_equivalent{S1, S2}}()");
      REQUIRE(output.isSuccess());
      CHECK(GET_FUNC_DECL(output, "f").getOutput() == GET_TYPE_ID(output, "Type__#0"));
    }

    SECTION("Alias struct") {
      const auto& output = ANALYZE("struct S1 = int A, float B, long C "
                                   "struct S2 = int A, float B, long C "
                                   "fun f(S1 s) -> S2 "
                                   "  intrinsic{reflect_usertype_alias}{S1, S2}(s)");
      REQUIRE(output.isSuccess());
      const auto& funcDef = GET_FUNC_DEF(output, "f", GET_TYPE_ID(output, "S1"));
      CHECK(
          funcDef.getBody() ==
          *prog::expr::callExprNode(
              output.getProg(),
              *output.getProg().lookupIntrinsic(
                  "__alias_usertype_S1_S2", {GET_TYPE_ID(output, "S1")}),
              EXPRS(prog::expr::constExprNode(
                  funcDef.getConsts(), *funcDef.getConsts().lookup("s")))));
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

    SECTION("Equivalent unions") {
      const auto& output = ANALYZE("struct Type{T} "
                                   "union U1 = int, float, long "
                                   "union U2 = int, float, long "
                                   "fun f()"
                                   "  Type{reflect_usertypes_are_equivalent{U1, U2}}()");
      REQUIRE(output.isSuccess());
      CHECK(GET_FUNC_DECL(output, "f").getOutput() == GET_TYPE_ID(output, "Type__#1"));
    }

    SECTION("Non equivalent unions") {
      const auto& output = ANALYZE("struct Type{T} "
                                   "union U1 = int, float, long "
                                   "union U2 = int, float, bool "
                                   "fun f()"
                                   "  Type{reflect_usertypes_are_equivalent{U1, U2}}()");
      REQUIRE(output.isSuccess());
      CHECK(GET_FUNC_DECL(output, "f").getOutput() == GET_TYPE_ID(output, "Type__#0"));
    }

    SECTION("Alias union") {
      const auto& output = ANALYZE("union U1 = int, float, long "
                                   "union U2 = int, float, long "
                                   "fun f(U1 u) -> U2 "
                                   "  intrinsic{reflect_usertype_alias}{U1, U2}(u)");
      REQUIRE(output.isSuccess());
      const auto& funcDef = GET_FUNC_DEF(output, "f", GET_TYPE_ID(output, "U1"));
      CHECK(
          funcDef.getBody() ==
          *prog::expr::callExprNode(
              output.getProg(),
              *output.getProg().lookupIntrinsic(
                  "__alias_usertype_U1_U2", {GET_TYPE_ID(output, "U1")}),
              EXPRS(prog::expr::constExprNode(
                  funcDef.getConsts(), *funcDef.getConsts().lookup("u")))));
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

  SECTION("Delegate") {

    SECTION("Input count") {
      const auto& output =
          ANALYZE("struct Type{T} "
                  "fun f() Type{reflect_delegate_input_count{function{int, bool, float, char}}}()");
      REQUIRE(output.isSuccess());
      CHECK(GET_FUNC_DECL(output, "f").getOutput() == GET_TYPE_ID(output, "Type__#3"));
    }

    SECTION("Input count") {
      const auto& output = ANALYZE("struct Type{T} "
                                   "fun f() Type{reflect_delegate_input_count{function{char}}}()");
      REQUIRE(output.isSuccess());
      CHECK(GET_FUNC_DECL(output, "f").getOutput() == GET_TYPE_ID(output, "Type__#0"));
    }

    SECTION("Input type") {
      const auto& output = ANALYZE(
          "struct Type{T} "
          "fun f() Type{reflect_delegate_input_type{function{int, bool, float, char}, #1}}()");
      REQUIRE(output.isSuccess());
      CHECK(GET_FUNC_DECL(output, "f").getOutput() == GET_TYPE_ID(output, "Type__bool"));
    }

    SECTION("Return type") {
      const auto& output =
          ANALYZE("struct Type{T} "
                  "fun f() Type{reflect_delegate_ret_type{function{int, bool, float, char}}}()");
      REQUIRE(output.isSuccess());
      CHECK(GET_FUNC_DECL(output, "f").getOutput() == GET_TYPE_ID(output, "Type__char"));
    }

    SECTION("IsAction") {
      const auto& output =
          ANALYZE("fun f() -> bool intrinsic{reflect_delegate_is_action}{function{float, char}}()");
      REQUIRE(output.isSuccess());
      CHECK(
          GET_FUNC_DEF(output, "f").getBody() == *prog::expr::litBoolNode(output.getProg(), false));
    }

    SECTION("IsAction") {
      const auto& output =
          ANALYZE("fun f() -> bool intrinsic{reflect_delegate_is_action}{action{float, char}}()");
      REQUIRE(output.isSuccess());
      CHECK(
          GET_FUNC_DEF(output, "f").getBody() == *prog::expr::litBoolNode(output.getProg(), true));
    }
  }
}

} // namespace frontend
