#pragma once
#include <iostream>
#include <optional>
#include <string>

namespace lex {

enum class Keyword { Function };

std::ostream& operator<<(std::ostream& out, const Keyword& rhs);

auto getKeyword(const std::string& str) -> std::optional<Keyword>;

} // namespace lex
