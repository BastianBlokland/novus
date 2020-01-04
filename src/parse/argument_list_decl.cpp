#include "parse/argument_list_decl.hpp"
#include "utilities.hpp"

namespace parse {

static auto getIdOrErr(const lex::Token& token) {
  return getId(token).value_or(std::string("err"));
}

ArgumentListDecl::ArgSpec::ArgSpec(Type type, lex::Token identifier) :
    m_type{std::move(type)}, m_identifier{std::move(identifier)} {}

auto ArgumentListDecl::ArgSpec::operator==(const ArgSpec& rhs) const noexcept -> bool {
  return m_type == rhs.m_type && m_identifier == rhs.m_identifier;
}

auto ArgumentListDecl::ArgSpec::getType() const noexcept -> const Type& { return m_type; }

auto ArgumentListDecl::ArgSpec::getIdentifier() const noexcept -> const lex::Token& {
  return m_identifier;
}

ArgumentListDecl::ArgumentListDecl(
    lex::Token open, std::vector<ArgSpec> args, std::vector<lex::Token> commas, lex::Token close) :
    m_open{std::move(open)},
    m_args{std::move(args)},
    m_commas{std::move(commas)},
    m_close{std::move(close)} {}

auto ArgumentListDecl::operator==(const ArgumentListDecl& rhs) const noexcept -> bool {
  return m_args == rhs.m_args;
}

auto ArgumentListDecl::operator!=(const ArgumentListDecl& rhs) const noexcept -> bool {
  return !ArgumentListDecl::operator==(rhs);
}

auto ArgumentListDecl::begin() const -> iterator { return m_args.begin(); }

auto ArgumentListDecl::end() const -> iterator { return m_args.end(); }

auto ArgumentListDecl::getCount() const -> unsigned int { return m_args.size(); }

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
  if (!std::all_of(m_args.begin(), m_args.end(), [](const auto& a) {
        return a.getIdentifier().getKind() == lex::TokenKind::Identifier && a.getType().validate();
      })) {
    return false;
  }
  if (m_commas.size() != (m_args.empty() ? 0 : m_args.size() - 1)) {
    return false;
  }
  return true;
}

auto operator<<(std::ostream& out, const ArgumentListDecl& rhs) -> std::ostream& {
  out << '(';
  for (auto i = 0U; i < rhs.m_args.size(); ++i) {
    if (i != 0) {
      out << ',';
    }
    out << rhs.m_args[i].getType() << '-' << getIdOrErr(rhs.m_args[i].getIdentifier());
  }
  return out << ')';
}

} // namespace parse
