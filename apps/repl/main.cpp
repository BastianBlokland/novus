#include "lex/lexer.hpp"
#include "lex/utilities.hpp"
#include <ncurses.h>

enum class ColorType {
  Normal = 1,
  Red,
  Green,
  Yellow,
  Blue,
  Magenta,
  Cyan,
  Error,
};

auto getColor(const lex::Token& token) {
  switch (token.getCat()) {
  case lex::TokenCat::Operator:
    return ColorType::Normal;
  case lex::TokenCat::Seperator:
    return ColorType::Magenta;
  case lex::TokenCat::Literal:
    return ColorType::Yellow;
  case lex::TokenCat::Keyword:
    return ColorType::Blue;
  case lex::TokenCat::Identifier:
    return ColorType::Green;
  case lex::TokenCat::Error:
    return ColorType::Error;
  case lex::TokenCat::Unknown:
    return ColorType::Normal;
  }
}

auto renderChar(const int y, const int x, const char c, const ColorType color) {
  // NOLINTNEXTLINE(hicpp-signed-bitwise)
  mvaddch(y, x, static_cast<unsigned int>(c) | COLOR_PAIR(static_cast<unsigned int>(color)));
}

auto render(const std::vector<char>& input) {
  const auto tokens = lex::lexAll(input.begin(), input.end());
  clear();

  // Draw input.
  auto x = 0;
  auto y = 0;
  for (size_t i = 0; i < input.size(); ++i) {
    const char c = input[i];
    if (c == '\n') {
      ++y;
      x = 0;
    } else {
      const auto optToken = lex::findToken(tokens.begin(), tokens.end(), i);
      if (optToken) {
        renderChar(y, x, c, getColor(optToken.value()));
      } else {
        renderChar(y, x, c, ColorType::Normal);
      }
      ++x;
    }
  }
}

auto run() {
  const char escapeChar = 27;
  const char deleteChar = 127;

  int cursor = 0;
  std::vector<char> input{};
  while (true) {
    int c = getch();
    switch (c) {
    case escapeChar:
      return;
    case deleteChar:
      if (!input.empty() && cursor > 0) {
        input.erase(input.begin() + cursor - 1);
        --cursor;
      }
      break;
    case KEY_UP:
    case KEY_DOWN:
      break;
    case KEY_RIGHT:
      if (cursor < static_cast<int>(input.size())) {
        ++cursor;
      }
      break;
    case KEY_LEFT:
      if (cursor > 0) {
        --cursor;
      }
      break;
    default:
      if (c < std::numeric_limits<char>::max()) {
        input.insert(input.begin() + cursor, static_cast<char>(c));
        ++cursor;
      }
      break;
    }

    render(input);
    move(0, cursor);
    refresh();
  }
}

auto main() -> int {
  const int escDelay = 50;

  // Init ncurses.
  initscr();                // Setup screen.
  cbreak();                 // Disable input buffering.
  noecho();                 // Disable echoing input.
  intrflush(stdscr, false); // Flush on interupt.
  keypad(stdscr, true);     // Enable cursor keys.
  curs_set(1);              // No cursor.
  set_escdelay(escDelay);   // Reduce delay for esc key.

  if (has_colors()) {
    start_color();
    init_pair(static_cast<int>(ColorType::Normal), COLOR_WHITE, COLOR_BLACK);
    init_pair(static_cast<int>(ColorType::Red), COLOR_RED, COLOR_BLACK);
    init_pair(static_cast<int>(ColorType::Green), COLOR_GREEN, COLOR_BLACK);
    init_pair(static_cast<int>(ColorType::Yellow), COLOR_YELLOW, COLOR_BLACK);
    init_pair(static_cast<int>(ColorType::Blue), COLOR_BLUE, COLOR_BLACK);
    init_pair(static_cast<int>(ColorType::Magenta), COLOR_MAGENTA, COLOR_BLACK);
    init_pair(static_cast<int>(ColorType::Cyan), COLOR_CYAN, COLOR_BLACK);
    init_pair(static_cast<int>(ColorType::Error), COLOR_BLACK, COLOR_RED);
  }

  // Run application.
  run();

  // Deinit ncurses.
  endwin();
  return 0;
}
