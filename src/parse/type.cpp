#include "parse/type.hpp"
#include "lex/token_payload_static_int.hpp"
#include "parse/type_param_list.hpp"
#include "utilities.hpp"
#include <sstream>

namespace parse {

static auto getIdOrErr(const lex::Token& token) {
  switch (token.getKind()) {
  case lex::TokenKind::Identifier: {
    return *getId(token);
  }
  case lex::TokenKind::Keyword: {
    std::ostringstream oss;
    oss << *getKw(token);
    return oss.str();
  }
  case lex::TokenKind::StaticInt: {
    std::ostringstream oss;
    oss << "#" << token.getPayload<lex::StaticIntTokenPayload>()->getValue();
    return oss.str();
  }
  default:
    return std::string("err");
  }
}

Type::Type(lex::Token id) : m_id{std::move(id)}, m_paramList{nullptr} {}

Type::Type(lex::Token id, TypeParamList paramList) :
    m_id{std::move(id)}, m_paramList{new TypeParamList{std::move(paramList)}} {}

Type::Type(const Type& other) noexcept : m_id{other.m_id}, m_paramList{nullptr} {
  if (other.m_paramList) {
    m_paramList = new TypeParamList{*other.m_paramList};
  }
}

Type::Type(Type&& other) noexcept : m_id{std::move(other.m_id)}, m_paramList(other.m_paramList) {
  other.m_paramList = nullptr;
}

Type::~Type() { delete m_paramList; }

auto Type::operator=(const Type& other) noexcept -> Type& {
  if (this != &other) {
    m_id = other.m_id;
    delete m_paramList;
    if (other.m_paramList) {
      m_paramList = new TypeParamList{*other.m_paramList};
    } else {
      m_paramList = nullptr;
    }
  }
  return *this;
}

auto Type::operator=(Type&& other) noexcept -> Type& {
  if (this != &other) {
    m_id = std::move(other.m_id);
    delete m_paramList;
    m_paramList       = other.m_paramList;
    other.m_paramList = nullptr;
  }
  return *this;
}

auto Type::operator==(const Type& rhs) const noexcept -> bool {
  if (m_id != rhs.m_id) {
    return false;
  }
  if ((m_paramList == nullptr) != (rhs.m_paramList == nullptr)) {
    return false;
  }
  return m_paramList == nullptr || *m_paramList == *rhs.m_paramList;
}

auto Type::operator!=(const Type& rhs) const noexcept -> bool { return !Type::operator==(rhs); }

auto Type::getSpan() const -> input::Span {
  if (m_paramList) {
    return input::Span::combine(m_id.getSpan(), m_paramList->getSpan());
  }
  return m_id.getSpan();
}

auto Type::getId() const -> const lex::Token& { return m_id; }

auto Type::isStaticInt() const -> bool { return m_id.getKind() == lex::TokenKind::StaticInt; }

auto Type::getParamList() const -> const TypeParamList* { return m_paramList; }

auto Type::getParamCount() const -> unsigned int {
  return m_paramList ? m_paramList->getCount() : 0;
}

auto Type::validate() const -> bool {
  switch (m_id.getKind()) {
  case lex::TokenKind::Identifier:
  case lex::TokenKind::Keyword:
  case lex::TokenKind::StaticInt:
    break;
  default:
    return false;
  }
  if (m_paramList && !m_paramList->validate()) {
    return false;
  }
  return true;
}

auto operator<<(std::ostream& out, const Type& rhs) -> std::ostream& {
  out << getIdOrErr(rhs.m_id);
  if (rhs.m_paramList) {
    out << *rhs.m_paramList;
  }
  return out;
}

} // namespace parse
