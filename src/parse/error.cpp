#include "parse/error.hpp"
#include "lex/token.hpp"
#include "lex/token_payload.hpp"
#include "lex/token_type.hpp"
#include "parse/node_error.hpp"
#include <sstream>
#include <string>

namespace parse {

auto errLexError(lex::Token errToken) -> NodePtr {
  if (errToken.getType() != lex::TokenType::Error) {
    throw std::invalid_argument("Given token is not an error-token");
  }
  const auto msg = errToken.getPayload<lex::ErrorTokenPayload>()->getMessage();
  return errorNode(msg, std::move(errToken));
}

auto errInvalidStmtStart(lex::Token token) -> NodePtr {
  if (token.isError()) {
    return errLexError(token);
  }

  std::ostringstream oss;
  oss << "Invalid statement start: " << token;
  return errorNode(oss.str(), std::move(token));
}

} // namespace parse
