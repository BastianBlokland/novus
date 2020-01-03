#pragma once
#include "lex/token.hpp"
#include "parse/node.hpp"
#include <optional>
#include <vector>

namespace parse {

[[nodiscard]] auto getKw(const lex::Token& token) -> std::optional<lex::Keyword>;
[[nodiscard]] auto getId(const lex::Token& token) -> std::optional<std::string>;

[[nodiscard]] auto getSpan(const std::vector<lex::Token>& tokens) -> std::optional<input::Span>;
[[nodiscard]] auto getSpan(const std::vector<NodePtr>& nodes) -> std::optional<input::Span>;

[[nodiscard]] auto nodesEqual(const std::vector<NodePtr>& a, const std::vector<NodePtr>& b) -> bool;

[[nodiscard]] auto anyNodeNull(const std::vector<NodePtr>& v) -> bool;

[[nodiscard]] auto validateParentheses(const lex::Token& open, const lex::Token& close) -> bool;

} // namespace parse
