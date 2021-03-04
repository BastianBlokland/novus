#include "catch2/catch.hpp"
#include "helpers.hpp"

namespace frontend {

TEST_CASE("[frontend] Analyzing parse diagnostics", "frontend") {

  CHECK_DIAG("conWrite(1 + `)", error("Invalid character '`'", NO_SRC));
}

} // namespace frontend
