#include "parse/argument_list_decl.hpp"
#include "utilities.hpp"
#include <algorithm>
#include <cassert>

namespace parse {

static auto getIdOrErr(const lex::Token& token) {
  return getId(token).value_or(std::string("err"));
}

ArgumentListDecl::ArgInitializer::ArgInitializer(lex::Token eq, NodePtr expr) :
    m_eq{std::move(eq)}, m_expr{std::move(expr)} {}

auto ArgumentListDecl::ArgInitializer::operator==(const ArgInitializer& rhs) const noexcept
    -> bool {
  return *m_expr == *rhs.m_expr;
}

auto ArgumentListDecl::ArgInitializer::getEq() const noexcept -> const lex::Token& { return m_eq; }

auto ArgumentListDecl::ArgInitializer::getExpr() const noexcept -> const Node& { return *m_expr; }

auto ArgumentListDecl::ArgInitializer::takeExpr() noexcept -> NodePtr { return std::move(m_expr); }

auto ArgumentListDecl::ArgInitializer::validate() const -> bool {
  return m_eq.getKind() == lex::TokenKind::OpEq && m_expr;
}

ArgumentListDecl::ArgSpec::ArgSpec(
    Type type, lex::Token identifier, std::optional<ArgInitializer> initializer) :
    m_type{std::move(type)},
    m_identifier{std::move(identifier)},
    m_initializer{std::move(initializer)} {}

auto ArgumentListDecl::ArgSpec::operator==(const ArgSpec& rhs) const noexcept -> bool {
  return m_type == rhs.m_type && m_identifier == rhs.m_identifier;
}

auto ArgumentListDecl::ArgSpec::getSpan() const -> input::Span {
  if (m_initializer) {
    return input::Span::combine(m_type.getSpan(), m_initializer->getExpr().getSpan());
  }
  return input::Span::combine(m_type.getSpan(), m_identifier.getSpan());
}

auto ArgumentListDecl::ArgSpec::getType() const noexcept -> const Type& { return m_type; }

auto ArgumentListDecl::ArgSpec::getIdentifier() const noexcept -> const lex::Token& {
  return m_identifier;
}

auto ArgumentListDecl::ArgSpec::hasInitializer() const noexcept -> bool {
  return m_initializer.has_value();
}

auto ArgumentListDecl::ArgSpec::getInitializer() const noexcept -> const ArgInitializer& {
  assert(hasInitializer());
  return *m_initializer;
}

auto ArgumentListDecl::ArgSpec::getInitializer() noexcept -> ArgInitializer& {
  assert(hasInitializer());
  return *m_initializer;
}

auto ArgumentListDecl::ArgSpec::validate() const -> bool {
  if (m_identifier.getKind() != lex::TokenKind::Identifier) {
    return false;
  }
  if (!m_type.validate()) {
    return false;
  }
  if (m_initializer && !m_initializer->validate()) {
    return false;
  }
  return true;
}

ArgumentListDecl::ArgumentListDecl(
    lex::Token open,
    std::vector<ArgSpec> args,
    std::vector<lex::Token> commas,
    lex::Token close,
    bool missingComma) :
    m_open{std::move(open)},
    m_args{std::move(args)},
    m_commas{std::move(commas)},
    m_close{std::move(close)},
    m_missingComma{missingComma} {}

auto ArgumentListDecl::operator==(const ArgumentListDecl& rhs) const noexcept -> bool {
  return m_args == rhs.m_args;
}

auto ArgumentListDecl::operator!=(const ArgumentListDecl& rhs) const noexcept -> bool {
  return !ArgumentListDecl::operator==(rhs);
}

auto ArgumentListDecl::begin() const -> Iterator { return m_args.begin(); }

auto ArgumentListDecl::end() const -> Iterator { return m_args.end(); }

auto ArgumentListDecl::getCount() const -> unsigned int { return m_args.size(); }

auto ArgumentListDecl::getInitializerCount() const -> unsigned int {
  return std::count_if(
      m_args.begin(), m_args.end(), [](const auto& a) { return a.hasInitializer(); });
}

auto ArgumentListDecl::getInitializer(unsigned int i) const -> const Node& {
  for (const auto& arg : m_args) {
    if (arg.hasInitializer()) {
      if (i-- == 0) {
        return arg.getInitializer().getExpr();
      }
    }
  }
  throw std::out_of_range{"No initializer at given index"};
}

auto ArgumentListDecl::takeInitializer(unsigned int i) -> NodePtr {
  for (auto& arg : m_args) {
    if (arg.hasInitializer()) {
      if (i-- == 0) {
        return arg.getInitializer().takeExpr();
      }
    }
  }
  throw std::out_of_range{"No initializer at given index"};
}

auto ArgumentListDecl::getSpan() const -> input::Span {
  return input::Span::combine(m_open.getSpan(), m_close.getSpan());
}

auto ArgumentListDecl::getOpen() const -> const lex::Token& { return m_open; }

auto ArgumentListDecl::getArgs() const -> const std::vector<ArgSpec>& { return m_args; }

auto ArgumentListDecl::getCommas() const -> const std::vector<lex::Token>& { return m_commas; }

auto ArgumentListDecl::getClose() const -> const lex::Token& { return m_close; }

auto ArgumentListDecl::validate() const -> bool {
  if (!validateParentheses(m_open, m_close)) {
    return false;
  }
  if (!std::all_of(m_args.begin(), m_args.end(), [](const auto& a) { return a.validate(); })) {
    return false;
  }
  if (m_missingComma || m_commas.size() != (m_args.empty() ? 0 : m_args.size() - 1)) {
    return false;
  }
  return true;
}

auto ArgumentListDecl::hasMissingComma() const -> bool { return m_missingComma; }

auto operator<<(std::ostream& out, const ArgumentListDecl& rhs) -> std::ostream& {
  out << '(';
  for (auto i = 0U; i < rhs.m_args.size(); ++i) {
    if (i != 0) {
      out << ',';
    }
    out << rhs.m_args[i].getType() << '-' << getIdOrErr(rhs.m_args[i].getIdentifier());
    if (rhs.m_args[i].hasInitializer()) {
      out << "-opt";
    }
  }
  return out << ')';
}

} // namespace parse
