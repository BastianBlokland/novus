#include "parse/type_substitution_list.hpp"
#include "utilities.hpp"

namespace parse {

static auto getIdOrErr(const lex::Token& token) {
  return getId(token).value_or(std::string("err"));
}

TypeSubstitutionList::TypeSubstitutionList(
    lex::Token open,
    std::vector<lex::Token> subs,
    std::vector<lex::Token> commas,
    lex::Token close,
    bool missingComma) :
    m_open{std::move(open)},
    m_subs{std::move(subs)},
    m_commas{std::move(commas)},
    m_close{std::move(close)},
    m_missingComma{missingComma} {}

auto TypeSubstitutionList::operator==(const TypeSubstitutionList& rhs) const noexcept -> bool {
  return m_subs == rhs.m_subs;
}

auto TypeSubstitutionList::operator!=(const TypeSubstitutionList& rhs) const noexcept -> bool {
  return !TypeSubstitutionList::operator==(rhs);
}

auto TypeSubstitutionList::begin() const -> Iterator { return m_subs.begin(); }

auto TypeSubstitutionList::end() const -> Iterator { return m_subs.end(); }

auto TypeSubstitutionList::getCount() const -> size_t { return m_subs.size(); }

auto TypeSubstitutionList::getSpan() const -> input::Span {
  return input::Span::combine(m_open.getSpan(), m_close.getSpan());
}

auto TypeSubstitutionList::getOpen() const -> const lex::Token& { return m_open; }

auto TypeSubstitutionList::getSubs() const -> const std::vector<lex::Token>& { return m_subs; }

auto TypeSubstitutionList::getCommas() const -> const std::vector<lex::Token>& { return m_commas; }

auto TypeSubstitutionList::getClose() const -> const lex::Token& { return m_close; }

auto TypeSubstitutionList::validate() const -> bool {
  if (m_open.getKind() != lex::TokenKind::SepOpenCurly) {
    return false;
  }
  if (m_subs.empty()) {
    return false;
  }
  if (!std::all_of(m_subs.begin(), m_subs.end(), [](const auto& a) {
        return a.getKind() == lex::TokenKind::Identifier;
      })) {
    return false;
  }
  if (m_missingComma || m_commas.size() != m_subs.size() - 1) {
    return false;
  }
  if (m_close.getKind() != lex::TokenKind::SepCloseCurly) {
    return false;
  }
  return true;
}

auto TypeSubstitutionList::hasMissingComma() const -> bool { return m_missingComma; }

auto operator<<(std::ostream& out, const TypeSubstitutionList& rhs) -> std::ostream& {
  out << '{';
  for (auto i = 0U; i < rhs.m_subs.size(); ++i) {
    if (i != 0) {
      out << ',';
    }
    out << getIdOrErr(rhs.m_subs[i]);
  }
  return out << '}';
}

} // namespace parse
