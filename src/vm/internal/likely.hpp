#pragma once

// Hint to the compiler that a branch is likely to be taken.
#define likely(x) __builtin_expect(!!(x), 1)

// Hint to the compiler that a branch is unlikely to be taken.
#define unlikely(x) __builtin_expect(!!(x), 0)
