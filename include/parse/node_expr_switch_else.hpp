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

class SwitchExprElseNode final : public Node {
public:
  SwitchExprElseNode() = delete;

  SwitchExprElseNode(lex::Token kw, lex::Token arrow, NodePtr rhs) :
      Node(NodeType::ExprSwitchElse),
      m_kw{std::move(kw)},
      m_arrow{std::move(arrow)},
      m_rhs{std::move(rhs)} {

    if (getKw(m_kw) != lex::Keyword::Else) {
      throw std::invalid_argument("Incorrect keyword for else clause");
    }
    if (m_arrow.getType() != lex::TokenType::SepArrow) {
      throw std::invalid_argument("Incorrect seperator token");
    }
    if (m_rhs == nullptr) {
      throw std::invalid_argument("Rhs cannot be null");
    }
  }

  auto operator==(const Node& rhs) const noexcept -> bool override {
    const auto r = dynamic_cast<const SwitchExprElseNode*>(&rhs);
    return r != nullptr && *m_rhs == *r->m_rhs;
  }

  auto operator!=(const Node& rhs) const noexcept -> bool override {
    return !SwitchExprElseNode::operator==(rhs);
  }

  [[nodiscard]] auto operator[](int i) const -> const Node& override {
    switch (i) {
    case 0:
      return *m_rhs;
    default:
      throw std::out_of_range("No child at given index");
    }
  }

  [[nodiscard]] auto getChildCount() const -> int override { return 1; }

  [[nodiscard]] auto clone() const -> NodePtr override {
    return std::make_unique<SwitchExprElseNode>(m_kw, m_arrow, m_rhs->clone());
  }

private:
  const lex::Token m_kw;
  const lex::Token m_arrow;
  const NodePtr m_rhs;

  auto print(std::ostream& out) const -> std::ostream& override { return out << "else"; }
};

// Factories.
inline auto switchExprElseNode(lex::Token kw, lex::Token arrow, NodePtr rhs) -> NodePtr {
  return std::make_unique<SwitchExprElseNode>(std::move(kw), std::move(arrow), std::move(rhs));
}

} // namespace parse
