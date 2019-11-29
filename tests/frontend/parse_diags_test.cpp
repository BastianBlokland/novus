#include "catch2/catch.hpp"
#include "helpers.hpp"

namespace frontend {

TEST_CASE("Analyzing parse diagnostics", "[frontend]") {

  CHECK_DIAG("print(1 + .)", error(src, "Parse error: Invalid character '.'", input::Span{10, 10}));
}

} // namespace frontend
