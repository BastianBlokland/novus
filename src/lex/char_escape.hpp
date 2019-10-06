#pragma once
#include <optional>

namespace lex {

auto escape(const char c) -> std::optional<char>;
auto unescape(const char c) -> std::optional<char>;

} // namespace lex
