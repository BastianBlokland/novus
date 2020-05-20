#pragma once

namespace gsl {

// Marker to indicate a variable owns certain memory.
// Used for the cppcoreguidelines static analyzer:
// https://clang.llvm.org/extra/clang-tidy/checks/cppcoreguidelines-owning-memory.html
template <typename T>
using owner = T;

} // namespace gsl
