#pragma once
#include "lex/keyword.hpp"
#include "lex/token.hpp"
#include "lex/token_type.hpp"
#include "parse/node.hpp"
#include "parse/node_type.hpp"
#include "parse/utilities.hpp"
#include <memory>
#include <utility>

namespace parse {

class SwitchExprIfNode final : public Node {
public:
  SwitchExprIfNode() = delete;

  SwitchExprIfNode(lex::Token kw, NodePtr cond, lex::Token arrow, NodePtr rhs) :
      Node(NodeType::ExprSwitchIf),
      m_kw{std::move(kw)},
      m_cond{std::move(cond)},
      m_arrow{std::move(arrow)},
      m_rhs{std::move(rhs)} {

    if (getKw(m_kw) != lex::Keyword::If) {
      throw std::invalid_argument("Incorrect keyword for if clause");
    }
    if (m_cond == nullptr) {
      throw std::invalid_argument("Condition expr cannot be null");
    }
    if (m_arrow.getType() != lex::TokenType::SepArrow) {
      throw std::invalid_argument("Incorrect seperator token");
    }
    if (m_rhs == nullptr) {
      throw std::invalid_argument("Rhs cannot be null");
    }
  }

  auto operator==(const Node& rhs) const noexcept -> bool override {
    const auto r = dynamic_cast<const SwitchExprIfNode*>(&rhs);
    return r != nullptr && *m_cond == *r->m_cond && *m_rhs == *r->m_rhs;
  }

  auto operator!=(const Node& rhs) const noexcept -> bool override {
    return !SwitchExprIfNode::operator==(rhs);
  }

  [[nodiscard]] auto operator[](int i) const -> const Node& override {
    switch (i) {
    case 0:
      return *m_cond;
    case 1:
      return *m_rhs;
    default:
      throw std::out_of_range("No child at given index");
    }
  }

  [[nodiscard]] auto getChildCount() const -> int override { return 2; }

  [[nodiscard]] auto clone() const -> NodePtr override {
    return std::make_unique<SwitchExprIfNode>(m_kw, m_cond->clone(), m_arrow, m_rhs->clone());
  }

private:
  const lex::Token m_kw;
  const NodePtr m_cond;
  const lex::Token m_arrow;
  const NodePtr m_rhs;

  auto print(std::ostream& out) const -> std::ostream& override { return out << "if"; }
};

// Factories.
inline auto switchExprIfNode(lex::Token kw, NodePtr cond, lex::Token arrow, NodePtr rhs)
    -> NodePtr {
  return std::make_unique<SwitchExprIfNode>(
      std::move(kw), std::move(cond), std::move(arrow), std::move(rhs));
}

} // namespace parse
