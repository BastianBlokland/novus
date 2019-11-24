#pragma once
#include <optional>
#include <string>

namespace input {

auto escape(char c) -> std::optional<char>;
auto escape(const std::string& str) -> std::string;

auto unescape(char c) -> std::optional<char>;

} // namespace input
