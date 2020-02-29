#include "lex/utilities.hpp"
#include <vector>

namespace lex {

using VecItr      = std::vector<Token>::iterator;
using VecConstItr = std::vector<Token>::const_iterator;
using OptTok      = std::optional<Token>;

// Explicit instantiations.
template auto findToken<VecItr>(VecItr begin, VecItr end, unsigned int pos) -> OptTok;
template auto findToken<VecConstItr>(VecConstItr begin, VecConstItr end, unsigned int pos)
    -> OptTok;

} // namespace lex
