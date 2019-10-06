#pragma once
#include "lex/token.hpp"
#include <algorithm>
#include <optional>

using std::is_same;
using std::iterator_traits;

namespace lex {

template <typename InputItr>
auto findToken(InputItr begin, const InputItr end, const int sourcePos) -> std::optional<Token> {

  static_assert(is_same<typename iterator_traits<InputItr>::value_type, Token>::value,
                "Valuetype of input iterator has to be 'Token'");

  auto lower = std::lower_bound(begin, end, sourcePos,
                                [](const Token lhs, const int rhs) { return lhs.getSpan() < rhs; });
  if (lower == end || lower->getSpan().getStart() > sourcePos) {
    return std::nullopt;
  }

  return *lower;
}

} // namespace lex
