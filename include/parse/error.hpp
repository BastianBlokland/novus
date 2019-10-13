#pragma once
#include "lex/token.hpp"
#include "parse/node_error.hpp"
#include <memory>
#include <string>

namespace parse {

auto errLexError(lex::Token errToken) -> std::unique_ptr<Node>;

auto errInvalidStmtStart(lex::Token token) -> std::unique_ptr<Node>;

} // namespace parse
