#include "lex/token_payload_lit_char.hpp"
#include "input/char_escape.hpp"

namespace lex {

auto LitCharTokenPayload::print(std::ostream& out) const -> std::ostream& {
  out << '\'';
  const auto escaped = input::escape(m_val);
  if (escaped) {
    out << '\\' << *escaped;
  } else {
    out << m_val;
  }
  return out << '\'';
}

} // namespace lex
