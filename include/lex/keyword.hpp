#pragma once
#include <iostream>
#include <optional>
#include <string>

namespace lex {

enum class Keyword {
  Intrinsic,
  Import,
  Fun,
  Act,
  Self,
  Lambda,
  Impure,
  Fork,
  Lazy,
  Struct,
  Union,
  Enum,
  If,
  Else,
  Is,
  As,
  Noinline
};

auto operator<<(std::ostream& out, const Keyword& rhs) -> std::ostream&;

auto getKeyword(const std::string& str) -> std::optional<Keyword>;

} // namespace lex
