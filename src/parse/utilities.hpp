#pragma once
#include "lex/token.hpp"
#include "parse/node.hpp"
#include <optional>
#include <vector>

namespace parse {

auto getKw(const lex::Token& token) -> std::optional<lex::Keyword>;
auto getId(const lex::Token& token) -> std::optional<std::string>;

auto getSpan(const std::vector<lex::Token>& tokens) -> std::optional<input::SourceSpan>;
auto getSpan(const std::vector<NodePtr>& nodes) -> std::optional<input::SourceSpan>;

auto nodesEqual(const std::vector<NodePtr>& a, const std::vector<NodePtr>& b) -> bool;

auto anyNodeNull(const std::vector<NodePtr>&) -> bool;

} // namespace parse
