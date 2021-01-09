#include "catch2/catch.hpp"
#include "helpers.hpp"
#include "parse/error.hpp"
#include "parse/node_error.hpp"

namespace {

auto nestedInvalidParenExpr(int depth, parse::NodePtr node) -> parse::NodePtr {
  if (depth <= 0) {
    return node;
  }
  return errInvalidParenExpr(OPAREN, nestedInvalidParenExpr(depth - 1, std::move(node)), END);
};

} // namespace

namespace parse {

TEST_CASE("[parse] Parsing miscellaneous expressions", "parse") {

  SECTION("Max recursion depth") {
    CHECK_EXPR(
        std::string(100, '('), nestedInvalidParenExpr(99, errMaxExprRecursionDepthReached(OPAREN)));
  }
}

} // namespace parse
