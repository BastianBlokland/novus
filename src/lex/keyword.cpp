#include "lex/keyword.hpp"
#include <unordered_map>

namespace lex {

auto operator<<(std::ostream& out, const Keyword& rhs) -> std::ostream& {
  switch (rhs) {
  case Keyword::Intrinsic:
    out << "intrinsic";
    break;
  case Keyword::Import:
    out << "import";
    break;
  case Keyword::Fun:
    out << "fun";
    break;
  case Keyword::Act:
    out << "act";
    break;
  case Keyword::Self:
    out << "self";
    break;
  case Keyword::Lambda:
    out << "lambda";
    break;
  case Keyword::Impure:
    out << "impure";
    break;
  case Keyword::Fork:
    out << "fork";
    break;
  case Keyword::Lazy:
    out << "lazy";
    break;
  case Keyword::Struct:
    out << "struct";
    break;
  case Keyword::Union:
    out << "union";
    break;
  case Keyword::Enum:
    out << "enum";
    break;
  case Keyword::If:
    out << "if";
    break;
  case Keyword::Else:
    out << "else";
    break;
  case Keyword::Is:
    out << "is";
    break;
  case Keyword::As:
    out << "as";
    break;
  }
  return out;
}

auto getKeyword(const std::string& str) -> std::optional<Keyword> {
  static const std::unordered_map<std::string, Keyword> keywordTable = {
      {"intrinsic", Keyword::Intrinsic},
      {"import", Keyword::Import},
      {"fun", Keyword::Fun},
      {"act", Keyword::Act},
      {"self", Keyword::Self},
      {"lambda", Keyword::Lambda},
      {"impure", Keyword::Impure},
      {"fork", Keyword::Fork},
      {"lazy", Keyword::Lazy},
      {"struct", Keyword::Struct},
      {"union", Keyword::Union},
      {"enum", Keyword::Enum},
      {"if", Keyword::If},
      {"else", Keyword::Else},
      {"is", Keyword::Is},
      {"as", Keyword::As},
  };

  const auto keywordSearch = keywordTable.find(str);
  if (keywordSearch != keywordTable.end()) {
    const auto keyword = keywordSearch->second;
    return std::optional(keyword);
  }

  return std::nullopt;
}

} // namespace lex
