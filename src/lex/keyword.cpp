#include "lex/keyword.hpp"
#include <unordered_map>

namespace lex {

auto operator<<(std::ostream& out, const Keyword& rhs) -> std::ostream& {
  switch (rhs) {
  case Keyword::If:
    out << "if";
    break;
  case Keyword::Else:
    out << "else";
    break;
  case Keyword::Print:
    out << "print";
    break;
  }
  return out;
}

auto getKeyword(const std::string& str) -> std::optional<Keyword> {
  static const std::unordered_map<std::string, Keyword> keywordTable = {
      {"if", Keyword::If},
      {"else", Keyword::Else},
      {"print", Keyword::Print},
  };

  const auto keywordSearch = keywordTable.find(str);
  if (keywordSearch != keywordTable.end()) {
    const auto keyword = keywordSearch->second;
    return std::optional(keyword);
  }

  return std::nullopt;
}

} // namespace lex
