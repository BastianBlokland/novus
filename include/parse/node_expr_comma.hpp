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

class CommaExprNode final : public Node {
public:
  CommaExprNode() = delete;

  CommaExprNode(std::vector<NodePtr> subExprs, std::vector<lex::Token> commas) :
      Node(NodeType::ExprComma), m_subExprs{std::move(subExprs)}, m_commas{std::move(commas)} {

    if (m_subExprs.size() < 2) {
      throw std::invalid_argument("Comma expression has to contain aleast two sub-expressions");
    }
    if (std::any_of(
            m_subExprs.begin(), m_subExprs.end(), [](const NodePtr& p) { return p == nullptr; })) {
      throw std::invalid_argument("subExprs cannot contain a nullptr");
    }
    if (m_commas.size() != m_subExprs.size() - 1) {
      throw std::invalid_argument("Incorrect number of commas");
    }
    if (std::any_of(m_commas.begin(), m_commas.end(), [](const lex::Token& t) {
          return t.getType() != lex::TokenType::SepComma;
        })) {
      throw std::invalid_argument("Cannot contain a non-comma token");
    }
  }

  auto operator==(const Node& rhs) const noexcept -> bool override {
    const auto r = dynamic_cast<const CommaExprNode*>(&rhs);
    return r != nullptr && m_subExprs.size() == r->m_subExprs.size() &&
        std::equal(
               m_subExprs.begin(),
               m_subExprs.end(),
               r->m_subExprs.begin(),
               [](const NodePtr& l, const NodePtr& r) { return *l == *r; }) &&
        m_commas == r->m_commas;
  }

  auto operator!=(const Node& rhs) const noexcept -> bool override {
    return !CommaExprNode::operator==(rhs);
  }

  [[nodiscard]] auto operator[](int i) const -> const Node& override {
    if (i < 0 || static_cast<unsigned>(i) >= m_subExprs.size()) {
      throw std::out_of_range("No child at given index");
    }
    return *m_subExprs[i];
  }

  [[nodiscard]] auto getChildCount() const -> int override { return m_subExprs.size(); }

  [[nodiscard]] auto clone() const -> NodePtr override {
    return std::make_unique<CommaExprNode>(cloneSet(m_subExprs), m_commas);
  }

private:
  const std::vector<NodePtr> m_subExprs;
  const std::vector<lex::Token> m_commas;

  auto print(std::ostream& out) const -> std::ostream& override { return out << "comma"; }
};

// Factories.
inline auto commaExprNode(std::vector<NodePtr> subExprs, std::vector<lex::Token> commas)
    -> NodePtr {
  return std::make_unique<CommaExprNode>(std::move(subExprs), std::move(commas));
}

template <typename Container>
inline auto commaExprNode(Container subExprs) -> NodePtr {
  auto subExprsVec = std::vector<NodePtr>{};
  for (auto& subExpr : subExprs) {
    subExprsVec.push_back(std::move(subExpr));
  }
  return std::make_unique<CommaExprNode>(
      std::move(subExprsVec),
      std::vector<lex::Token>(subExprs.size() - 1, lex::basicToken(lex::TokenType::SepComma)));
}

} // namespace parse
