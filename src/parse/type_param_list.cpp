#include "parse/type_param_list.hpp"
#include "utilities.hpp"

namespace parse {

TypeParamList::TypeParamList(
    lex::Token open, std::vector<Type> params, std::vector<lex::Token> commas, lex::Token close) :
    m_open{std::move(open)},
    m_params{std::move(params)},
    m_commas{std::move(commas)},
    m_close{std::move(close)} {}

auto TypeParamList::operator==(const TypeParamList& rhs) const noexcept -> bool {
  return m_params == rhs.m_params;
}

auto TypeParamList::operator!=(const TypeParamList& rhs) const noexcept -> bool {
  return !TypeParamList::operator==(rhs);
}

auto TypeParamList::operator[](unsigned int i) const -> const Type& {
  if (i >= m_params.size()) {
    throw std::out_of_range{"No parameter at given index"};
  }
  return m_params[i];
}

auto TypeParamList::getCount() const -> unsigned int { return m_params.size(); }

auto TypeParamList::begin() const -> Iterator { return m_params.begin(); }

auto TypeParamList::end() const -> Iterator { return m_params.end(); }

auto TypeParamList::getSpan() const -> input::Span {
  return input::Span::combine(m_open.getSpan(), m_close.getSpan());
}

auto TypeParamList::getOpen() const -> const lex::Token& { return m_open; }

auto TypeParamList::getTypes() const -> const std::vector<Type>& { return m_params; }

auto TypeParamList::getCommas() const -> const std::vector<lex::Token>& { return m_commas; }

auto TypeParamList::getClose() const -> const lex::Token& { return m_close; }

auto TypeParamList::validate() const -> bool {
  if (m_open.getKind() != lex::TokenKind::SepOpenCurly) {
    return false;
  }
  if (m_params.empty()) {
    return false;
  }
  if (!std::all_of(m_params.begin(), m_params.end(), [](const auto& t) { return t.validate(); })) {
    return false;
  }
  if (m_commas.size() != m_params.size() - 1) {
    return false;
  }
  if (m_close.getKind() != lex::TokenKind::SepCloseCurly) {
    return false;
  }
  return true;
}

auto operator<<(std::ostream& out, const TypeParamList& rhs) -> std::ostream& {
  out << '{';
  for (auto i = 0U; i < rhs.m_params.size(); ++i) {
    if (i != 0) {
      out << ',';
    }
    out << rhs.m_params[i];
  }
  return out << '}';
}

} // namespace parse
