#include "catch2/catch.hpp"
#include "frontend/diag_defs.hpp"
#include "helpers.hpp"

namespace frontend {

TEST_CASE("[frontend] Analyzing import statements", "frontend") {

  SECTION("Diagnostics") {
    CHECK_DIAG(
        "import \"nonexisting.nov\"",
        errUnresolvedImport(src, "nonexisting.nov", input::Span{7, 23}));
  }
}

} // namespace frontend
