#include "parse/error.hpp"
#include "lex/token.hpp"
#include "lex/token_payload.hpp"
#include "lex/token_type.hpp"
#include "parse/node_error.hpp"
#include <string>

namespace parse {

auto errLexError(lex::Token errToken) -> std::unique_ptr<Node> {
  if (errToken.getType() != lex::TokenType::Error) {
    throw std::invalid_argument("Given token is not an error-token");
  }

  const auto msg = "Invalid token: " + errToken.getPayload<lex::ErrorTokenPayload>()->getMessage();
  return errorNode(msg, std::move(errToken));
}

auto errInvalidStmtStart(lex::Token token) -> std::unique_ptr<Node> {
  const auto msg = "Invalid statement start: " + token.str();
  return errorNode(msg, std::move(token));
}

} // namespace parse
