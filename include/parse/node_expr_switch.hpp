#pragma once
#include "lex/token.hpp"
#include "lex/token_type.hpp"
#include "parse/node.hpp"
#include "parse/node_type.hpp"
#include "parse/utilities.hpp"
#include <algorithm>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace parse {

class SwitchExprNode final : public Node {
public:
  SwitchExprNode() = delete;

  SwitchExprNode(std::vector<NodePtr> ifClauses, NodePtr elseClause) :
      Node(NodeType::ExprSwitch),
      m_ifClauses{std::move(ifClauses)},
      m_elseClause{std::move(elseClause)} {

    if (m_ifClauses.empty()) {
      throw std::invalid_argument("Atleast one if clause is required");
    }
    if (std::any_of(m_ifClauses.begin(), m_ifClauses.end(), [](const NodePtr& p) {
          return p == nullptr;
        })) {
      throw std::invalid_argument("Switch cannot contain a null if-clause");
    }
    if (m_elseClause == nullptr) {
      throw std::invalid_argument("Else-clause cannot be null");
    }
  }

  auto operator==(const Node& rhs) const noexcept -> bool override {
    const auto r = dynamic_cast<const SwitchExprNode*>(&rhs);
    return r != nullptr && m_ifClauses.size() == r->m_ifClauses.size() &&
        std::equal(
               m_ifClauses.begin(),
               m_ifClauses.end(),
               r->m_ifClauses.begin(),
               [](const NodePtr& l, const NodePtr& r) { return *l == *r; }) &&
        *m_elseClause == *r->m_elseClause;
  }

  auto operator!=(const Node& rhs) const noexcept -> bool override {
    return !SwitchExprNode::operator==(rhs);
  }

  [[nodiscard]] auto operator[](int i) const -> const Node& override {
    auto index = static_cast<unsigned>(i);
    if (i >= 0 && index < m_ifClauses.size()) {
      return *m_ifClauses[index];
    }
    if (index == m_ifClauses.size()) {
      return *m_elseClause;
    }
    throw std::out_of_range("No child at given index");
  }

  [[nodiscard]] auto getChildCount() const -> int override { return m_ifClauses.size() + 1; }

  [[nodiscard]] auto clone() const -> NodePtr override {
    return std::make_unique<SwitchExprNode>(cloneSet(m_ifClauses), m_elseClause->clone());
  }

private:
  const std::vector<NodePtr> m_ifClauses;
  const NodePtr m_elseClause;

  auto print(std::ostream& out) const -> std::ostream& override { return out << "switch"; }
};

// Factories.
inline auto switchExprNode(std::vector<NodePtr> ifClauses, NodePtr elseClause) -> NodePtr {
  return std::make_unique<SwitchExprNode>(std::move(ifClauses), std::move(elseClause));
}

} // namespace parse
