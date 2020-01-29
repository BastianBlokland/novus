#include "catch2/catch.hpp"
#include "helpers.hpp"

namespace frontend {

TEST_CASE("Analyzing parse diagnostics", "[frontend]") {

  CHECK_DIAG(
      "conWrite(1 + `)", error(src, "Parse error: Invalid character '`'", input::Span{13, 13}));
}

} // namespace frontend
