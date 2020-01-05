#include "catch2/catch.hpp"
#include "frontend/diag_defs.hpp"
#include "helpers.hpp"
#include "prog/expr/node_const.hpp"
#include "prog/expr/node_lit_int.hpp"
#include "prog/expr/node_lit_string.hpp"

namespace frontend {

TEST_CASE("Analyzing index expressions", "[frontend]") {

  SECTION("Get single argument index operator") {
    const auto& output = ANALYZE("struct Pair = int a, int b "
                                 "fun [](Pair p, int i) "
                                 "  if i == 0 -> p.a "
                                 "  else      -> p.b "
                                 "fun f(Pair p) p[0]");
    REQUIRE(output.isSuccess());
    const auto& func = GET_FUNC_DEF(output, "f", GET_TYPE_ID(output, "Pair"));

    auto args = std::vector<prog::expr::NodePtr>{};
    args.push_back(prog::expr::constExprNode(func.getConsts(), *func.getConsts().lookup("p")));
    args.push_back(prog::expr::litIntNode(output.getProg(), 0));

    CHECK(
        func.getExpr() ==
        *prog::expr::callExprNode(
            output.getProg(),
            GET_FUNC_ID(
                output,
                "__op_squaresquare",
                GET_TYPE_ID(output, "Pair"),
                GET_TYPE_ID(output, "int")),
            std::move(args)));
  }

  SECTION("Get multi argument index operator") {
    const auto& output = ANALYZE("struct Pair = string a, string b "
                                 "fun [](Pair p, int i, string b) "
                                 "  if i == 0 -> Pair(p.a, b) "
                                 "  else      -> Pair(p.b, b) "
                                 "fun f(Pair p) p[0, \"hello world\"]");
    REQUIRE(output.isSuccess());
    const auto& func = GET_FUNC_DEF(output, "f", GET_TYPE_ID(output, "Pair"));

    auto args = std::vector<prog::expr::NodePtr>{};
    args.push_back(prog::expr::constExprNode(func.getConsts(), *func.getConsts().lookup("p")));
    args.push_back(prog::expr::litIntNode(output.getProg(), 0));
    args.push_back(prog::expr::litStringNode(output.getProg(), "hello world"));

    CHECK(
        func.getExpr() ==
        *prog::expr::callExprNode(
            output.getProg(),
            GET_FUNC_ID(
                output,
                "__op_squaresquare",
                GET_TYPE_ID(output, "Pair"),
                GET_TYPE_ID(output, "int"),
                GET_TYPE_ID(output, "string")),
            std::move(args)));
  }

  SECTION("Diagnostics") {
    CHECK_DIAG(
        "fun f(int i) -> int i[0]",
        errUndeclaredIndexOperator(src, {"int", "int"}, input::Span{20, 23}));
  }
}

} // namespace frontend