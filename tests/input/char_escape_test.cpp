#include "catch2/catch.hpp"
#include "input/char_escape.hpp"

namespace input {

TEST_CASE("[input] Escaping characters", "input") {
  CHECK(escape("\n") == "\\n");
  CHECK(escape("hello\n\tworld") == "hello\\n\\tworld");
}

} // namespace input
