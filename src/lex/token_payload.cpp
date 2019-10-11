#include "lex/token_payload.hpp"
#include "char_escape.hpp"

namespace lex {

auto LitStringTokenPayload::print(std::ostream& out) const -> std::ostream& {
  for (const char c : m_val) {
    const auto escaped = escape(c);
    if (escaped) {
      out << '\\' << escaped.value();
    } else {
      out << c;
    }
  }
  return out;
}

} // namespace lex
