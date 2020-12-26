#pragma once

// Hints to the compiler that a branch is likely / unlikely to be taken.
#if defined(__clang__) || defined(__GNUG__)

#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

#else // !defined(__clang__) && !defined(__GNUG__)

#define likely(x) x
#define unlikely(x) x

#endif // !defined(__clang__) && !defined(__GNUG__)

// Attribute to the disable a specific sanitizer check.
#if defined(__clang__)

#define NO_SANITIZE(n) __attribute__((no_sanitize(#n)))

#else // !defined(__clang__)

#define NO_SANITIZE(n)

#endif // !defined(__clang__)
