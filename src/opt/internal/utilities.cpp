#include "prog/expr/node_lit_bool.hpp"
#include "prog/expr/node_lit_char.hpp"
#include "prog/expr/node_lit_enum.hpp"
#include "prog/expr/node_lit_float.hpp"
#include "prog/expr/node_lit_int.hpp"
#include "prog/expr/node_lit_long.hpp"
#include "prog/expr/node_lit_string.hpp"
#include "prog/expr/rewriter.hpp"
#include <cassert>

namespace opt::internal {

auto getInt(const prog::expr::Node& n) -> int32_t {
  switch (n.getKind()) {
  case prog::expr::NodeKind::LitEnum:
    return n.downcast<prog::expr::LitEnumNode>()->getVal();
  case prog::expr::NodeKind::LitChar:
    return static_cast<int32_t>(n.downcast<prog::expr::LitCharNode>()->getVal());
  default:
    return n.downcast<prog::expr::LitIntNode>()->getVal();
  }
}

auto getBool(const prog::expr::Node& n) -> bool {
  return n.downcast<prog::expr::LitBoolNode>()->getVal();
}

auto getFloat(const prog::expr::Node& n) -> float {
  return n.downcast<prog::expr::LitFloatNode>()->getVal();
}

auto getLong(const prog::expr::Node& n) -> int64_t {
  return n.downcast<prog::expr::LitLongNode>()->getVal();
}

auto getChar(const prog::expr::Node& n) -> uint8_t {
  return n.downcast<prog::expr::LitCharNode>()->getVal();
}

auto getString(const prog::expr::Node& n) -> std::string {
  return n.downcast<prog::expr::LitStringNode>()->getVal();
}

auto rewriteAll(const std::vector<prog::expr::NodePtr>& nodes, prog::expr::Rewriter* rewriter)
    -> std::vector<prog::expr::NodePtr> {

  auto newNodes = std::vector<prog::expr::NodePtr>{};
  newNodes.reserve(nodes.size());
  for (const auto& node : nodes) {
    newNodes.push_back(rewriter->rewrite(*node));
  }
  return newNodes;
}

auto cloneToVec(const prog::expr::NodePtr& node) -> std::vector<prog::expr::NodePtr> {
  auto newNodes = std::vector<prog::expr::NodePtr>{};
  newNodes.reserve(1);
  newNodes.push_back(node->clone(nullptr));
  return newNodes;
}

} // namespace opt::internal
