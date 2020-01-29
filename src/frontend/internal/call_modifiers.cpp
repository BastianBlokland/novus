#include "internal/call_modifiers.hpp"
#include "internal/utilities.hpp"
#include <sstream>

namespace frontend::internal {

static auto addArg(
    prog::expr::NodePtr arg,
    std::pair<std::vector<prog::expr::NodePtr>, prog::sym::TypeSet>* args) {

  args->second = args->second.withExtraType(arg->getType());
  args->first.push_back(std::move(arg));
}

auto modifyCall(
    const Context* ctx,
    const lex::Token& nameToken,
    const parse::CallExprNode& node,
    std::pair<std::vector<prog::expr::NodePtr>, prog::sym::TypeSet>* args) -> void {

  // Assert has an exception where you are allowed omit the second arg (the message).
  if (getName(nameToken) == "assert" && args->second.getCount() == 1 &&
      args->second[0] == ctx->getProg()->getBool()) {

    // Construct a message containing the source position.
    auto startPos = ctx->getSrc().getTextPos(node.getSpan().getStart());
    auto endPos   = ctx->getSrc().getTextPos(node.getSpan().getEnd());

    std::ostringstream msgoss;
    msgoss << ctx->getSrc().getId() << " [" << startPos << "] - [" << endPos << "]";

    // Add the extra message arg.
    addArg(prog::expr::litStringNode(*ctx->getProg(), msgoss.str()), args);
  }
}

} // namespace frontend::internal
