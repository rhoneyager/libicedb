#pragma once
#ifndef BT_HPP_DEFS
#define BT_HPP_DEFS
#ifndef __STDC_WANT_LIB_EXT1__
#define __STDC_WANT_LIB_EXT1__ 1
#endif
#include <stddef.h>

/* Symbol export / import macros */
#if defined _MSC_FULL_VER
#define COMPILER_EXPORTS_VERSION_A_BT
#elif defined __INTEL_COMPILER
#define COMPILER_EXPORTS_VERSION_B_BT
#elif defined __GNUC__
#define COMPILER_EXPORTS_VERSION_B_BT
#elif defined __MINGW32__
#define COMPILER_EXPORTS_VERSION_B_BT
#elif defined __clang__
#define COMPILER_EXPORTS_VERSION_B_BT
#else
#define COMPILER_EXPORTS_VERSION_UNKNOWN
#endif

// Defaults for static libraries
#define SHARED_EXPORT_BT
#define SHARED_IMPORT_BT
#define HIDDEN_BT
#define PRIVATE_BT

#if defined COMPILER_EXPORTS_VERSION_A_BT
#undef SHARED_EXPORT_BT
#undef SHARED_IMPORT_BT
#define SHARED_EXPORT_BT __declspec(dllexport)
#define SHARED_IMPORT_BT __declspec(dllimport)
#elif defined COMPILER_EXPORTS_VERSION_B_BT
#undef SHARED_EXPORT_BT
#undef SHARED_IMPORT_BT
#undef HIDDEN_BT
#undef PRIVATE_BT
#define SHARED_EXPORT_BT __attribute__ ((visibility("default")))
#define SHARED_IMPORT_BT __attribute__ ((visibility("default")))
#define HIDDEN_BT __attribute__ ((visibility("hidden")))
#define PRIVATE_BT __attribute__ ((visibility("internal")))
#else
#pragma message("defs.h warning: compiler is unrecognized")
#endif

// OS definitions
#ifdef __unix__
# ifdef __linux__
#  define BT_OS_LINUX
# endif
# if defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__bsdi__) || defined(__DragonFly__) || defined(__APPLE__) || defined(__MACH__)
#  define BT_OS_UNIX
# endif
#endif
#if (defined(__APPLE__) || defined(__MACH__)) && !defined(BT_OS_UNIX)
# define BT_OS_MACOS
#endif
#if defined(_WIN32) || defined(__CYGWIN__)
# define BT_OS_WINDOWS
#endif
#if !defined(BT_OS_WINDOWS) && !defined(BT_OS_UNIX) && !defined(BT_OS_LINUX) && !defined(BT_OS_MACOS)
# define BT_OS_UNSUPPORTED
#endif

// If SHARED_(libname) is defined, then the target library both 
// exprts and imports. If not defined, then it is a static library.

// Macros defined as EXPORTING_(libname) are internal to the library.
// They indicate that SHARED_EXPORT_SDBR should be used.
// If EXPORTING_ is not defined, then SHARED_IMPORT_SDBR should be used.

#if SHARED_BT
#if EXPORTING_BT
#define DL_BT SHARED_EXPORT_BT
#else
#define DL_BT SHARED_IMPORT_BT
#endif
#else
#define DL_BT SHARED_EXPORT_BT
#endif
/* Symbol export / import macros */
#define BT_SYMBOL_SHARED DL_BT
#define BT_SYMBOL_PRIVATE HIDDEN_BT

// Definitions to get function signatures.
// In C99 and C++11, __func__ should work in all cases for getting the function name.
// However, I want the entire function signature.
#if defined(_MSC_FULL_VER)
# define BT_FUNCSIG __FUNCSIG__
//#elif defined(__GNUC__) || defined(__clang__)
#else
// __PRETTY_FUNCTION__ is a variable, not a preprocessor macro.
//# if defined(__PRETTY_FUNCTION__)
#  define BT_FUNCSIG __PRETTY_FUNCTION__
//# elif defined(__func__)
//#  define BT_FUNCSIG __func__
//# elif defined(__FUNCTION__)
//#  define BT_FUNCSIG __FUNCTION__
//# else
//#  define BT_FUNCSIG "Unknown Function"
//# endif
#endif

#define BT_WIDEN2(x) L ## x
#define BT_WIDEN(x) BT_WIDEN2(x)
/* Global exception raising code (invokes debugger) */
BT_SYMBOL_SHARED void BT_DEBUG_RAISE_EXCEPTION_HANDLER_A(const char*, int, const char*);
BT_SYMBOL_SHARED void BT_DEBUG_RAISE_EXCEPTION_HANDLER_WC(const wchar_t*, int, const wchar_t*);
#define BT_DEBUG_RAISE_EXCEPTION() BT_DEBUG_RAISE_EXCEPTION_HANDLER_WC( BT_WIDEN(__FILE__), (int)__LINE__, BT_WIDEN(BT_FUNCSIG));

#define BT_DEBUG_RAISE_EXCEPTION_HANDLER BT_DEBUG_RAISE_EXCEPTION_HANDLER_A
/* Global error codes. */
#define BT_GLOBAL_ERROR_TODO 999
#if defined(__STDC_LIB_EXT1__) || defined(__STDC_SECURE_LIB__)
#define BT_USING_SECURE_STRINGS 1
#define BT_DEFS_COMPILER_HAS_FPRINTF_S
#define BT_DEFS_COMPILER_HAS_FPUTS_S
#else
#define _CRT_SECURE_NO_WARNINGS
#endif

/* Thread local storage stuff */
#ifdef _MSC_FULL_VER
#define BT_THREAD_LOCAL __declspec( thread )
#endif
#ifdef __GNUC__
#define BT_THREAD_LOCAL __thread
#endif

/// Pointer to an object that is modfied by a function
#define BT_OUT
/// Denotes an 'optional' parameter (one which can be replaced with a NULL or nullptr)
#define BT_OPTIONAL

// Error inheritance
# ifndef BT_ERROR_INHERITS_FROM
#  define BT_ERROR_INHERITS_FROM std::exception
# endif

#endif
