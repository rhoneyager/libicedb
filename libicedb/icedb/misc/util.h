#pragma once
#ifndef ICEDB_H_UTIL
#define ICEDB_H_UTIL
#include "../defs.h"
#include "../dlls/linking.h"
#include <string.h>
ICEDB_BEGIN_DECL_C

DL_ICEDB size_t ICEDB_COMPAT_strncpy_s(char *dest, size_t destSz, const char* src, size_t srcSz);
DL_ICEDB char* ICEDB_COMPAT_strdup_s(const char* src, size_t srcSz);

DL_ICEDB size_t ICEDB_COMPAT_wcsncpy_s(wchar_t *dest, size_t destSz, const wchar_t* src, size_t srcSz);
DL_ICEDB wchar_t* ICEDB_COMPAT_wcsdup_s(const wchar_t* src, size_t srcSz);


	/** Safe file stream printf **/
#ifdef ICEDB_DEFS_COMPILER_HAS_FPRINTF_S
#define ICEDB_COMPAT_fprintf_s fprintf_s
#define ICEDB_COMPAT_fwprintf_s fwprintf_s
#else
#define ICEDB_COMPAT_fprintf_s fprintf
#define ICEDB_COMPAT_fwprintf_s fwprintf
#endif

	/** Save file stream fputs **/
#ifdef ICEDB_DEFS_COMPILER_HAS_FPUTS_S
#define ICEDB_COMPAT_fputs_s fputs
#define ICEDB_COMPAT_fputws_s fputws
#else
#define ICEDB_COMPAT_fputs_s fputs
#define ICEDB_COMPAT_fputws_s fputws
#endif

#ifdef ICEDB_DEFS_COMPILER_HAS_STRNLEN_S
#define ICEDB_COMPAT_wcsnlen_s wcsmlen_s
#define ICEDB_COMPAT_strnlen_s strnlen_s
#else
#define ICEDB_COMPAT_wcsnlen_s wcsnlen_s
#define ICEDB_COMPAT_strnlen_s strnlen
#endif

ICEDB_END_DECL_C

// This and the implementation both go in separate files.

ICEDB_DLL_INTERFACE_BEGIN(ICEDB_core_util)
ICEDB_DLL_INTERFACE_DECLARE_FUNCTION(ICEDB_core_util, strncpy_s, size_t, char *, size_t, const char*, size_t)
ICEDB_DLL_INTERFACE_DECLARE_FUNCTION(ICEDB_core_util, strdup_s, char*, const char *, size_t)
ICEDB_DLL_INTERFACE_END

#define ICEDB_core_util_impl \
	ICEDB_DLL_INTERFACE_IMPLEMENTATION_BEGIN(ICEDB_core_util); \
	ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION(ICEDB_core_util, strncpy_s, "ICEDB_COMPAT_strncpy_s", size_t, char *, size_t, const char*, size_t); \
	ICEDB_DLL_INTERFACE_IMPLEMENTATION_SYMBOL_FUNCTION(ICEDB_core_util, strdup_s, "ICEDB_COMPAT_strdup_s", char*, const char *, size_t); \
	ICEDB_DLL_INTERFACE_IMPLEMENTATION_CONSTRUCTOR(ICEDB_core_util); \
	ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(ICEDB_core_util, strncpy_s, size_t, char *, size_t, const char*, size_t); \
	ICEDB_DLL_INTERFACE_IMPLEMENTATION_FUNCTION(ICEDB_core_util, strdup_s, char*, const char *, size_t); \
	ICEDB_DLL_INTERFACE_IMPLEMENTATION_END(ICEDB_core_util);



#endif
