#pragma once

// If the platform's standard library has the <charconv> header this includes it and sets the
// 'HAS_CHAR_CONV' define.

#if __has_include(<charconv>)

#include <charconv>
#define HAS_CHAR_CONV

#endif
