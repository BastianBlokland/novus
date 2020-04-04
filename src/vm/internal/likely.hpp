#pragma once

#if defined(__clang__) || defined(__GNUG__)

// Hint to the compiler that a branch is likely to be taken.
#define likely(x) __builtin_expect(!!(x), 1)

// Hint to the compiler that a branch is unlikely to be taken.
#define unlikely(x) __builtin_expect(!!(x), 0)

#else

#define likely(x) x

#define unlikely(x) x

#endif
