#include "catch2/catch.hpp"
#include "helpers.hpp"
#include "parse/error.hpp"
#include "parse/node_comment.hpp"

namespace parse {

TEST_CASE("Parsing comments", "[parse]") {

  CHECK_STMT("// Hello world", commentNode(COMMENT(" Hello world")));
  CHECK_STMT(
      "// Hello\n"
      "struct s = int a\n"
      "// World",
      commentNode(COMMENT(" Hello")),
      structDeclStmtNode(
          STRUCT,
          ID("s"),
          std::nullopt,
          EQ,
          {StructDeclStmtNode::FieldSpec(TYPE("int"), ID("a"))},
          COMMAS(0)),
      commentNode(COMMENT(" World")));

  SECTION("Spans") { CHECK_STMT_SPAN(" // Hello world ", input::Span(1, 15)); }
}

} // namespace parse
