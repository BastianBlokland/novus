#pragma once
#include "lex/token.hpp"

namespace parse {

auto getKw(const lex::Token& token) -> std::optional<lex::Keyword>;
auto getId(const lex::Token& token) -> std::optional<std::string>;

} // namespace parse
