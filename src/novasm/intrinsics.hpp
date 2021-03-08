#pragma once

// Attribute to the disable a specific sanitizer check.
#if defined(__clang__)

#define NO_SANITIZE(n) __attribute__((no_sanitize(#n)))

#else // !defined(__clang__)

#define NO_SANITIZE(n)

#endif // !defined(__clang__)
