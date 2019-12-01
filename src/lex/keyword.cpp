#include "lex/keyword.hpp"
#include <unordered_map>

namespace lex {

auto operator<<(std::ostream& out, const Keyword& rhs) -> std::ostream& {
  switch (rhs) {
  case Keyword::Fun:
    out << "fun";
    break;
  case Keyword::Struct:
    out << "struct";
    break;
  case Keyword::If:
    out << "if";
    break;
  case Keyword::Else:
    out << "else";
    break;
  }
  return out;
}

auto getKeyword(const std::string& str) -> std::optional<Keyword> {
  static const std::unordered_map<std::string, Keyword> keywordTable = {
      {"fun", Keyword::Fun},
      {"struct", Keyword::Struct},
      {"if", Keyword::If},
      {"else", Keyword::Else},
  };

  const auto keywordSearch = keywordTable.find(str);
  if (keywordSearch != keywordTable.end()) {
    const auto keyword = keywordSearch->second;
    return std::optional(keyword);
  }

  return std::nullopt;
}

} // namespace lex
