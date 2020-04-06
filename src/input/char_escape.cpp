#include "input/char_escape.hpp"
#include <iomanip>
#include <locale>
#include <sstream>
#include <unordered_map>

namespace input {

auto escape(const char c) -> std::optional<char> {
  static const std::unordered_map<char, char> table = {
      {'"', '"'},
      {'\'', '\''},
      {'\\', '\\'},
      {'\0', '0'},
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
      {'\'', '\''},
      {'\\', '\\'},
      {'0', '\0'},
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

auto escapeNonPrintingAsHex(const std::string& str) -> std::string {
  std::stringstream ss;

  for (auto it = str.begin(); it != str.end(); ++it) {
    const auto c = *it;
    if (std::isprint(c)) {
      ss << c;
    } else {
      ss << '\\' << std::setfill('0') << std::setw(2) << std::hex
         << (0xff & static_cast<unsigned int>(c));
    }
  }

  return ss.str();
}

} // namespace input
