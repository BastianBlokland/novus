#include "char_escape.hpp"
#include <unordered_map>

namespace lex {

auto escape(const char c) -> std::optional<char> {
  static const std::unordered_map<char, char> table = {
      {'"', '"'},  {'\\', '\\'}, {'\a', 'a'}, {'\b', 'b'}, {'\f', 'f'},
      {'\n', 'n'}, {'\r', 'r'},  {'\t', 't'}, {'\v', 'v'},
  };

  const auto search = table.find(c);
  if (search != table.end()) {
    return std::optional(search->second);
  }

  return std::nullopt;
}

auto unescape(const char c) -> std::optional<char> {
  static const std::unordered_map<char, char> table = {
      {'"', '"'},  {'\\', '\\'}, {'a', '\a'}, {'b', '\b'}, {'f', '\f'},
      {'n', '\n'}, {'r', '\r'},  {'t', '\t'}, {'v', '\v'},
  };

  const auto search = table.find(c);
  if (search != table.end()) {
    return std::optional(search->second);
  }

  return std::nullopt;
}

} // namespace lex
