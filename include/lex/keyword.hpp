#pragma once
#include <iostream>
#include <optional>
#include <string>

namespace lex {

enum class Keyword { Import, Fun, Action, Lambda, Struct, Union, Enum, If, Else, Is, As };

auto operator<<(std::ostream& out, const Keyword& rhs) -> std::ostream&;

auto getKeyword(const std::string& str) -> std::optional<Keyword>;

} // namespace lex
