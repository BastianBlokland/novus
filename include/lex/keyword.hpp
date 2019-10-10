#pragma once
#include <iostream>
#include <optional>
#include <string>

namespace lex {

enum class Keyword { If, Else, Fun, Print };

auto operator<<(std::ostream& out, const Keyword& rhs) -> std::ostream&;

auto getKeyword(const std::string& str) -> std::optional<Keyword>;

} // namespace lex
