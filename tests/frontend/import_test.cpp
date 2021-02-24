#include "catch2/catch.hpp"
#include "frontend/diag_defs.hpp"
#include "helpers.hpp"

namespace frontend {

TEST_CASE("[frontend] Analyzing import statements", "frontend") {

  SECTION("Diagnostics") {
    CHECK_DIAG(
        "import \"nonexisting.ns\"",
        errUnresolvedImport(src, "nonexisting.ns", input::Span{7, 22}));
  }
}

} // namespace frontend
