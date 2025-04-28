#pragma once

#define DEEPINLINE

// #define OPINTCHECK

// #define FETCHCHECK
// #define STACKCHECK

// #define FLOATCHECK
// #define MTYPECHECK
// #define OPRNDCHECK

// #define MSVCBTSWAP
// #define MSVCROTATE

#if (defined(_MSC_VER) && (defined(_M_ARM) || defined(_M_ARM64) || defined(_M_ARM64EC) || defined(_M_IX86) || defined(_M_X64)))
#define MSVCBTSWAP
#define MSVCROTATE
#endif