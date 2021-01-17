#include "internal/call_modifiers.hpp"
#include "frontend/diag_defs.hpp"
#include "internal/utilities.hpp"
#include <cassert>
#include <sstream>

namespace frontend::internal {

static auto addArg(
    prog::expr::NodePtr arg,
    std::pair<std::vector<prog::expr::NodePtr>, prog::sym::TypeSet>& args) {

  args.second = args.second.withExtraType(arg->getType());
  args.first.push_back(std::move(arg));
}

auto modifyCallArgs(
    Context* ctx,
    const TypeSubstitutionTable* /*unused*/,
    const lex::Token& nameToken,
    std::optional<parse::TypeParamList> /*unused*/,
    const parse::CallExprNode& node,
    std::pair<std::vector<prog::expr::NodePtr>, prog::sym::TypeSet>& args) -> void {

  assert(ctx);

  // If 'assert' is called with a single argument (the condition) then we synthesize another
  // argument containing the message (with the source-location of the assert).
  if (getName(nameToken) == "assert" && args.second.getCount() == 1 &&
      args.second[0] == ctx->getProg()->getBool()) {

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
