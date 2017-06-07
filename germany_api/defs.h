#pragma once
#ifndef ICEDB_H_DEFS
#define ICEDB_H_DEFS
#define __STDC_WANT_LIB_EXT1__ 1
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Compiler and version diagnostics */

/* Detection of the operating system and compiler version. Used to declare symbol export / import. */

/* Declare the feature sets that are supported */
#define ICEDB_FEATURE_GZIP 0 /* Auto-detection of gzip. Needed for hdf5 file storage. */

/* Symbol export / import macros */
#define ICEDB_SYMBOL_SHARED
#define ICEDB_SYMBOL_PRIVATE


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
#ifdef __cplusplus
}
#endif

#endif
