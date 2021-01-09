#include "catch2/catch.hpp"
#include "input/info.hpp"
#include "input/textpos.hpp"
#include <string>

namespace input {

TEST_CASE("[input] Input info", "input") {

  SECTION("Find text pos (lf)") {
    const auto input = std::string{"Hello\nBeautifull\nworld"};
    const auto info  = buildInfo(input.begin(), input.end());

    CHECK(info.getTextPos(0) == TextPos{1, 1});
    CHECK(info.getTextPos(1) == TextPos{1, 2});
    CHECK(info.getTextPos(4) == TextPos{1, 5});
    CHECK(info.getTextPos(5) == TextPos{1, 6}); // Line-feed, still on prev line.
    CHECK(info.getTextPos(6) == TextPos{2, 1});
    CHECK(info.getTextPos(15) == TextPos{2, 10});
    CHECK(info.getTextPos(17) == TextPos{3, 1});
    CHECK(info.getTextPos(22) == TextPos{3, 6}); // One beyond the last char.
  }

  SECTION("Find text pos (crlf)") {
    const auto input = std::string{"Hello\r\nBeautifull\r\nworld"};
    const auto info  = buildInfo(input.begin(), input.end());

    CHECK(info.getTextPos(0) == TextPos{1, 1});
    CHECK(info.getTextPos(1) == TextPos{1, 2});
    CHECK(info.getTextPos(4) == TextPos{1, 5});
    CHECK(info.getTextPos(5) == TextPos{1, 6}); // carriage-return, still on prev line.
    CHECK(info.getTextPos(6) == TextPos{1, 7}); // line-feed, still on prev line.
    CHECK(info.getTextPos(7) == TextPos{2, 1});
    CHECK(info.getTextPos(16) == TextPos{2, 10});
    CHECK(info.getTextPos(19) == TextPos{3, 1});
    CHECK(info.getTextPos(24) == TextPos{3, 6}); // One beyond the last char.
  }
}

} // namespace input
