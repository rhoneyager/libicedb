#pragma once
#ifndef ICEDB_H_DEFS
#define ICEDB_H_DEFS
#define __STDC_WANT_LIB_EXT1__ 1
#include <stddef.h>

#if defined(__cplusplus) || defined(c_plusplus)
# define ICEDB_BEGIN_DECL
# define ICEDB_END_DECL
#else
# define ICEDB_BEGIN_DECL
# define ICEDB_END_DECL
#endif

#if defined(__cplusplus) || defined(c_plusplus)
# define ICEDB_BEGIN_DECL_C     extern "C" {
# define ICEDB_END_DECL_C       }
# define ICEDB_CALL_C extern "C"
#else
# define ICEDB_BEGIN_DECL_C
# define ICEDB_END_DECL_C
# define ICEDB_CALL_C
#endif

#if defined(__cplusplus) || defined(c_plusplus)
# define ICEDB_BEGIN_DECL_CPP     
# define ICEDB_END_DECL_CPP
# define ICEDB_CALL_CPP
#else
# define ICEDB_BEGIN_DECL_CPP extern "CPP" {
# define ICEDB_END_DECL_CPP }
# define ICEDB_CALL_CPP extern "CPP"
#endif

ICEDB_BEGIN_DECL_C

/* Compiler and version diagnostics */

/* Detection of the operating system and compiler version. Used to declare symbol export / import. */

/* Declare the feature sets that are supported */
#define ICEDB_FEATURE_GZIP 0 /* Auto-detection of gzip. Needed for hdf5 file storage. */


/* Symbol export / import macros */
#if defined _MSC_FULL_VER
#define COMPILER_EXPORTS_VERSION_A_ICEDB
#elif defined __INTEL_COMPILER
#define COMPILER_EXPORTS_VERSION_B_ICEDB
#elif defined __GNUC__
#define COMPILER_EXPORTS_VERSION_B_ICEDB
#elif defined __MINGW32__
#define COMPILER_EXPORTS_VERSION_B_ICEDB
#elif defined __clang__
#define COMPILER_EXPORTS_VERSION_B_ICEDB
#else
#define COMPILER_EXPORTS_VERSION_UNKNOWN
#endif

// Defaults for static libraries
#define SHARED_EXPORT_ICEDB
#define SHARED_IMPORT_ICEDB
#define HIDDEN_ICEDB
#define PRIVATE_ICEDB

#if defined COMPILER_EXPORTS_VERSION_A_ICEDB
#undef SHARED_EXPORT_ICEDB
#undef SHARED_IMPORT_ICEDB
#define SHARED_EXPORT_ICEDB __declspec(dllexport)
#define SHARED_IMPORT_ICEDB __declspec(dllimport)
#elif defined COMPILER_EXPORTS_VERSION_B_ICEDB
#undef SHARED_EXPORT_ICEDB
#undef SHARED_IMPORT_ICEDB
#undef HIDDEN_ICEDB
#undef PRIVATE_ICEDB
#define SHARED_EXPORT_ICEDB __attribute__ ((visibility("default")))
#define SHARED_IMPORT_ICEDB __attribute__ ((visibility("default")))
#define HIDDEN_ICEDB __attribute__ ((visibility("hidden")))
#define PRIVATE_ICEDB __attribute__ ((visibility("internal")))
#else
#pragma message("defs.h warning: compiler is unrecognized")
#endif

// If SHARED_(libname) is defined, then the target library both 
// exprts and imports. If not defined, then it is a static library.

// Macros defined as EXPORTING_(libname) are internal to the library.
// They indicate that SHARED_EXPORT_SDBR should be used.
// If EXPORTING_ is not defined, then SHARED_IMPORT_SDBR should be used.

#if SHARED_ICEDB
#if EXPORTING_ICEDB
#define DL_ICEDB SHARED_EXPORT_ICEDB
#else
#define DL_ICEDB SHARED_IMPORT_ICEDB
#endif
#else
#define DL_ICEDB SHARED_EXPORT_ICEDB
#endif
/* Symbol export / import macros */
#define ICEDB_SYMBOL_SHARED DL_ICEDB
#define ICEDB_SYMBOL_PRIVATE HIDDEN_ICEDB


#ifdef _MSC_FULL_VER
#define ICEDB_DEBUG_FSIG __FUNCSIG__
#endif
#ifdef __GNUC__
#define ICEDB_DEBUG_FSIG __PRETTY_FUNCTION__
#endif
	/* Global exception raising code (invokes debugger) */
	ICEDB_SYMBOL_SHARED void ICEDB_DEBUG_RAISE_EXCEPTION_HANDLER(const char*, int, const char*);
#define ICEDB_DEBUG_RAISE_EXCEPTION() ICEDB_DEBUG_RAISE_EXCEPTION_HANDLER(__FILE__, (int)__LINE__,ICEDB_DEBUG_FSIG);
/* Global error codes. */
#define ICEDB_GLOBAL_ERROR_TODO 999

#if defined(__STDC_LIB_EXT1__) || defined(__STDC_SECURE_LIB__)
#define ICEDB_USING_SECURE_STRINGS 1
#define ICEDB_DEFS_COMPILER_HAS_FPRINTF_S
#define ICEDB_DEFS_COMPILER_HAS_FPUTS_S
#else
#define _CRT_SECURE_NO_WARNINGS
#endif

	/* Thread local storage stuff */
#ifdef _MSC_FULL_VER
#define ICEDB_THREAD_LOCAL __declspec( thread )
#endif
#ifdef __GNUC__
#define ICEDB_THREAD_LOCAL __thread
#endif

// Compiler interface warning suppression
#if defined _MSC_FULL_VER
#pragma warning(push)
#pragma warning( disable : 4251 )
#endif

ICEDB_END_DECL_C

#endif
