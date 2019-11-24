#include "input/char_escape.hpp"
#include <unordered_map>

namespace input {

auto escape(const char c) -> std::optional<char> {
  static const std::unordered_map<char, char> table = {
      {'"', '"'},
      {'\\', '\\'},
      {'\a', 'a'},
      {'\b', 'b'},
      {'\f', 'f'},
      {'\n', 'n'},
      {'\r', 'r'},
      {'\t', 't'},
      {'\v', 'v'},
  };

  const auto search = table.find(c);
  if (search != table.end()) {
    return std::optional(search->second);
  }

  return std::nullopt;
}

auto escape(const std::string& str) -> std::string {
  auto result = std::string{};
  result.reserve(str.size());

  for (const auto& c : str) {
    const auto esc = escape(c);
    if (esc) {
      result.push_back('\\');
      result.push_back(*esc);
    } else {
      result.push_back(c);
    }
  }
  return result;
}

auto unescape(const char c) -> std::optional<char> {
  static const std::unordered_map<char, char> table = {
      {'"', '"'},
      {'\\', '\\'},
      {'a', '\a'},
      {'b', '\b'},
      {'f', '\f'},
      {'n', '\n'},
      {'r', '\r'},
      {'t', '\t'},
      {'v', '\v'},
  };

  const auto search = table.find(c);
  if (search != table.end()) {
    return std::optional(search->second);
  }

  return std::nullopt;
}

} // namespace input
