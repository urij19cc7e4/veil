#pragma once

#include "ffi.h"

// #define __FFI_NONE
// #define __FFI_AIX
// #define __FFI_ARC64
// #define __FFI_ARCOMPACT
// #define __FFI_COMPAT_GCC_SYSV
// #define __FFI_COMPAT_LINUX
// #define __FFI_COMPAT_LINUX64
// #define __FFI_COMPAT_LINUX_SOFT_FLOAT
// #define __FFI_COMPAT_SYSV
// #define __FFI_DARWIN
// #define __FFI_EABI
// #define __FFI_EFI64
// #define __FFI_ELFBSD
// #define __FFI_FASTCALL
// #define __FFI_GNUW64
// #define __FFI_LINUX
// #define __FFI_LP64D
// #define __FFI_LP64F
// #define __FFI_LP64S
// #define __FFI_MS_CDECL
// #define __FFI_N32
// #define __FFI_N32_SOFT_FLOAT
// #define __FFI_N64
// #define __FFI_N64_SOFT_FLOAT
// #define __FFI_O32
// #define __FFI_O32_SOFT_FLOAT
// #define __FFI_OBSD
// #define __FFI_OSF
// #define __FFI_PA32
// #define __FFI_PA64
// #define __FFI_PASCAL
// #define __FFI_REGISTER
// #define __FFI_STDCALL
// #define __FFI_SYSV
// #define __FFI_THISCALL
// #define __FFI_UNIX
// #define __FFI_UNIX64
// #define __FFI_V8
// #define __FFI_V9
// #define __FFI_VFP
// #define __FFI_WASM32
// #define __FFI_WASM32_EMSCRIPTEN
// #define __FFI_WIN64

#if defined(X86_WIN32)
#define __FFI_FASTCALL
#define __FFI_MS_CDECL
#define __FFI_PASCAL
#define __FFI_REGISTER
#define __FFI_STDCALL
#define __FFI_SYSV
#define __FFI_THISCALL
#elif defined(X86_WIN64)
#define __FFI_WIN64
#else
#define __FFI_FASTCALL
#define __FFI_PASCAL
#define __FFI_REGISTER
#define __FFI_STDCALL
#define __FFI_SYSV
#define __FFI_THISCALL
#define __FFI_UNIX64
#endif