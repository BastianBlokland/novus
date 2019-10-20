#pragma once
#include "lex/token.hpp"
#include "lex/token_payload.hpp"
#include "lex/token_type.hpp"
#include "parse/node.hpp"
#include "parse/node_type.hpp"
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace parse {

inline auto getId(const lex::Token& token) {
  if (token.getType() == lex::TokenType::Identifier) {
    return token.getPayload<lex::IdentifierTokenPayload>()->getIdentifier();
  }
  return std::string{"err"};
}

class FuncDeclStmtNode final : public Node {
public:
  FuncDeclStmtNode() = delete;

  FuncDeclStmtNode(
      lex::Token retType,
      lex::Token id,
      lex::Token open,
      std::vector<std::pair<lex::Token, lex::Token>> args,
      std::vector<lex::Token> commas,
      lex::Token close,
      NodePtr body) :

      Node(NodeType::StmtFuncDecl),
      m_retType{std::move(retType)},
      m_id{std::move(id)},
      m_open{std::move(open)},
      m_args{std::move(args)},
      m_commas{std::move(commas)},
      m_close{std::move(close)},
      m_body{std::move(body)} {

    if (m_args.empty() ? !m_commas.empty() : m_commas.size() != m_args.size() - 1) {
      throw std::invalid_argument("Incorrect number of commas");
    }
    if (m_body == nullptr) {
      throw std::invalid_argument("Body cannot be null");
    }
  }

  auto operator==(const Node& rhs) const noexcept -> bool override {
    const auto r = dynamic_cast<const FuncDeclStmtNode*>(&rhs);
    return r != nullptr && m_retType == r->m_retType && m_id == r->m_id &&
        m_args.size() == r->m_args.size() &&
        std::equal(
               m_args.begin(),
               m_args.end(),
               r->m_args.begin(),
               [](const auto& l, const auto& r) {
                 return l.first == r.first && l.second == r.second;
               }) &&
        *m_body == *r->m_body;
  }

  auto operator!=(const Node& rhs) const noexcept -> bool override {
    return !FuncDeclStmtNode::operator==(rhs);
  }

  [[nodiscard]] auto operator[](int i) const -> const Node& override {
    switch (i) {
    case 0:
      return *m_body;
    default:
      throw std::out_of_range("No child at given index");
    }
  }

  [[nodiscard]] auto getChildCount() const -> int override { return 1; }

  [[nodiscard]] auto clone() const -> NodePtr override {
    return std::make_unique<FuncDeclStmtNode>(
        m_retType, m_id, m_open, m_args, m_commas, m_close, m_body->clone());
  }

private:
  const lex::Token m_retType;
  const lex::Token m_id;
  const lex::Token m_open;
  const std::vector<std::pair<lex::Token, lex::Token>> m_args;
  const std::vector<lex::Token> m_commas;
  const lex::Token m_close;
  const NodePtr m_body;

  auto print(std::ostream& out) const -> std::ostream& override {
    out << getId(m_retType) << '-' << getId(m_id) << '(';
    for (auto i = 0U; i < m_args.size(); ++i) {
      if (i != 0) {
        out << ",";
      }
      out << getId(m_args[i].first) << '-' << getId(m_args[i].second);
    }
    return out << ')';
  }
};

// Factories.
inline auto funcDeclStmtNode(
    lex::Token retType,
    lex::Token id,
    lex::Token open,
    std::vector<std::pair<lex::Token, lex::Token>> args,
    std::vector<lex::Token> commas,
    lex::Token close,
    NodePtr body) -> NodePtr {
  return std::make_unique<FuncDeclStmtNode>(
      std::move(retType),
      std::move(id),
      std::move(open),
      std::move(args),
      std::move(commas),
      std::move(close),
      std::move(body));
}

} // namespace parse
