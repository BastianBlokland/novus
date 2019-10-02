#include "lex/keyword.hpp"

namespace lex {

auto operator<<(std::ostream& out, const Keyword& rhs) -> std::ostream& {
  switch (rhs) {
  case Keyword::Function:
    out << "fun";
    break;
  }
  return out;
}

} // namespace lex
