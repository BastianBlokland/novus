#include "lex/keyword.hpp"
#include <unordered_map>

namespace lex {

auto operator<<(std::ostream& out, const Keyword& rhs) -> std::ostream& {
  switch (rhs) {
  case Keyword::Function:
    out << "fun";
    break;
  }
  return out;
}

auto getKeyword(const std::string& str) -> std::optional<Keyword> {
  static const std::unordered_map<std::string, Keyword> keywordTable = {{"fun", Keyword::Function}};

  const auto keywordSearch = keywordTable.find(str);
  if (keywordSearch != keywordTable.end()) {
    const auto keyword = keywordSearch->second;
    return std::optional(keyword);
  }

  return std::nullopt;
}

} // namespace lex
