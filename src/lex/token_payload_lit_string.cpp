#include "lex/token_payload_lit_string.hpp"
#include "input/char_escape.hpp"

namespace lex {

auto LitStringTokenPayload::print(std::ostream& out) const -> std::ostream& {
  return out << '\"' << input::escape(m_val) << '\"';
}

} // namespace lex
