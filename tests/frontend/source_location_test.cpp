#include "catch2/catch.hpp"
#include "helpers.hpp"
#include "prog/expr/node_lit_int.hpp"
#include "prog/expr/node_lit_string.hpp"

namespace frontend {

TEST_CASE("[frontend] Source location intrinsics", "frontend") {

  SECTION("Source location file is injected in an optional argument initializer") {
    const auto& output = ANALYZE("fun a(string f = intrinsic{source_loc_file}()) f "
                                 "fun b() a()");
    REQUIRE(output.isSuccess());

    const auto& funcDecl = GET_FUNC_DECL(output, "b");
    const auto& funcDef  = output.getProg().getFuncDef(funcDecl.getId());
    CHECK(
        funcDef.getBody() ==
        *prog::expr::callExprNode(
            output.getProg(),
            GET_FUNC_ID(output, "a", GET_TYPE_ID(output, "string")),
            EXPRS(prog::expr::litStringNode(output.getProg(), "test"))));
  }

  SECTION("Source location line is injected in an optional argument initializer") {
    const auto& output = ANALYZE("fun a(int i = intrinsic{source_loc_line}()) i\n"
                                 "fun b() a()");
    REQUIRE(output.isSuccess());

    const auto& funcDecl = GET_FUNC_DECL(output, "b");
    const auto& funcDef  = output.getProg().getFuncDef(funcDecl.getId());
    CHECK(
        funcDef.getBody() ==
        *prog::expr::callExprNode(
            output.getProg(),
            GET_FUNC_ID(output, "a", GET_TYPE_ID(output, "int")),
            EXPRS(prog::expr::litIntNode(output.getProg(), 2))));
  }

  SECTION("Source location column is injected in an optional argument initializer") {
    const auto& output = ANALYZE("fun a(int i = intrinsic{source_loc_column}()) i\n"
                                 "fun b() a()");
    REQUIRE(output.isSuccess());

    const auto& funcDecl = GET_FUNC_DECL(output, "b");
    const auto& funcDef  = output.getProg().getFuncDef(funcDecl.getId());
    CHECK(
        funcDef.getBody() ==
        *prog::expr::callExprNode(
            output.getProg(),
            GET_FUNC_ID(output, "a", GET_TYPE_ID(output, "int")),
            EXPRS(prog::expr::litIntNode(output.getProg(), 9))));
  }
}

} // namespace frontend
