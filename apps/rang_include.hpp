#if defined(__GNUG__) && defined(_WIN32)

// Suppress the 'cast-function-type' warning in the rang header. Reason is on MinGW this fails.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-function-type"
#include "rang.hpp"
#pragma GCC diagnostic pop

#else // !__GNUG__ || !_WIN32

#include "rang.hpp"

#endif
