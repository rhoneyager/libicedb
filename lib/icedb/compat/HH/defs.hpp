#pragma once
#ifndef HH_H_DEFS
#define HH_H_DEFS
#define __STDC_WANT_LIB_EXT1__ 1
#include <stddef.h>

#if defined(__cplusplus) || defined(c_plusplus)
# define HH_BEGIN_DECL
# define HH_END_DECL
#else
# define HH_BEGIN_DECL
# define HH_END_DECL
#endif

#if defined(__cplusplus) || defined(c_plusplus)
# define HH_BEGIN_DECL_C     extern "C" {
# define HH_END_DECL_C       }
# define HH_CALL_C extern "C"
#else
# define HH_BEGIN_DECL_C
# define HH_END_DECL_C
# define HH_CALL_C
#endif

#if defined(__cplusplus) || defined(c_plusplus)
# define HH_BEGIN_DECL_CPP     
# define HH_END_DECL_CPP
# define HH_CALL_CPP
#else
# define HH_BEGIN_DECL_CPP extern "CPP" {
# define HH_END_DECL_CPP }
# define HH_CALL_CPP extern "CPP"
#endif

HH_BEGIN_DECL_C

/* Compiler and version diagnostics */

/* Detection of the operating system and compiler version. Used to declare symbol export / import. */

/* Declare the feature sets that are supported */
//#define HH_FEATURE_GZIP 0 /* Auto-detection of gzip. Needed for hdf5 file storage. */


/* Symbol export / import macros */
#if defined _MSC_FULL_VER
#define COMPILER_EXPORTS_VERSION_A_HH
#elif defined __INTEL_COMPILER
#define COMPILER_EXPORTS_VERSION_B_HH
#elif defined __GNUC__
#define COMPILER_EXPORTS_VERSION_B_HH
#elif defined __MINGW32__
#define COMPILER_EXPORTS_VERSION_B_HH
#elif defined __clang__
#define COMPILER_EXPORTS_VERSION_B_HH
#else
#define COMPILER_EXPORTS_VERSION_UNKNOWN
#endif

// Defaults for static libraries
#define SHARED_EXPORT_HH
#define SHARED_IMPORT_HH
#define HIDDEN_HH
#define PRIVATE_HH

#if defined COMPILER_EXPORTS_VERSION_A_HH
#undef SHARED_EXPORT_HH
#undef SHARED_IMPORT_HH
#define SHARED_EXPORT_HH __declspec(dllexport)
#define SHARED_IMPORT_HH __declspec(dllimport)
#elif defined COMPILER_EXPORTS_VERSION_B_HH
#undef SHARED_EXPORT_HH
#undef SHARED_IMPORT_HH
#undef HIDDEN_HH
#undef PRIVATE_HH
#define SHARED_EXPORT_HH __attribute__ ((visibility("default")))
#define SHARED_IMPORT_HH __attribute__ ((visibility("default")))
#define HIDDEN_HH __attribute__ ((visibility("hidden")))
#define PRIVATE_HH __attribute__ ((visibility("internal")))
#else
#pragma message("HDFforHumans defs.hpp warning: compiler is unrecognized")
#endif

// OS definitions
#ifdef __unix__
#ifdef __linux__
#define HH_OS_LINUX
#endif
#if defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__bsdi__) || defined(__DragonFly__) || defined(__APPLE__)
#define HH_OS_UNIX
#endif
#endif
#ifdef _WIN32
#define HH_OS_WINDOWS
#endif
#if !defined(_WIN32) && !defined(HH_OS_UNIX) && !defined(HH_OS_LINUX)
#define HH_OS_UNSUPPORTED
#endif


// If SHARED_(libname) is defined, then the target library both 
// exprts and imports. If not defined, then it is a static library.

// Macros defined as EXPORTING_(libname) are internal to the library.
// They indicate that SHARED_EXPORT_SDBR should be used.
// If EXPORTING_ is not defined, then SHARED_IMPORT_SDBR should be used.

#if SHARED_HDFforHumans
#if EXPORTING_HDFforHumans
#define DL_HH SHARED_EXPORT_HH
#else
#define DL_HH SHARED_IMPORT_HH
#endif
#else
#define DL_HH SHARED_EXPORT_HH
#endif
/* Symbol export / import macros */
#define HH_SYMBOL_SHARED DL_HH
#define HH_SYMBOL_PRIVATE HIDDEN_HH


#ifdef _MSC_FULL_VER
#define HH_DEBUG_FSIG __FUNCSIG__
#endif
#if defined(__GNUC__) || defined(__clang__)
#if defined(__PRETTY_FUNCTION__)
#define HH_DEBUG_FSIG __PRETTY_FUNCTION__
#elif defined(__func__)
#define HH_DEBUG_FSIG __func__
#elif defined(__FUNCTION__)
#define HH_DEBUG_FSIG __FUNCTION__
#else
#define HH_DEBUG_FSIG ""
#endif
#endif

#define HH_WIDEN2(x) L ## x
#define HH_WIDEN(x) HH_WIDEN2(x)
/* Global exception raising code (invokes debugger) */
HH_SYMBOL_SHARED void HH_DEBUG_RAISE_EXCEPTION_HANDLER_A(const char*, int, const char*);
HH_SYMBOL_SHARED void HH_DEBUG_RAISE_EXCEPTION_HANDLER_WC(const wchar_t*, int, const wchar_t*);
#define HH_DEBUG_RAISE_EXCEPTION() HH_DEBUG_RAISE_EXCEPTION_HANDLER_WC( HH_WIDEN(__FILE__), (int)__LINE__, HH_WIDEN(HH_DEBUG_FSIG));

#define HH_DEBUG_RAISE_EXCEPTION_HANDLER HH_DEBUG_RAISE_EXCEPTION_HANDLER_A
/* Global error codes. */
#define HH_GLOBAL_ERROR_TODO 999
#if defined(__STDC_LIB_EXT1__) || defined(__STDC_SECURE_LIB__)
#define HH_USING_SECURE_STRINGS 1
#define HH_DEFS_COMPILER_HAS_FPRINTF_S
#define HH_DEFS_COMPILER_HAS_FPUTS_S
#else
#define _CRT_SECURE_NO_WARNINGS
#endif

/* Thread local storage stuff */
#ifdef _MSC_FULL_VER
#define HH_THREAD_LOCAL __declspec( thread )
#endif
#ifdef __GNUC__
#define HH_THREAD_LOCAL __thread
#endif

// Compiler interface warning suppression
#if defined _MSC_FULL_VER
#pragma warning(push)
#pragma warning( disable : 4003 ) // Bug in boost with VS2016.3
#pragma warning( disable : 4251 ) // DLL interface
#pragma warning( disable : 4275 ) // DLL interface
#endif

/// Pointer to an object that is modfied by a function
#define HH_OUT
/// Denotes an 'optional' parameter (one which can be replaced with a NULL or nullptr)
#define HH_OPTIONAL

HH_END_DECL_C

// Errata:

#ifndef HH_NO_ERRATA
// Boost bug with C++17 requires this define. See https://stackoverflow.com/questions/41972522/c2143-c2518-when-trying-to-compile-project-using-boost-multiprecision
#ifndef _HAS_AUTO_PTR_ETC
#define _HAS_AUTO_PTR_ETC	(!_HAS_CXX17)
#endif /* _HAS_AUTO_PTR_ETC */
#endif

#endif
