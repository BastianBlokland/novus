#pragma once
#include <optional>

namespace lex {

auto escape(char c) -> std::optional<char>;
auto unescape(char c) -> std::optional<char>;

} // namespace lex
