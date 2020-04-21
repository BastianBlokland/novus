#include "catch2/catch.hpp"
#include "frontend/diag_defs.hpp"
#include "helpers.hpp"
#include "prog/expr/node_call_self.hpp"
#include "prog/expr/node_lit_bool.hpp"
#include "prog/expr/node_lit_int.hpp"
#include "prog/expr/node_switch.hpp"

namespace frontend {

TEST_CASE("Analyzing self call expressions", "[frontend]") {

  SECTION("Get basic self call") {
    const auto& output = ANALYZE("fun f() false ? self() : 1 ");
    REQUIRE(output.isSuccess());

    auto conditions = std::vector<prog::expr::NodePtr>{};
    conditions.push_back(prog::expr::litBoolNode(output.getProg(), false));

    auto branches = std::vector<prog::expr::NodePtr>{};
    branches.push_back(prog::expr::callSelfExprNode(GET_TYPE_ID(output, "int"), {}));
    branches.push_back(prog::expr::litIntNode(output.getProg(), 1));
    auto switchExpr =
        prog::expr::switchExprNode(output.getProg(), std::move(conditions), std::move(branches));

    CHECK(GET_FUNC_DEF(output, "f").getExpr() == *switchExpr);
  }

  SECTION("Get anonymous function with self call") {
    const auto& output = ANALYZE("fun f() lambda () false ? self() : 1 ");
    REQUIRE(output.isSuccess());

    auto conditions = std::vector<prog::expr::NodePtr>{};
    conditions.push_back(prog::expr::litBoolNode(output.getProg(), false));

    auto branches = std::vector<prog::expr::NodePtr>{};
    branches.push_back(prog::expr::callSelfExprNode(GET_TYPE_ID(output, "int"), {}));
    branches.push_back(prog::expr::litIntNode(output.getProg(), 1));
    auto switchExpr =
        prog::expr::switchExprNode(output.getProg(), std::move(conditions), std::move(branches));

    CHECK(findAnonFuncDef(output, 0).getExpr() == *switchExpr);
  }

  SECTION("Diagnostics") {
    CHECK_DIAG("conWrite(self())", errSelfCallInNonFunc(src, input::Span{9, 14}));
    CHECK_DIAG(
        "fun f(int i) i < 0 ? fork self(0) : i", errForkedSelfCall(src, input::Span{21, 32}));
    CHECK_DIAG("fun f() self()", errSelfCallWithoutInferredRetType(src, input::Span{8, 13}));
    CHECK_DIAG(
        "fun f(int i) i < 0 ? self() : i",
        errIncorrectNumArgsInSelfCall(src, 1, 0, input::Span{21, 26}));
    CHECK_DIAG(
        "fun f(int i) i < 0 ? self(i, 1) : i",
        errIncorrectNumArgsInSelfCall(src, 1, 2, input::Span{21, 30}));
    CHECK_DIAG(
        "fun f(int i) i < 0 ? self(\"hello\") : i",
        errNoImplicitConversionFound(src, "string", "int", input::Span{26, 32}));
  }
}

} // namespace frontend
