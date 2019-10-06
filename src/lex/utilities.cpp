#include "lex/utilities.hpp"
#include <vector>

namespace lex {

using vecItr = std::vector<Token>::iterator;
using vecConstItr = std::vector<Token>::const_iterator;
using optTok = std::optional<Token>;

// Explicit instantiations.
template auto findToken<vecItr>(vecItr begin, vecItr end, int pos) -> optTok;
template auto findToken<vecConstItr>(vecConstItr begin, vecConstItr end, int pos) -> optTok;

} // namespace lex
